#pragma once

#include <driver/spi_master.h>
#include <stdbool.h>
#include <stddef.h>

// command
#define DC_C 0
// data
#define DC_D 1

typedef uint8_t _u8;
typedef int8_t _i8;
typedef uint16_t _u16;
typedef int16_t _i16;
typedef uint32_t _u32;

typedef enum {
  MODE_RGB = 0x00,
  MODE_BGR = 0x08,
} color_mode_t;

typedef enum {
  ANGLE_0,
  ANGLE_90,
  ANGLE_180,
  ANGLE_270,
} rotation_t;

typedef enum {
  SCROLL_RIGHT = 1,
  SCROLL_LEFT,
  SCROLL_DOWN,
  SCROLL_UP,
} scroll_type_t;

typedef struct {
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
} spi_display_t;