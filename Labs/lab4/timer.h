#ifndef TIMER_H
#define TIMER_H

#ifndef _TIMER_H_
#define _TIMER_H_

#include <lcom/lcf.h>
#include <stdint.h>
#include "i8254.h"

// Declaração da variável global counter (para usar no timed_scan)
extern int counter;
extern int hook_id;

int (timer_get_conf)(uint8_t timer, uint8_t *st);
int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field);
int (timer_set_frequency)(uint8_t timer, uint32_t freq);
int (timer_subscribe_int)(uint8_t *bit_no);
int (timer_unsubscribe_int)();
void (timer_int_handler)();

#endif /* _TIMER_H_ */

#endif //TIMER_H
