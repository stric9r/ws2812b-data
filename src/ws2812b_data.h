/// ws2812b_data
///
/// This module tracks user changes and updates a stream buffer used by SPI
/// It is assumed that SPI is running at 2.5Mbps.

#ifndef WS2812B_DATA_H_
#define WS2812B_DATA_H_

#include "stdint.h"
#include "stddef.h"
#include "stdbool.h"

/// Used to help with storage buffer creation, 3 bytes per LED
#define WS2812B_BYTES_PER_LED 3u
/// Used to help with stream buffer creation 24 bits per LED
#define WS2812B_BITS_PER_LED (WS2812B_BYTES_PER_LED * 8u)
/// Bits per LED at 2.5 Mhz
#define WS2812_BITS_PER_LED_2P5MHZ (WS2812B_BITS_PER_LED * 3u)
/// Bits per LED at 5Mhz
#define WS2812_BITS_PER_LED_5MHZ (WS2812B_BITS_PER_LED * 6u)
/// Bytes per LED for stream buffer running at 2.5Mhz
#define WS2812_BYTES_PER_LED_2P5MHZ (WS2812_BITS_PER_LED_2P5MHZ / 8u)
/// Bytes per LED for stream buffer running at 5MHz
#define WS2812_BYTES_PER_LED_5MHZ (WS2812_BITS_PER_LED_5MHZ / 8u)

typedef enum
{
  WS2812B_INIT_FAILED,  ///< WS2812B_INIT_FAILED
  WS2812B_INIT_2p5MHz,  ///< WS2812B_INIT_2p5MHz
  WS2812B_INIT_5MHz,    ///< WS2812B_INIT_5MHz
}ws2812b_init_state_t;

/// This struct holds instances of ws2812b strips
typedef struct
{
    uint8_t *            p_buffer;      ///< The buffer storage area for the LED strip data
    size_t               buffer_sz;     ///< The size led store p_buffer
    uint8_t *            p_stream;      ///< The buffer that streams data over SPI
    size_t               stream_sz;     ///< The size of the stream buffer
    size_t               led_count;     ///< The number of LED's on the strip
    ws2812b_init_state_t init_state;    ///< Tracks if an instance is properly initialized
}
ws2812b_t;


bool ws2812b_data_init(ws2812b_t * const p_instance,
                       ws2812b_init_state_t const desired_spi_clk);
bool ws2812b_data_set_x(ws2812b_t * const p_instance,
                        size_t const led_num_start,
                        size_t const led_num_to_set,
                        uint8_t const red,
                        uint8_t const green,
                        uint8_t const blue);
bool ws2812b_data_set(ws2812b_t * const p_instance,
                      size_t const led_num,
                      uint8_t const red,
                      uint8_t const green,
                      uint8_t const blue);
bool ws2812b_data_set_all(ws2812b_t * const p_instance,
                          uint8_t const red,
                          uint8_t const green,
                          uint8_t const blue);
bool ws2812b_data_clear_all(ws2812b_t * const p_instance);
void ws2812b_update_stream_2p5mhz(ws2812b_t * const p_instance);
void ws2812b_update_stream_5mhz(ws2812b_t * const p_instance);

#endif /* WS2812B_DATA_H_ */
