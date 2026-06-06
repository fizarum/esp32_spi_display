#pragma once

#include "font.h"
#include "spi_display.h"
#include "symbol_data.h"

#ifdef __cplusplus
extern "C" {
#endif

// basic api
void gfx_draw_pixels(const spi_display_t* dev, _u16 l, _u16 t, _u16 r, _u16 b,
                     const _u16* colors, size_t size);

// drawing api
void gfx_clear(const spi_display_t* dev, const _u16 color);

void gfx_fill_rect(const spi_display_t* dev, _u16 l, _u16 t, _u16 r, _u16 b,
                   _u16 color);

void gfx_draw_rect(const spi_display_t* dev, _u16 l, _u16 t, _u16 r, _u16 b,
                   _u16 color);

void gfx_draw_h_line(const spi_display_t* dev, const _u16 l, const _u16 t,
                     const _u16 length, const _u16 color);

void gfx_draw_v_line(const spi_display_t* dev, const _u16 l, const _u16 t,
                     const _u16 length, const _u16 color);

void gfx_draw_line(const spi_display_t* dev, _u16 l, _u16 t, _u16 r, _u16 b,
                   const _u16 color);

void gfx_draw_circle(const spi_display_t* dev, const _u16 c_x, const _u16 c_y,
                     const _u16 radius, const _u16 color);

void gfx_fill_circle(const spi_display_t* dev, const _u16 c_x, const _u16 c_y,
                     const _u16 radius, const _u16 color);

// draws unclosed circle with start and end angle
void gfx_draw_unclosed_circle(const spi_display_t* dev, const _u16 c_x,
                              const _u16 c_y, const _u16 radius,
                              const _u16 start_angle, const _u16 end_angle,
                              const _u16 color);

void gfx_draw_gauge(const spi_display_t* dev, const _u16 c_x, const _u16 c_y,
                    const _u16 radius, const _u16 start_angle,
                    const _u16 end_angle, const _u16 color);

/** @brief Draws part of gauge  */
void gfx_draw_gauge_segment(const spi_display_t* dev, const _u16 c_x,
                            const _u16 c_y, const _u16 radius,
                            const _u16 start_angle, const _u16 end_angle,
                            const _u16 color);

// text api
_u8 gfx_draw_char(const spi_display_t* dev, symbol_data_t* symbol,
                  const _u16 left, const _u16 top, const font_t* font,
                  const _u16 color);

_u16 gfx_draw_string(const spi_display_t* dev, symbol_data_t** symbols,
                     const _u16 len, const _u16 left, const _u16 top,
                     const font_t* font, const _u16 color);

#ifdef __cplusplus
}
#endif