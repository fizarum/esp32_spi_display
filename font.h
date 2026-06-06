#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

/**
 * @brief General font description, should be used
 * with SymbolData_t to define font size
 */
typedef struct font_t {
  _u8 width;
  _u8 height;
  _u8 scale;
  _u8 spacing;
} font_t;

static _u8 font_get_width(const font_t* font) {
  //  height is taken because for font smaller than 8x8
  //  height will contain actual size, when width is still 8 pt
  //  for example, for 5x5 font its 8x5 (width x height).
  //  Its known issue which will be fixed later
  return font->height * font->scale + font->spacing;
}

static _u8 font_get_height(const font_t* font) {
  // we have to apply spacing * 2 for correct top and bottom spacing
  return font->height * font->scale + font->spacing * 2;
}

#ifdef __cplusplus
}
#endif