#include "display.h"

#include <gpio_hal.h>
#include <pwm_hal.h>
#include <spi_hal.h>
#include <string.h>

#define HOST_ID SPI2_HOST

static bool transmit_command(const spi_device_handle_t spi_handle,
                             const _i8 dc_gpio, const _u8 command);
static bool transmit_data(const spi_device_handle_t spi_handle,
                          const _i8 dc_gpio, const _u8* data,
                          const size_t length);
static bool transmit(const spi_device_handle_t spi_handle, const _u8* data,
                     const size_t length);
static bool lighten(const _i8 bl, const _u8 percents);

DMA_ATTR static _u8 buffer[1];

// stores currently initialized spi, should be > 0 if display_init_spi()
// completed successfully
static spi_host_device_t spi_host_device = -1;
static spi_transaction_t transaction;
TickType_t _100 = pdMS_TO_TICKS(100);

// public API

bool display_init_spi(_i8 mosi, _i8 clk, _i8 dc, _i8 res) {
  gpio_configure_and_set(dc, GPIO_MODE_OUTPUT, 0);

  if (gpio_configure_and_set(res, GPIO_MODE_OUTPUT, 1)) {
    vTaskDelay(_100);
    gpio_set(res, 0);
    vTaskDelay(_100);
    gpio_set(res, 1);
    vTaskDelay(_100);
  }

  if (spi_init(HOST_ID, mosi, clk)) {
    spi_host_device = HOST_ID;
    return true;
  }

  spi_host_device = -1;
  return false;
}

bool display_add(spi_display_t* dev, const _i8 cs, const _i8 dc,
                 const _i8 res_gpio, const _i8 bl) {
  if (dev == NULL) {
    return false;
  }

  if (spi_host_device == -1) {
    return false;
  }

  gpio_configure_and_set(bl, GPIO_MODE_OUTPUT, 0);

  spi_device_handle_t handle;

  bool is_ok = spi_add_device(&handle, HOST_ID, cs, SPI_MASTER_FREQ_40M);
  assert(is_ok == true);

  dev->dc = dc;
  dev->bl = bl;
  dev->res = res_gpio;
  dev->spi_handle = handle;

  dev->transmit_command = &transmit_command;
  dev->transmit_data = &transmit_data;
  dev->lighten = &lighten;

  is_ok = display_configure(dev, DISPLAY_WIDTH, DISPLAY_HEIGHT,
                            DISPLAY_OFFSET_X, DISPLAY_OFFSET_Y, ANGLE_0);
  assert(is_ok == true);
  display_set_color_mode(dev, MODE_BGR);
  assert(is_ok == true);

  return true;
}

bool display_configure(spi_display_t* dev, const _u16 width, const _u16 height,
                       const _i8 offset_x, const _i8 offset_y,
                       const rotation_t rotation) {
  dev->width = width;
  dev->height = height;
  dev->offset_x = offset_x;
  dev->offset_y = offset_y;
  dev->rotation = rotation;
  dev->font_rotaion = rotation;
}

bool display_set_color_mode(spi_display_t* dev, const color_mode_t mode) {
  dev->color_mode = mode;
}

// private part
static bool transmit_command(const spi_device_handle_t spi_handle,
                             const _i8 dc_gpio, const _u8 command) {
  gpio_set_level(dc_gpio, DC_C);

  buffer[0] = command;

  return transmit(spi_handle, buffer, 1);
}

static bool transmit_data(const spi_device_handle_t spi_handle,
                          const _i8 dc_gpio, const _u8* data,
                          const size_t length) {
  gpio_set_level(dc_gpio, DC_D);

  return transmit(spi_handle, data, length);
}

static IRAM_ATTR bool transmit(const spi_device_handle_t spi_handle,
                               const _u8* data, const size_t length) {
  memset(&transaction, 0, sizeof(transaction));

  transaction.length = length * 8;
  transaction.tx_buffer = data;
  esp_err_t result = spi_device_transmit(spi_handle, &transaction);

  assert(result == ESP_OK);

  return true;
}

static bool lighten(const _i8 bl, const _u8 percents) {
  return pwm_set_percents(bl, percents);
}