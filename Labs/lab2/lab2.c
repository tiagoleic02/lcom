#include <lcom/lcf.h>
#include <lcom/lab2.h>
#include "i8254.h"

#include <stdbool.h>
#include <stdint.h>

extern int hook_id;
int counter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab2/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab2/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(timer_test_read_config)(uint8_t timer, enum timer_status_field field) {
  if(timer < 0 || timer > 2) return 1;

  uint8_t status; //variável que vai conter a configuração do timer

  if (timer_get_conf(timer, &status) != 0) return 1;  // Lê a configuração do timer (status byte)
  if (timer_display_conf(timer, status, field) != 0) return 1;   // Chama a função para exibir a configuração, passando o timer, o status e o campo solicitado

  return 0;
}

int(timer_test_time_base)(uint8_t timer, uint32_t TIMER_freq) {
  if (timer < 0 || timer > 2) return 1;
  if (TIMER_freq < 19 || TIMER_freq > CPU_FREQ) return 1;

  //Configurar a frequência do timer
  if (timer_set_frequency(timer, TIMER_freq) != 0) return 1;

  return 0;
}

int(timer_test_int)(uint8_t time) {
  uint8_t irq_set;

  if (timer_subscribe_int(&irq_set) != 0) return 1;   //Subscrever interrupções

  //declaração de variáveis uteis para o ciclo while
  int ipc_status, r;
  message msg;
  int seconds = 0;   //Reiniciar contador que vai ser usado para o loop

  //Loop de processamento de interrupções
  while (seconds < time) {
    //Receber mensagem
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { //Notificação recebida
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: //Notificação de interrupção do hardware
          if (msg.m_notify.interrupts & irq_set) { //subscreve a interrupção
            timer_int_handler();
            if (counter % 60 == 0) {
              timer_print_elapsed_time();
              seconds++;
            }
          }
          break;
        default:
          break; //não é esperado mais nenhuma notificação
      }
    }
  }

  if ((timer_unsubscribe_int()) != 0) return 1;   //Desativação das interrupções

  return 0;
}
