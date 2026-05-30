#include "display.h"

#include <gpio_hal.h>
#include <pwm_hal.h>
#include <spi_hal.h>
#include <string.h>

#include "st7789/st7789.h"

#define SPI_BYTE_BUFF_MAX_SIZE 1
#define HOST_ID SPI2_HOST

// stores currently initialized spi, should be > 0 if display_init_spi()
// completed successfully
spi_host_device_t spi_host_device = -1;

static bool transmit_command(const spi_device_handle_t spi_handle,
                             const _i8 dc_gpio, const _u8 command);
static bool transmit_data(const spi_device_handle_t spi_handle,
                          const _i8 dc_gpio, const _u8* data,
                          const size_t length);
static bool transmit(const spi_device_handle_t spi_handle, const _u8* data,
                     const size_t length);
static bool lighten(const _i8 bl, const _u8 percents);
static void configure_device(spi_display_t* display,
                             spi_device_handle_t spi_handle);

DMA_ATTR static _u8 buffer[SPI_BYTE_BUFF_MAX_SIZE];
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

bool display_create(spi_display_t* dev, const _i8 cs, const _i8 dc,
                    const _i8 res_gpio, const _i8 bl) {
  if (dev == NULL) {
    return false;
  }

  if (spi_host_device == -1) {
    return false;
  }

  gpio_configure_and_set(bl, GPIO_MODE_OUTPUT, 0);

  spi_device_handle_t handle;

  configure_device(dev, handle);

  bool added = spi_add_device(&handle, HOST_ID, cs, SPI_MASTER_FREQ_40M);
  if (added) {
    dev->dc = dc;
    dev->bl = bl;
    dev->res = res_gpio;
    dev->spi_handle = handle;
    display_init(dev);
    return true;
  }
  return false;
}

// private part
void configure_device(spi_display_t* display, spi_device_handle_t spi_handle) {
  display->width = DISPLAY_WIDTH;
  display->height = DISPLAY_HEIGHT;
  display->offset_x = DISPLAY_OFFSET_X;
  display->offset_y = DISPLAY_OFFSET_Y;
  display->rotation = ANGLE_0;
  display->font_rotaion = ANGLE_0;
  display->color_mode = MODE_BGR;
  display->bl = -1;
  display->spi_handle = spi_handle;
  display->transmit_command = &transmit_command;
  display->transmit_data = &transmit_data;
  display->lighten = &lighten;
}

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