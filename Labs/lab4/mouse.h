#ifndef MOUSE_H
#define MOUSE_H

#include <minix/sysutil.h>
#include <lcom/lcf.h>
#include "i8042.h"
#include "KBC.h"

static uint8_t timer_hook_id = 0;
static int mouse_hook_id = 2;        // hook_id para o rato - o valor não tem significado, só tem que ser diferente do valor do teclado e do timer
static uint8_t byte_index = 0;       // Índice do byte do pacote

extern uint8_t packet[3];            // Pacote do rato
extern uint8_t current_byte;         // Último byte lido

int (mouse_subscribe_int)(uint8_t *bit_no);
int (mouse_unsubscribe_int)();
void (mouse_ih)();
void (mouse_sync_bytes)();
void (mouse_bytes_to_packet)();
int (mouse_write_cmd)(uint8_t cmd);

#endif //MOUSE_H
