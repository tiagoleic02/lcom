#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <stdint.h>
#include "i8254.h"

int hook_id = TIMER0_IRQ;  // Usar o IRQ do Timer 0
int counter = 0;

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  if (timer < 0 || timer > 2 || st == NULL) return 1; //Certificar que os argumentos são todos válidos

  //Preparar comando Read-Back para ler o status do timer especifico
  uint8_t cmd = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);

  //Enviar comando para o registo de controlo
  if (sys_outb(TIMER_CTRL, cmd) != 0) return 1;

  //Ler o valor do timer selecionado
  if(util_sys_inb(TIMER_0 + timer, st) != 0) return 1;

  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {
  if (timer < 0 || timer > 2) return 1; // certificar que os argumentos são todos válidos

  union timer_status_field_val config; //union é uma struct em c; esta union está definida em LCF

  switch(field) {
    case tsf_all: //Se o user quer ver a configuração toda, copiamos os 8 bits do status.
      config.byte = st;
      break;
    case tsf_initial: //Ver os bits do modo de inicialização
      config.in_mode = (st & TIMER_LSB_MSB) >> 4;
      /* TIMER_LSB_MSB é uma máscara que contém os bits 5 e 4 (0x30 ou 0b00110000)
      Fazemos (st & TIMER_LSB_MSB) para isolar esses bits
      Em seguida, deslocamos 4 bits à direita (>> 4) para que o valor fique entre 0 e 3 */
      break;
    case tsf_mode: //Modo de operação
      st = (st & (BIT(3) | BIT(2) | BIT(1))) >> 1;  // Extrai os bits 3-1 para o modo de contagem, isolando os 3 bits correspondentes e deslocando 1 posição para a direita para extrair o modo de contagem correto
      if (st == 6) {
        config.count_mode = 2;  // Ajusta 6 para 2
      } else if (st == 7) {
        config.count_mode = 3;  // Ajusta 7 para 3
      } else config.count_mode = st;
      break;
    case tsf_base:
      config.bcd = st & TIMER_BCD;  // Verifica o bit 0 para saber se é BCD ou binário
      break;
    default:
      return 1;
  }
  //Exibir a informação formatada
  if ((timer_print_config(timer, field, config)) != 0) return 1;

  return 0;
}

int (timer_set_frequency)(uint8_t timer, uint32_t TIMER_freq) {
  if (timer < 0 || timer > 2) return 1;
  if (TIMER_freq < 19 || TIMER_freq > CPU_FREQ) return 1; //TIMER_freq não pode ser menor que 19 senão gera-se um valor com mais de 16 bits

  //Ler a configuração atual para preservar alguns bits
  uint8_t st;
  if ((timer_get_conf(timer, &st)) != 0) return 1;

  // Preparar o comando para configurar o timer
  uint8_t ctrl_word = (st & 0x0F) | TIMER_LSB_MSB;  // Preservar os 4 bits inferiores e definir modo de acesso

  // Selecionar o timer correto
  switch (timer) {
    case 0: ctrl_word |= TIMER_SEL0; break;
    case 1: ctrl_word |= TIMER_SEL1; break;
    case 2: ctrl_word |= TIMER_SEL2; break;
  }
  //Enviar o comando para o registo de controlo
  if ((sys_outb(TIMER_CTRL, ctrl_word)) != 0) return 1;

  // Calcular o valor de contagem baseado na frequência
  uint16_t initial_count = CPU_FREQ / TIMER_freq;

  //Obter o LSB e MSB do valor de contagem
  uint8_t lsb, msb;
  util_get_LSB(initial_count, &lsb);
  util_get_MSB(initial_count, &msb);

  //Enviar o valor de contagem para o timer
  if ((sys_outb(TIMER_0 + timer, lsb)) != 0) return 1;
  if ((sys_outb(TIMER_0 + timer, msb)) != 0) return 1;

  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  if (bit_no == NULL) return 1;

  *bit_no = BIT(hook_id); // Retornar o bit correspondente

  // Subscrever a interrupção e usar policy IRQ_REENABLE
  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id) != 0) return 1;
  return 0;
}

int (timer_unsubscribe_int)() {
  //remover a politica de interrupção para cancelar a subscrição das interrupções
  if ((sys_irqrmpolicy(&hook_id)) != 0) return 1;

  return 0;
}

void (timer_int_handler)() {
  counter++; //Incrementar o contador a cada interrupção
}
