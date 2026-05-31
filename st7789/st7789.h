#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

#include "../palette.h"
#include "../spi_display.h"

void display_init(spi_display_t* dev);

void display_draw_pixel(spi_display_t* dev, _u16 x, _u16 y, _u16 color);
void display_draw_pixels(spi_display_t* dev, _u16 l, _u16 t, _u16 r, _u16 b,
                         _u16* colors, size_t size);

void display_fill_rect(spi_display_t* dev, _u16 l, _u16 t, _u16 r, _u16 b,
                       _u16 color);
void display_clear(spi_display_t* dev, _u16 color);

void display_set_on_off(spi_display_t* dev, const bool on);
void display_sleep(spi_display_t* dev);
void display_wakeup(spi_display_t* dev);
bool display_set_inversion(spi_display_t* dev, const bool inversion);
