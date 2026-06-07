#pragma once

#include "../spi_display.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "../buff_utils.h"
#include "freertos/FreeRTOS.h"

#define CASET 0x2A     // set column(x) address
#define PASET 0x2B     // set Page(y) address
#define RAMWR 0x2C     // Memory Write
#define MADCTL 0x36    // Memory Access Control
#define PIXSET 0x3A    // Pixel Format Set
#define DINVOFF 0x20   // Display Inversion OFF
#define DINVON 0x21    // Display Inversion
#define FRMCTR1 0xB1   // Frame Rate Control
#define DISCTRL 0xB6   // Display Function Control
#define GAMSET 0x26    // Gamma Set
#define PGAMCTRL 0xE0  // Positive Gamma Correction
#define NGAMCTRL 0xE1  // Negative Gamma Correction
#define SLPIN 0x10     // Sleep In
#define SLPOUT 0x11    // Sleep Out
#define DISPOFF 0x28   // Display OFF
#define DISPON 0x29    // Display ON
#define VSCRDEF 0x33   // Vertical Scrolling Definition
#define VSCRSADD 0x37  // Vertical Scrolling Start Address
#define POWER_CTRL1 0xC0
#define POWER_CTRL2 0xC1
#define VCOM_CTRL1 0xC5
#define VCOM_CTRL2 0xC7

// Backlight Control 7(PWM_OUT output frequency control.)
#define BCKL_CTRL7 0xBE

#define BUFFER_SIZE 1024

static TickType_t _2 = pdTICKS_TO_MS(2);

DMA_ATTR static _u8 buffer[BUFFER_SIZE];

static _u16 _temp_colors_buff[1] = {0};

void display_select_region(const spi_display_t* dev, _u16 l, _u16 t, _u16 r,
                           _u16 b);
void display_draw_pixel(const spi_display_t* dev, _u16 left, _u16 top,
                        _u16 color);
void display_draw_pixels(const spi_display_t* dev, _u16 left, _u16 top,
                         _u16 right, _u16 bottom, const _u16* colors,
                         const size_t size);
bool display_rotate(spi_display_t* dev, const rotation_t rotation);
bool display_set_color_mode(spi_display_t* dev, const color_mode_t mode);
void display_set_scroll_area(spi_display_t* dev, _u16 tfa, _u16 vsa, _u16 bfa);
void display_reset_scroll_area(spi_display_t* dev, _u16 vsa);
void display_scroll(spi_display_t* dev, _u16 vsp);
void display_set_on_off(spi_display_t* dev, const bool on);
bool display_sleep(spi_display_t* dev);
bool display_wakeup(spi_display_t* dev);
bool display_set_inversion(spi_display_t* dev, const bool inversion);

