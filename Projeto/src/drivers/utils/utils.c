#include <lcom/lcf.h>
#include <stdint.h>

int(util_get_LSB)(const uint16_t val, uint8_t *lsb) {
  if (lsb == NULL) return 1;
  *lsb = (uint8_t)val;
  return 0;
}

int(util_get_MSB)(const uint16_t val, uint8_t *msb) {
  if (msb == NULL) return 1;
  *msb = (uint8_t)(val >> 8);
  return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
  uint32_t val;
  if(value == NULL) return 1;
  int result = sys_inb(port, &val);
  *value = (uint8_t)val;
  return result;
}
