#pragma once

#include <driver/spi_master.h>
#include <types.h>

#include "rotation.h"

// command
#define DC_C 0
// data
#define DC_D 1

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  MODE_RGB = 0x00,
  MODE_BGR = 0x08,
} color_mode_t;

typedef enum {
  SCROLL_RIGHT = 1,
  SCROLL_LEFT,
  SCROLL_DOWN,
  SCROLL_UP,
} scroll_type_t;

typedef struct spi_display_t spi_display_t;

/**
 * @brief interface describing API of low level spi based display device. Its
 * used in driver specific implementation as abstraction of mcu specific
 * implementation of spi.
 */
typedef struct spi_display_t {
  _u16 width;
  _u16 height;

  _i8 offset_x;
  _i8 offset_y;

  /**
   * @brief scren rotation, counting from default (0)
   */
  rotation_t rotation;

  /**
   * @brief scren rotation, counting from default (0)
   */
  rotation_t font_rotaion;

  /**
   * @brief can RGB or BGR.
   * Check datasheet for details: "10.1.29 Memory Access Control (36h)" page 142
   */
  color_mode_t color_mode;

  /**
   * @brief D/C gpio pin
   */
  _i8 dc;

  /**
   * @brief Reset gpio pin
   */
  _i8 res;

  /**
   * @brief Backlight gpio pin
   */
  _i8 bl;

  spi_device_handle_t spi_handle;

  /** @brief Basic spi command transmit */
  bool (*transmit_command)(const spi_device_handle_t spi_handle,
                           const _i8 dc_gpio, const _u8 command);

  /** @brief Basic spi data transmit */
  bool (*transmit_data)(const spi_device_handle_t spi_handle, const _i8 dc_gpio,
                        const _u8* data, const size_t length);

  bool (*lighten)(const _i8 bl, const _u8 percents);

  bool (*set_color_mode)(spi_display_t* self, const color_mode_t mode);

  // drawing api
  void (*select_region)(const spi_display_t* dev, _u16 l, _u16 t, _u16 r,
                        _u16 b);

  void (*draw_pixel)(const spi_display_t* self, const _u16 x, const _u16 y,
                     const _u16 color);
  void (*draw_pixels)(const spi_display_t* self, _u16 l, _u16 t, _u16 r, _u16 b,
                      const _u16* pixels, const size_t len);
} spi_display_t;

#ifdef __cplusplus
}
#endif