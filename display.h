#pragma once

#include <spi_display.h>

#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 280
#define DISPLAY_OFFSET_X 0
#define DISPLAY_OFFSET_Y 20

/**
 * Initializes spi bus, should be called before display_create()
 */
bool display_init_spi(_i8 mosi, _i8 clk, _i8 dc, _i8 res);

/**
 * Creates new display device and add it to spi bus
 */
bool display_create(spi_display_t* dev, const _i8 cs, const _i8 dc,
                    const _i8 res_gpio, const _i8 bl);
