#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

/**
 * @brief Container for symbol bitmap
 */
typedef struct symbol_data_t {
  _u8* data;
  _u16 length;
  _u8 ascii_code;
} symbol_data_t;

#ifdef __cplusplus
}
#endif