void display_init(spi_display_t* dev) {
  dev->transmit_command(dev->spi_handle, dev->dc, POWER_CTRL1);
  /**
  0x26 - 4.75V
  0x1F - 4.4V
  0x17 - 4.0V
  0x11 - 3.7V
  0x09 - 3.3V
  */
  _u8 ctrl1Data[1] = {0x26};
  dev->transmit_data(dev->spi_handle, dev->dc, ctrl1Data, sizeof(ctrl1Data));

  dev->transmit_command(dev->spi_handle, dev->dc, POWER_CTRL2);
  //  lowest value and default
  // AVDD=VCIx2, VGH=VCIx7, VGL=-VCIx3
  _u8 ctrl2Data[1] = {0x01};
  dev->transmit_data(dev->spi_handle, dev->dc, ctrl2Data, sizeof(ctrl2Data));

  dev->transmit_command(dev->spi_handle, dev->dc, VCOM_CTRL1);
  /**
  VMH: 0x1E = 3.45V
  0x35 = 5.225V
  0x3E = 5.850V
  VML: 0x28 = -1.500V
  */
  _u8 vcomCtrl1[2] = {0x35, 0x28};
  dev->transmit_data(dev->spi_handle, dev->dc, vcomCtrl1, sizeof(vcomCtrl1));

  dev->transmit_command(dev->spi_handle, dev->dc, VCOM_CTRL2);
  // VCOMH/VCOML voltage adjustment
  // VMH – 2 VML – 2
  _u8 vcomCtrl2[1] = {0xbe};
  dev->transmit_data(dev->spi_handle, dev->dc, vcomCtrl2, sizeof(vcomCtrl2));

  dev->transmit_command(dev->spi_handle, dev->dc, BCKL_CTRL7);
  // 0xff - 245Hz
  // 0x04 - 12.549 KHz
  _u8 br7[1] = {0x04};
  dev->transmit_data(dev->spi_handle, dev->dc, br7, sizeof(br7));

  // just give some time to apply some changes, when mcu has high
  // clock speed (240 MHz, for example) this part works incorrectly
  // without delay
  vTaskDelay(pdMS_TO_TICKS(10));

  display_rotate(dev, ANGLE_270);

  dev->transmit_command(dev->spi_handle, dev->dc, PIXSET);
  // 65K color: 16-bit/pixel
  _u8 pixSet[1] = {0x55};
  dev->transmit_data(dev->spi_handle, dev->dc, pixSet, sizeof(pixSet));

  // display_set_inversion(dev, false);

  /**
  Frame Rate:
  0x1B = 70 Hz
  0x19 = 76 Hz
  0x15 = 90 Hz
  0x13 = 100 Hz
  */
  dev->transmit_command(dev->spi_handle, dev->dc, FRMCTR1);
  _u8 frmCtrl1[2] = {0x00, 0x1B};
  dev->transmit_data(dev->spi_handle, dev->dc, frmCtrl1, sizeof(frmCtrl1));

  /**
   REV:1 - liqud crystal normally white
   GS:0 SS:1 SM:0
   SS:1, means S720 -> S1 (because of rotated display)
   ISC = 0x01 - scan cycle 3 frames (51 ms)
   PT = 0x02: Determine source/VCOM output in a non-display area in the partial
      display mode.
   PTG = 0x02: interval scan
   */
  _u8 disCtrl[4] = {0x0A, 0xA1, 0x27, 0x00};
  dev->transmit_command(dev->spi_handle, dev->dc, DISCTRL);
  dev->transmit_data(dev->spi_handle, dev->dc, disCtrl, sizeof(disCtrl));

  dev->transmit_command(dev->spi_handle, dev->dc, PGAMCTRL);
  _u8 pgc[15] = {
      0x1F, 0x1A, 0x18, 0x0A, 0x0F, 0x06, 0x45, 0x87,
      0x32, 0x0A, 0x07, 0x02, 0x07, 0x05, 0x00,
  };
  dev->transmit_data(dev->spi_handle, dev->dc, pgc, sizeof(pgc));

  dev->transmit_command(dev->spi_handle, dev->dc, NGAMCTRL);
  _u8 ngc[15] = {
      0x00, 0x25, 0x27, 0x05, 0x10, 0x09, 0x3A, 0x78,
      0x4D, 0x05, 0x18, 0x0D, 0x38, 0x3A, 0x1F,
  };
  dev->transmit_data(dev->spi_handle, dev->dc, ngc, sizeof(ngc));

  dev->transmit_command(dev->spi_handle, dev->dc, SLPOUT);
  /**
   * During the Resetting period, the display will be blanked
   * (The display is entering blanking sequence, which maximum time
   * is 120 ms, when Reset Starts in Sleep Out –mode.
   *
   * Check datasheet, page 218, "15.4. Reset Timing"
   */
  vTaskDelay(pdMS_TO_TICKS(121));
  dev->transmit_command(dev->spi_handle, dev->dc, DISPON);

  dev->select_region = display_select_region;
  dev->draw_pixel = display_draw_pixel;
  dev->draw_pixels = display_draw_pixels;
  dev->set_color_mode = display_set_color_mode;

  display_set_color_mode(dev, MODE_BGR);
}

void display_select_region(const spi_display_t* dev, _u16 l, _u16 t, _u16 r,
                           _u16 b) {
  dev->transmit_command(dev->spi_handle, dev->dc, CASET);
  buffer_set_2u16(buffer, l, r);
  dev->transmit_data(dev->spi_handle, dev->dc, buffer, 4);

  dev->transmit_command(dev->spi_handle, dev->dc, PASET);
  buffer_set_2u16(buffer, t, b);
  dev->transmit_data(dev->spi_handle, dev->dc, buffer, 4);
}

void display_draw_pixel(const spi_display_t* dev, _u16 left, _u16 top,
                        _u16 color) {
  _temp_colors_buff[0] = color;
  display_draw_pixels(dev, left, top, left, top, _temp_colors_buff, 1);
}

// TODO: Add 8 bit version
void display_draw_pixels(const spi_display_t* dev, _u16 left, _u16 top,
                         _u16 right, _u16 bottom, const _u16* colors,
                         const size_t size) {
  if (size >= BUFFER_SIZE / 2) {
    return;
  }

  if (left >= dev->width) return;
  if (top >= dev->height) return;

  if (right >= dev->width) {
    right = dev->width - 1;
  }

  if (bottom >= dev->height) {
    bottom = dev->height - 1;
  }

  display_select_region(dev, left, top, right, bottom);
  dev->transmit_command(dev->spi_handle, dev->dc, RAMWR);

  buffer_set_u16_array(buffer, colors, size);
  dev->transmit_data(dev->spi_handle, dev->dc, buffer, size * 2);
}

