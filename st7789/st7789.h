#pragma once

#include "../spi_display.h"

#ifdef __cplusplus
extern "C" {
#endif

void display_init(spi_display_t* dev);

void display_set_on_off(spi_display_t* dev, const bool on);
void display_sleep(spi_display_t* dev);
void display_wakeup(spi_display_t* dev);
bool display_set_inversion(spi_display_t* dev, const bool inversion);

#ifdef __cplusplus
}
#endif