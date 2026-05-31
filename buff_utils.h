#pragma once
#include <stddef.h>
#include <stdint.h>

typedef uint8_t _u8;
typedef int8_t _i8;
typedef uint16_t _u16;
typedef int16_t _i16;
typedef uint32_t _u32;

/**
 * set single _u16 value to _u8 buffer
 */
static void buffer_set_u16(_u8* buff, const _u16 value) {
  buff[0] = (value >> 8) & 0xff;
  buff[1] = value & 0xff;
}

static void buffer_set_2u16(_u8* buff, const _u16 first, const _u16 second) {
  buff[0] = (first >> 8) & 0xff;
  buff[1] = first & 0xff;
  buff[2] = (second >> 8) & 0xff;
  buff[3] = second & 0xff;
}

/**
 * set array of _u16 into _u8 buffer,
 * make sure that buffer length x2 times bigger than array's size
 *
 * @param buff where to store results
 * @param array source _u16 array
 * @param size length of array
 */
static void buffer_set_u16_array(_u8* buff, const _u16* array,
                                 const size_t size) {
  size_t buff_index = 0;

  for (size_t index = 0; index < size; index++) {
    const _u16 array_item = array[index];

    buff[buff_index++] = (array_item >> 8) & 0xff;
    buff[buff_index++] = array_item & 0xff;
  }
}

/**
 * fill buffer with value repeat_times times
 * @param buff where to store results
 * @param value value to fill by
 * @param repeat_times how many times apply value
 */
static void buffer_fill_u16(_u8* buff, const _u16 value,
                            const size_t repeat_times) {
  size_t buff_index = 0;
  for (size_t index = 0; index < repeat_times; index++) {
    buff[buff_index++] = (value >> 8) & 0xff;
    buff[buff_index++] = value & 0xff;
  }
}
