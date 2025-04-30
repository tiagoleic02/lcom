#include <lcom/lcf.h>
#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>

#include "i8042.h"
#include "keyboard.h"
#include "KBC.h"
#include "timer.h"

// Variáveis globais externas
extern uint8_t scancode;
extern bool two_byte;
extern int error_flag;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

// Leitura por interrupções
int (kbd_test_scan)() {
  uint8_t bit_no;         // Variável para armazenar o número da máscara de interrupção
  int r;
  int ipc_status;         // Status das interrupções
  message msg;            // Mensagem da comunicação entre processos
  uint8_t bytes[2];       // Array para guardar até 2 bytes do scancode
  uint8_t size = 0;       // Tamanho do scancode
  bool make;              // Indica se é makecode ou breakcode

  // Subscreve a interrupção do teclado
  if (kbd_subscribe_int(&bit_no)!=0) return 1;

  // Loop até o utilizador pressionar a tecla ESC
  while (scancode != ESC_BREAKCODE) {
    // Esperar por uma notificação/interrupção
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) continue;

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          // Se a interrupção for do teclado
          if (msg.m_notify.interrupts & bit_no) {
            kbc_ih();  // Chama o handler

            if (error_flag) continue; // Ignora se houve erro

            // Se for scancode de 2 bytes
            if (two_byte) {
              bytes[0] = SCANCODE_TWO_BYTE;
              bytes[1] = scancode;
              size = 2;
              two_byte = false;
            } else {
              bytes[0] = scancode;
              size = 1;
            }

            make = !(bytes[size - 1] & BIT(7)); // Bit mais significativo indica make ou break
            kbd_print_scancode(make, size, bytes);
          }
          break;
        }
      }
    }

    // Cancelar subscrição da interrupção
    if (kbd_unsubscribe_int()!=0) return 1;

    // Mostrar o número de chamadas a sys_inb
    kbd_print_no_sysinb(counter);

    return 0;
}

// Leitura por polling
int (kbd_test_poll)() {
  uint8_t bytes[2];
  uint8_t size=0;
  bool make;

  // Loop até o utilizador pressionar a tecla ESC
  while (scancode != ESC_BREAKCODE) {
    uint8_t data;
    // Tenta ler do buffer de saída (output) do KBC
    if (kbc_read_output(KBC_OUTPUT_BUF, &data) == 0) {
      if (data == SCANCODE_TWO_BYTE) {
        // Se for scancode de 2 bytes, guarda o prefixo e lê o próximo
        two_byte=true;
      } else {
        if (two_byte) {
          bytes[0] = SCANCODE_TWO_BYTE;
          bytes[1] = data;
          size = 2;
          two_byte = false;
        } else {
          // Se for de 1 byte
          bytes[0] = data;
          size = 1;
        }
        scancode = data;  // Atualiza o scancode global
        make = !(bytes[size - 1] & BIT(7)); // Bit mais significativo indica se é makecode ou breakcode
        kbd_print_scancode(make, size, bytes);
      }
    }
    tickdelay(micros_to_ticks(KBC_DELAY_US));
   }
    // Reativar interrupções do teclado
    if (kbd_restore_interrupts()!=0) return 1;
    // Mostrar o número de chamadas a sys_inb
    kbd_print_no_sysinb(counter);
    return 0;
}

// Leitura por interrupção com timeout (timer)
int (kbd_test_timed_scan)(uint8_t n) {
  uint8_t kbd_bit, timer_bit;
  int ipc_status;
  int r;
  message msg;
  uint8_t bytes[2];
  uint8_t size = 0;
  bool make;

  counter = 0; // Reset ao contador

  // Subscrição das interrupções do teclado e do timer
  if (kbd_subscribe_int(&kbd_bit) != 0) return 1;
  if (timer_subscribe_int(&timer_bit) != 0) return 1;

  while (scancode != ESC_BREAKCODE && counter < n * 60) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) continue;

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          // Timer interrupt
          if (msg.m_notify.interrupts & timer_bit) {
            timer_int_handler(); // Incrementa o contador
          }
          // Teclado interrupt
          if (msg.m_notify.interrupts & kbd_bit) {
            kbc_ih();
            if (error_flag) continue;

            counter = 0; // Reset do timer sempre que uma tecla é lida

            if (two_byte) {
              bytes[0] = SCANCODE_TWO_BYTE;
              bytes[1] = scancode;
              size = 2;
              two_byte = false;
            } else {
              bytes[0] = scancode;
              size = 1;
            }

            make = !(bytes[size - 1] & BIT(7));
            kbd_print_scancode(make, size, bytes);
          }
          break;
      }
    }
  }

  // Cancelar subscrições
  if (kbd_unsubscribe_int() != 0) return 1;
  if (timer_unsubscribe_int() != 0) return 1;

  // Mostrar o número de chamadas a sys_inb
  kbd_print_no_sysinb(counter);

  return 0;
}
