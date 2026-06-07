#pragma once

#include <spi_display.h>

#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 280
#define DISPLAY_OFFSET_X 0
#define DISPLAY_OFFSET_Y 20

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes spi bus, should be called before display_create()
 */
bool display_init_spi(_i8 mosi, _i8 clk, _i8 dc, _i8 res);

/**
 * Creates new display device and add it to spi bus
 */
bool display_add(spi_display_t* dev, const _i8 cs, const _i8 dc,
                 const _i8 res_gpio, const _i8 bl);

bool display_configure(spi_display_t* dev, const _u16 width, const _u16 height,
                       const _i8 offset_x, const _i8 offset_y,
                       const rotation_t rotation);

#ifdef __cplusplus
}
#endif