void display_set_on_off(spi_display_t* dev, const bool on) {
  dev->transmit_command(dev->spi_handle, dev->dc, on ? DISPON : DISPOFF);
}

bool display_sleep(spi_display_t* dev) {
  bool result = dev->transmit_command(dev->spi_handle, dev->dc, SLPIN);
  vTaskDelay(pdMS_TO_TICKS(130));
  return result;
}

bool display_wakeup(spi_display_t* dev) {
  bool result = dev->transmit_command(dev->spi_handle, dev->dc, SLPOUT);
  vTaskDelay(pdMS_TO_TICKS(130));
  return result;
}

bool display_rotate(spi_display_t* dev, const rotation_t rotation) {
  if (dev->rotation == rotation) {
    return true;
  }

  _u8 mx, my, mv = 0x00;

  bool isOldModePortrait =
      (dev->rotation == ANGLE_0) || (dev->rotation == ANGLE_180);
  bool isNewModePortait = (rotation == ANGLE_0) || (rotation == ANGLE_180);

  dev->transmit_command(dev->spi_handle, dev->dc, MADCTL);

  switch (rotation) {
    case ANGLE_90: {
      mx = 0x40;
      mv = 0x20;
      my = 0x00;
      break;
    }

    case ANGLE_180: {
      mx = 0x40;
      my = 0x80;
      mv = 0x00;
      break;
    }

    case ANGLE_270: {
      my = 0x80;
      mv = 0x20;
      mx = 0x00;
      break;
    }

    default:
      my = 0x00;
      mv = 0x00;
      mx = 0x00;
      break;
  }

  // update coords if orientaion hs been changed
  if (isOldModePortrait != isNewModePortait) {
    _u16 temp = dev->width;
    dev->width = dev->height;
    dev->height = temp;
  }
  dev->rotation = rotation;

  _u8 rotData[1] = {mx | my | mv | (dev->color_mode)};
  bool result =
      dev->transmit_data(dev->spi_handle, dev->dc, rotData, sizeof(rotData));

  return result;
}

bool display_set_inversion(spi_display_t* dev, const bool inversion) {
  return dev->transmit_command(dev->spi_handle, dev->dc,
                               inversion ? DINVON : DINVOFF);
}

bool display_set_color_mode(spi_display_t* dev, const color_mode_t mode) {
  dev->color_mode = mode;
  dev->transmit_command(dev->spi_handle, dev->dc, MADCTL);
  _u8 modeData[1] = {mode};
  return dev->transmit_data(dev->spi_handle, dev->dc, modeData, 1);
}

/**
 * @brief Set area as scrollable
 *
 * @param tfa Top Fixed Area (in No. of lines from Top of
 * the Frame Memory and Display)
 * @param vsa Vertical Scrolling Area  (in No. of lines of the Frame
 * Memory [not the display] from the Vertical Scrolling Start Address).
 * The first line read from Frame Memory appears immediately after
 * the bottom most line of the Top Fixed Area.
 * @param bfa Bottom Fixed Area (in No. of lines from Bottom of the Frame
 * Memory and Display). TFA, VSA and BFA refer to the Frame Memory Line Pointer.
 */
void display_set_scroll_area(spi_display_t* dev, _u16 tfa, _u16 vsa, _u16 bfa) {
  dev->transmit_command(dev->spi_handle, dev->dc, VSCRDEF);

  buffer_set_u16(buffer, tfa);
  dev->transmit_data(dev->spi_handle, dev->dc, buffer, 2);

  buffer_set_u16(buffer, vsa);
  dev->transmit_data(dev->spi_handle, dev->dc, buffer, 2);

  buffer_set_u16(buffer, bfa);
  dev->transmit_data(dev->spi_handle, dev->dc, buffer, 2);
}

void display_reset_scroll_area(spi_display_t* dev, _u16 vsa) {
  dev->transmit_command(dev->spi_handle, dev->dc, VSCRDEF);

  buffer_set_u16(buffer, 0);
  dev->transmit_data(dev->spi_handle, dev->dc, buffer, 2);

  buffer_set_u16(buffer, vsa);
  dev->transmit_data(dev->spi_handle, dev->dc, buffer, 2);

  buffer_set_u16(buffer, 0);
  dev->transmit_data(dev->spi_handle, dev->dc, buffer, 2);
}

/**
 * @brief Vertical Scrolling Start Address
 *
 * @param vsp Vertical Scrolling Start Address, the line in the
 * Frame Memory that will be written as the first line after
 * the last line of the Top Fixed Area on the display
 */
void display_scroll(spi_display_t* dev, _u16 vsp) {
  dev->transmit_command(dev->spi_handle, dev->dc, VSCRSADD);

  buffer_set_u16(buffer, vsp);
  dev->transmit_data(dev->spi_handle, dev->dc, buffer, 2);
}

#ifdef __cplusplus
}
#endif