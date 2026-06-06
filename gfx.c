#include "gfx.h"

#include <math.h>
#include <stdlib.h>

#include "buff_utils.h"
#include "esp_log.h"
#include "palette.h"

#define GFX_TAG "GFX"
// check is bit set, starting from most significant bit
// 8-bit version
#define GFX_IS_BIT_SET8(source, position) (source & (0x80 >> position))
#define BUFFER_SIZE 1024
#define GAUGE_LINE_WIDTH 20
// tile size is 3x3
#define TILE_COLORS_COUNT 9

const float rad_to_pi_coef = 180.0 / M_PI;
const float pi_to_rad_coef = M_PI / 180.0;

DMA_ATTR static _u8 buffer[BUFFER_SIZE];
static _u16 tile_colors[TILE_COLORS_COUNT] = {};

static _u16 _x1;
static _u16 _x2;
static _u16 _y1;
static _u16 _y2;

inline float to_degrees(float radians) { return radians * rad_to_pi_coef; }
inline float to_rads(_u16 degree) { return degree * pi_to_rad_coef; }
static void calculate_x_y_on_circle(const _u16 c_x, const _u16 c_y,
                                    const _u16 angle_degree,
                                    const _u16 circle_radius, _u16* x_res,
                                    _u16* y_res);

void gfx_draw_pixels(const spi_display_t* dev, _u16 l, _u16 t, _u16 r, _u16 b,
                     const _u16* colors, size_t size) {
  dev->draw_pixels(dev, l, t, r, b, colors, size);
}

void gfx_clear(const spi_display_t* dev, const _u16 color) {
  gfx_fill_rect(dev, 0, 0, dev->width, dev->height, color);
}

// TODO: recheck if it can be moved to draw_pixels() instead
void gfx_fill_rect(const spi_display_t* dev, _u16 l, _u16 t, _u16 r, _u16 b,
                   _u16 color) {
  if (l >= dev->width) return;
  if (r > dev->width) r = dev->width;
  if (t >= dev->height) return;
  if (b > dev->height) b = dev->height;

  _x1 = l + dev->offset_x;
  _x2 = r + dev->offset_x;
  _y1 = t + dev->offset_y;
  _y2 = b + dev->offset_y;

  dev->select_region(dev, _x1, _y1, _x2 - 1, _y2 - 1);

  _u16 lines_count = _y2 - _y1;
  _u16 line_len = _x2 - _x1;

  // first fill one line and put it into buffer
  buffer_fill_u16(buffer, color, line_len);

  // now fill lines one by one to display memory
  for (_u16 line_index = 0; line_index < lines_count; line_index++) {
    dev->transmit_data(dev->spi_handle, dev->dc, buffer, line_len * 2);
  }
}

void gfx_draw_rect(const spi_display_t* dev, _u16 l, _u16 t, _u16 r, _u16 b,
                   _u16 color) {
  _u16 h_len = r - l;
  _u16 v_len = b - t;
  gfx_draw_h_line(dev, l, t, h_len, color);
  gfx_draw_h_line(dev, l, b - 1, h_len, color);

  gfx_draw_v_line(dev, l, t, v_len, color);
  gfx_draw_v_line(dev, r, t, v_len, color);
}

void gfx_draw_h_line(const spi_display_t* dev, const _u16 l, const _u16 t,
                     const _u16 length, const _u16 color) {
  gfx_fill_rect(dev, l, t, l + length, t + 1, color);
}

void gfx_draw_v_line(const spi_display_t* dev, const _u16 l, const _u16 t,
                     const _u16 length, const _u16 color) {
  gfx_fill_rect(dev, l, t, l + 1, t + length, color);
}

void gfx_draw_line(const spi_display_t* dev, _u16 l, _u16 t, _u16 r, _u16 b,
                   const _u16 color) {
  int dx = abs(r - l);
  int dy = abs(b - t);

  _i8 stepX = l < r ? 1 : -1;
  _i8 stepY = t < b ? 1 : -1;

  int err = (dx > dy ? dx : -dy) / 2, e2;

  for (;;) {
    dev->draw_pixel(dev, l, t, color);

    if (l == r && t == b) break;
    e2 = err;

    if (e2 > -dx) {
      err -= dy;
      l += stepX;
    }
    if (e2 < dy) {
      err += dx;
      t += stepY;
    }
  }
}

// Bresenham's circle algorithm
void gfx_draw_circle(const spi_display_t* dev, const _u16 c_x, const _u16 c_y,
                     const _u16 radius, const _u16 color) {
  int f = 1 - radius;
  int ddF_x = 0;
  int ddF_y = -2 * radius;
  int x = 0;
  int y = radius;

  dev->draw_pixel(dev, c_x, c_y + radius, color);
  dev->draw_pixel(dev, c_x, c_y - radius, color);
  dev->draw_pixel(dev, c_x + radius, c_y, color);
  dev->draw_pixel(dev, c_x - radius, c_y, color);

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x + 1;
    dev->draw_pixel(dev, c_x + x, c_y + y, color);
    dev->draw_pixel(dev, c_x - x, c_y + y, color);
    dev->draw_pixel(dev, c_x + x, c_y - y, color);
    dev->draw_pixel(dev, c_x - x, c_y - y, color);
    dev->draw_pixel(dev, c_x + y, c_y + x, color);
    dev->draw_pixel(dev, c_x - y, c_y + x, color);
    dev->draw_pixel(dev, c_x + y, c_y - x, color);
    dev->draw_pixel(dev, c_x - y, c_y - x, color);
  }
}

