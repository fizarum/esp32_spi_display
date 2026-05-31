#include "st7789.h"

#include <string.h>

#include "../buff_utils.h"
#include "freertos/FreeRTOS.h"

#define SWRESET 0x01  // Software Reset
#define SLPOUT 0x11   // Sleep Out
#define COLMOD 0x3A   // Interface Pixel Format
#define MADCTL 0x36   // Memory Data Access Control
#define CASET 0x2A    // Column Address Set
#define RASET 0x2B    // Row Address Set
#define INVON 0x21    // Display Inversion On
#define INVOFF 0x20   // Display Inversion OFF
#define NORON 0x13    // Normal Display Mode On
#define DISPON 0x29   // Display On
#define DISPOFF 0x28  // Display Off
#define RAMWR 0x2C    // Memory Write
#define SLPIN 0x10    // Sleep In
#define SLPOUT 0x11   // Sleep Out

#define BUFFER_SIZE 1024

static TickType_t _10 = pdMS_TO_TICKS(10);
static TickType_t _150 = pdMS_TO_TICKS(150);
static TickType_t _255 = pdMS_TO_TICKS(255);

DMA_ATTR static _u8 buffer[BUFFER_SIZE];

static _u16 _x1;
static _u16 _x2;
static _u16 _y1;
static _u16 _y2;

static void display_select_region(spi_display_t* dev, _u16 l, _u16 t, _u16 r,
                                  _u16 b);

void display_init(spi_display_t* dev) {
  dev->transmit_command(dev->spi_handle, dev->dc, SWRESET);
  vTaskDelay(_150);

  dev->transmit_command(dev->spi_handle, dev->dc, SLPOUT);
  vTaskDelay(_255);

  // Interface Pixel Format: 16 bit/pixel
  dev->transmit_command(dev->spi_handle, dev->dc, COLMOD);
  _u8 colmodData[] = {0x55};
  dev->transmit_data(dev->spi_handle, dev->dc, colmodData, 1);
  vTaskDelay(_10);

  dev->transmit_command(dev->spi_handle, dev->dc, MADCTL);
  _u8 madCtlData[] = {0x00};
  dev->transmit_data(dev->spi_handle, dev->dc, madCtlData, 1);

  _u8 addrSetData[] = {0x00, 0x00, 0x00, 0xF0};
  dev->transmit_command(dev->spi_handle, dev->dc, CASET);
  dev->transmit_data(dev->spi_handle, dev->dc, addrSetData, 4);

  dev->transmit_command(dev->spi_handle, dev->dc, RASET);
  dev->transmit_data(dev->spi_handle, dev->dc, addrSetData, 4);

  dev->transmit_command(dev->spi_handle, dev->dc, INVON);
  vTaskDelay(_10);

  dev->transmit_command(dev->spi_handle, dev->dc, NORON);
  vTaskDelay(_10);

  dev->transmit_command(dev->spi_handle, dev->dc, DISPON);
  vTaskDelay(_255);

  dev->lighten(dev->bl, 100);
}

void display_draw_pixel(spi_display_t* dev, _u16 x, _u16 y, _u16 color) {
  _x1 = x + dev->offset_x;
  _x2 = x + dev->offset_x;
  _y1 = y + dev->offset_y;
  _y2 = y + dev->offset_y;

  display_select_region(dev, _x1, _y1, _x2, _y2);

  buffer_set_u16(buffer, color);
  dev->transmit_data(dev->spi_handle, dev->dc, buffer, 2);
}

void display_draw_pixels(spi_display_t* dev, _u16 l, _u16 t, _u16 r, _u16 b,
                         _u16* colors, size_t size) {
  if (size >= BUFFER_SIZE / 2) {
    return;
  }

  if (l >= dev->width) return;
  if (t >= dev->height) return;

  if (r >= dev->width) {
    r = dev->width - 1;
  }

  if (b >= dev->height) {
    b = dev->height - 1;
  }

  _x1 = l + dev->offset_x;
  _x2 = r + dev->offset_x;
  _y1 = t + dev->offset_y;
  _y2 = b + dev->offset_y;

  display_select_region(dev, _x1, _y1, _x2 - 1, _y2 - 1);

  _u16 len = _y2 - _y1;
  buffer_set_u16_array(buffer, colors, size);
  for (_u16 x = _x1; x < _x2; x++) {
    dev->transmit_data(dev->spi_handle, dev->dc, buffer, len * 2);
  }
}

void display_fill_rect(spi_display_t* dev, _u16 l, _u16 t, _u16 r, _u16 b,
                       _u16 color) {
  if (l >= dev->width) return;
  if (r > dev->width) r = dev->width;
  if (t >= dev->height) return;
  if (b > dev->height) b = dev->height;

  _x1 = l + dev->offset_x;
  _x2 = r + dev->offset_x;
  _y1 = t + dev->offset_y;
  _y2 = b + dev->offset_y;

  display_select_region(dev, _x1, _y1, _x2 - 1, _y2 - 1);

  _u16 lines_count = _y2 - _y1;
  _u16 line_len = _x2 - _x1;

  // first fill one line and put it into buffer
  buffer_fill_u16(buffer, color, line_len);

  // now fill lines one by one to display memory
  for (_u16 line_index = 0; line_index < lines_count; line_index++) {
    dev->transmit_data(dev->spi_handle, dev->dc, buffer, line_len * 2);
  }
}

void display_clear(spi_display_t* dev, _u16 color) {
  display_fill_rect(dev, 0, 0, dev->width, dev->height, color);
}

void display_set_on_off(spi_display_t* dev, const bool on) {
  dev->transmit_command(dev->spi_handle, dev->dc, on ? DISPON : DISPOFF);
}

void display_sleep(spi_display_t* dev) {
  dev->transmit_command(dev->spi_handle, dev->dc, SLPIN);
  // we have to wait for at least 120 msec (datasheet, page 119, "SLPIN"
  // section)
  vTaskDelay(pdMS_TO_TICKS(130));
}

void display_wakeup(spi_display_t* dev) {
  dev->transmit_command(dev->spi_handle, dev->dc, SLPOUT);
  vTaskDelay(pdMS_TO_TICKS(130));
}

// Display inversion on/off
bool display_set_inversion(spi_display_t* dev, const bool inversion) {
  return dev->transmit_command(dev->spi_handle, dev->dc,
                               inversion ? INVON : INVOFF);
}

/// @brief select region including points provided in€ arguments
/// so if l = 20 and r = 30, then length (r - l) is 11 (because 30th pixel also
/// included)
void display_select_region(spi_display_t* dev, _u16 l, _u16 t, _u16 r, _u16 b) {
  dev->transmit_command(dev->spi_handle, dev->dc, CASET);
  buffer_set_2u16(buffer, l, r);

  dev->transmit_data(dev->spi_handle, dev->dc, buffer, 4);

  dev->transmit_command(dev->spi_handle, dev->dc, RASET);
  buffer_set_2u16(buffer, t, b);
  dev->transmit_data(dev->spi_handle, dev->dc, buffer, 4);

  dev->transmit_command(dev->spi_handle, dev->dc, RAMWR);
}