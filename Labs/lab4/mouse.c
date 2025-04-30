#include "mouse.h"

uint8_t packet[3];         // Pacote do rato
uint8_t current_byte;      // Último byte lido

// Subscrever interrupções do rato
int mouse_subscribe_int(uint8_t *bit_no) {
    if (bit_no == NULL) return 1; // Verifica se o ponteiro é válido
    *bit_no = BIT(mouse_hook_id);  // Define a máscara para o rato
    return sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_hook_id); // Subscreve as interrupções com o modo exclusive
}

// Desativar a subscrição das interrupções do rato
int mouse_unsubscribe_int() {
    return sys_irqrmpolicy(&mouse_hook_id); // Remove a política de interrupção
}

// Função para lidar com interrupções do rato
void (mouse_ih)() {
    kbc_read_output(KBC_OUTPUT_BUF, &current_byte); // Lê um byte do rato
}

// Função para sincronizar bytes do pacote
void mouse_sync_bytes() {
    if (byte_index == 0 && (current_byte & BIT(3))) { // Verifica se é o byte CONTROL
        packet[byte_index] = current_byte;
        byte_index++;
    } else if (byte_index > 0 && byte_index < 3) { // Recebe DELTA_X e DELTA_Y
        packet[byte_index] = current_byte;
        byte_index++;
    }
    if (byte_index == 3) { // Pacote completo
        mouse_bytes_to_packet(); // Converte o array de bytes para a struct
        byte_index = 0; // Reseta o índice para o próximo pacote
    }
}

// Função para transformar o array de bytes numa struct
void mouse_bytes_to_packet() {
    struct packet pp; // Defina a struct packet conforme a documentação
    pp.bytes[0] = packet[0];
    pp.bytes[1] = packet[1];
    pp.bytes[2] = packet[2];

    // Processar os dados do pacote
    pp.lb = pp.bytes[0] & MOUSE_LB; // Botão esquerdo
    pp.rb = pp.bytes[0] & MOUSE_RB; // Botão direito
    pp.mb = pp.bytes[0] & MOUSE_MB; // Botão do meio
    pp.delta_x = (int8_t)pp.bytes[1]; // Deslocamento em X
    pp.delta_y = (int8_t)pp.bytes[2]; // Deslocamento em Y
    pp.x_ov = pp.bytes[0] & MOUSE_X_OVERFLOW; // Overflow em X
    pp.y_ov = pp.bytes[0] & MOUSE_Y_OVERFLOW; // Overflow em Y

    // Exibir o pacote
    mouse_print_packet(&pp);
}

// Função para escrever um comando no KBC
int mouse_write_cmd(uint8_t command) {
    uint8_t attempts = 10; // Número máximo de tentativas
    uint8_t response;

    // Tenta enviar o comando
    while (attempts > 0) {
        if (kbc_write_command(KBC_CMD_REG, KBC_WRITE_TO_MOUSE) != 0) {
            return 1; // Erro ao escrever o comando
        }
        if (kbc_write_command(KBC_OUTPUT_BUF, command) != 0) {
            return 1; // Erro ao escrever o comando no buffer de saída
        }

        // Espera pela resposta do rato
        tickdelay(micros_to_ticks(KBC_DELAY_US)); // Atraso de 20ms
        if (util_sys_inb(KBC_OUTPUT_BUF, &response) != 0) {
            return 1; // Erro ao ler a resposta
        }

        if (response == MOUSE_ACK) {
            return 0; // Comando aceito
        } else if (response == MOUSE_NACK) {
            attempts--; // Tenta novamente se receber NACK
        }
    }

    return 1; // Falha após todas as tentativas
}
