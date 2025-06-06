#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <lcom/lcf.h>
#include <lcom/vbe.h>

#include <stdint.h>

// Resolution and buffer info
static uint8_t bits_per_pixel;
static uint32_t vram_base;
static uint32_t vram_size;

// Frame buffer and mode info
extern uint8_t *frame_buffer;
extern uint8_t *video_mem;
extern vbe_mode_info_t mode_info;

// Grid positioning
extern int grid_origin_x;
extern int grid_origin_y;
void vg_set_grid_position(int grid_cols, int grid_rows, int cell_width, int cell_height);

// Core graphics API
int set_graphic_mode(uint16_t mode);
int set_frame_buffer(uint16_t mode);
void* my_vg_init(uint16_t mode);
int vg_clear_screen(uint32_t color);
int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color);
int draw_rectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
void swap_buffers();

// Drawing functions
int draw_pixel_indexed(uint16_t x, uint16_t y, uint8_t color_index);
void draw_xpm_indexed(xpm_map_t xpm, int x, int y);

#endif // GRAPHICS_H
