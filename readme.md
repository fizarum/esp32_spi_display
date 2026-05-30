# st7789
Simple implementation of st7789 driver for esp32

## dependencies
This library uses __mini_hal_esp32__(https://github.com/fizarum/mini_hal_esp32) so make sure you have it installed locally. if you're using PlatformIO you can add this dependency pretty easy:
```
lib_deps = 
    ;hal
    https://github.com/fizarum/mini_hal_esp32.git
```

## usage
To communicate with st7789 device you have to:
1. init & configure spi bus
2. create & configure *spi_display_t* device

Then you can use rest of API.


### 1. init & configure spi bus
To initialize spi bus just call __display_init_spi()__ with `mosi`, `sclk` and `reset` gpio parameters. 

### 2. create & configure *spi_display_t* device
Now we need to add a display as spi device. __display_create()__ adds new display and configure it for you. You can add multiple displays on same bus, but make sure you have provided correct `cs` gpio for each one.

That's it!

### example of usage 3 displays
```c
#include <display.h>
#include <palette.h>
#include <st7789/st7789.h>

// display devices
spi_display_t dev1;
spi_display_t dev2;
spi_display_t dev3;

// test function declarations
void test1_display(spi_display_t* display);
void test2_display(spi_display_t* display);
void test3_display(spi_display_t* display);

void app_main() {
  bool initialized = display_init_spi(MOSI_GPIO, SCLK_GPIO, DC_GPIO, RESET_GPIO);
  if (!initialized) {
    ESP_LOGE("MAIN", "failed to initialize spi");
    return;
  }

  bool added = display_create(&dev1, CS1, DC_GPIO, RESET_GPIO, -1);
  assert(added == true);

  added = display_create(&dev2, CS2, DC_GPIO, RESET_GPIO, -1);
  assert(added == true);

  added = display_create(&dev3, CS3, DC_GPIO, RESET_GPIO, -1);
  assert(added == true);

  test1_display(&dev1);
  test2_display(&dev2);
  test3_display(&dev3);
}

// test function implementations 
void test1_display(spi_display_t* display) {
  display_clear(display, COLOR_BLACK);
  display_fill_rect(display, 50, 50, 60, 60, COLOR_RED);
  display_draw_pixel(display, 40, 40, COLOR_WHITE);
}

void test2_display(spi_display_t* display) {
  display_clear(display, COLOR_BLACK);
  display_fill_rect(display, 50, 50, 60, 55, COLOR_GREEN);

  static _u16 colors[] = {
      COLOR_GREEN, COLOR_RED, COLOR_GREEN, COLOR_RED, COLOR_GREEN,
      COLOR_GREEN, COLOR_RED, COLOR_GREEN, COLOR_RED, COLOR_GREEN,
      COLOR_GREEN, COLOR_RED, COLOR_GREEN, COLOR_RED, COLOR_GREEN,
      COLOR_GREEN, COLOR_RED, COLOR_GREEN, COLOR_RED, COLOR_GREEN,
      COLOR_GREEN, COLOR_RED, COLOR_GREEN, COLOR_RED, COLOR_GREEN,
  };

  display_draw_pixels(display, 40, 40, 45, 45, colors, 25);
}

void test3_display(spi_display_t* display) {
  display_clear(display, COLOR_BLACK);
  display_fill_rect(display, 50, 50, 60, 60, COLOR_BLUE);
}
```