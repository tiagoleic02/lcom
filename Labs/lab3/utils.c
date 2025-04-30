#include <lcom/lcf.h>
#include <stdint.h>

extern int counter;  // variável para contar chamadas a sys_inb (usada pelo LCF)

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if (lsb == NULL) return 1; // Verificar se o apontador lsb é válido
  *lsb = (uint8_t)(val & 0xFF);  // Passar para o lsb apenas os 8 bits menos significativos de val;
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  if (msb == NULL) return 1; // Verificar se o apontador msb é válido
  *msb = (uint8_t)(val >> 8); //Deslocar 8 bits para a direita e passar tal como no lsb os restantes 8 bits do val para o msb
  return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
  if(value == NULL) return 1;
  uint32_t temp;
  counter++;
  int result = sys_inb(port, &temp); //Chamada do sys_inb para ler da porta
  *value = (uint8_t)temp; //Converter o valor de temp para uint8_t - é este o objetivo desta função!
  return result; //retornar para esta nova função o mesmo valor que a função original também retornava
}