void gfx_fill_circle(const spi_display_t* dev, const _u16 c_x, const _u16 c_y,
                     const _u16 radius, const _u16 color) {
  int f = 1 - radius;
  int ddF_x = 0;
  int ddF_y = -2 * radius;
  int x = 0;
  int y = radius;
  int16_t px = x;
  int16_t py = y;

  _u8 corners = 3;

  gfx_draw_v_line(dev, c_x, c_y - radius, 2 * radius + 1, color);

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    if (x < (y + 1)) {
      gfx_draw_v_line(dev, c_x + x, c_y - y, 2 * y, color);
      gfx_draw_v_line(dev, c_x - x, c_y - y, 2 * y, color);
    }
    if (y != py) {
      gfx_draw_v_line(dev, c_x + py, c_y - px, 2 * px, color);
      gfx_draw_v_line(dev, c_x - py, c_y - px, 2 * px, color);
      py = y;
    }
    px = x;
  }
}

// TODO: optimize it: the circle can be drawn by optimized gfx_draw_circle(),
// but "empty" part by calculate_x_y_on_circle()
void gfx_draw_unclosed_circle(const spi_display_t* dev, const _u16 c_x,
                              const _u16 c_y, const _u16 radius,
                              const _u16 start_angle, const _u16 end_angle,
                              const _u16 color) {
  array_fill_u16(tile_colors, color, TILE_COLORS_COUNT);
  static _u16 x_l, y_l = 0;

  for (_u16 angle = start_angle; angle < end_angle; angle++) {
    calculate_x_y_on_circle(c_x, c_y, angle, radius, &x_l, &y_l);
    // draw small tile, pixel isn't enough to fill it without artefacts
    dev->draw_pixels(dev, x_l, y_l, x_l + 2, y_l + 2, tile_colors, 9);
  }
}

void gfx_draw_gauge(const spi_display_t* dev, const _u16 c_x, const _u16 c_y,
                    const _u16 radius, const _u16 start_angle,
                    const _u16 end_angle, const _u16 color) {
  gfx_fill_circle(dev, c_x, c_y, radius, color);
  // make "hole" part
  gfx_fill_circle(dev, c_x, c_y, radius - GAUGE_LINE_WIDTH, COLOR_BLACK);
}

void gfx_draw_gauge_segment(const spi_display_t* dev, const _u16 c_x,
                            const _u16 c_y, const _u16 radius,
                            const _u16 start_angle, const _u16 end_angle,
                            const _u16 color) {
  for (_u16 offset = radius + 2; offset >= radius - GAUGE_LINE_WIDTH;
       offset--) {
    gfx_draw_unclosed_circle(dev, c_x, c_y, offset, start_angle, end_angle,
                             color);
  }
}

_u8 gfx_draw_char(const spi_display_t* dev, symbol_data_t* symbol,
                  const _u16 left, const _u16 top, const font_t* font,
                  const _u16 color) {
  if (font == NULL || font->scale == 0) {
    return 0;
  }

  if (symbol == NULL) {
    return font_get_width(font);
  }

  _u8 scale = font->scale;

  _u16 scaledX = 0;
  _u16 scaledY = 0;
  _u8 line = 0;

  for (_u8 y = 0; y < font->height; y++) {
    line = symbol->data[y];

    // if blank line - skip current step
    if (line == 0) continue;

    for (_u8 x = 0; x < font->width; x++) {
      if (GFX_IS_BIT_SET8(line, x)) {
        scaledX = left + x * scale;
        scaledY = top + y * scale;

        if (scale > 1) {
          // draw rectangle as scaled pixel
          gfx_fill_rect(dev, scaledX, scaledY, scaledX + scale, scaledY + scale,
                        color);
        } else {
          dev->draw_pixel(dev, scaledX, scaledY, color);
        }
      }
    }
  }
  return font_get_width(font);
}

_u16 gfx_draw_string(const spi_display_t* dev, symbol_data_t** symbols,
                     const _u16 len, const _u16 left, const _u16 top,
                     const font_t* font, const _u16 color) {
  _u16 x_pos = left;
  for (_u16 index = 0; index < len; index++) {
    if (x_pos > dev->width) {
      return x_pos;
    }
    symbol_data_t* symbol = symbols[index];
    x_pos += gfx_draw_char(dev, symbol, x_pos, top, font, color);
  }
  return x_pos;
}

// private part
void calculate_x_y_on_circle(const _u16 c_x, const _u16 c_y,
                             const _u16 angle_degree, const _u16 circle_radius,
                             _u16* x_res, _u16* y_res) {
  // clockwise direction for calculations
  _u16 angle = abs(angle_degree - 360);

  float sin_val = sin(to_rads(angle));
  float cos_val = cos(to_rads(angle));

  // from "cos(angle) = Adjacent / Hypotenuse"
  // we can get: Adjacent = cos(angle) * Hypotenuse(radius)
  _i16 adj = cos_val * circle_radius;

  // from "sin(angle) = Opposite / Hypotenuse"
  // we can get: Opposite = sin(angle) * Hypotenuse;
  _u16 opposite = sin_val * circle_radius;

  *x_res = c_x + opposite;
  *y_res = c_y + adj;
}