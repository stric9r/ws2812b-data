# ws2812b-data
This module generates a stream of data for a WSS2812B LED strip to stream over SPI.
It only works with 2.5Mhz or 5Mhz SPI.  (Note that 5Mhz is not tested yet).
If you need it to work with a different clock speed then you'll need to create a 
``` ws2812b_update_stream_?Mhz(ws2812b_t * p_instance) ``` function.

This module works by allowing the app to update the storage buffer with the colors per LED in the strip.
Once ready, the app calls the ```ws2812b_update_stream...``` function and then the passes the 
```ws2812b_t::p_stream``` and ```ws2812b_t::stream_size``` to the platform's SPI write function.
After the write completes, there should be a reset delay added by the app.  
The stream does not contain the reset delay.  It is up the the app to implement this.

Example of it working [here](https://www.youtube.com/watch?v=ARf2NLlesRc)

## Usage
Using this module is pretty simple.  Below is psuedo-ish code to help understand, it is a very simple example to convey how to use this module.

```
#include "wsb2812b_data.h"

/// How many WS2812B LED's on the strip to control
#define LED_STRIP_LED_COUNT 16u

/// Calculate size of storage buffer used to update the LEDs
#define LED_STRIP_BUFF_SZ (LED_STRIP_LED_COUNT * WS2812B_BYTES_PER_LED)
/// Calculate the size of the stream buffer used to send the WS2812B LED data out over SPI
#define LED_STRIP_STREAM_SZ (LED_STRIP_LED_COUNT * WS2812_BYTES_PER_LED_2P5MHZ)

/// Create an "instance" of the WS2812B LED strip
static ws2812b_t led_strip;

/// Create the storage and stream buffers
static uint8_t led_strip_buffer[LED_STRIP_BUFF_SZ] = {0u};
static uint8_t led_strip_stream[LED_STRIP_STREAM_SZ] = {0u};

int main(void)
{
      // MCU init - MCU Dependent
      mcu_init();
      
      // SPI init - MCU dependent - Assumes an SPI clock of 2.5Mhz
      spi_init(2.5Mhz)
    
      // Init the WS2812B instance by updating 
      // the strucutre with pointers to buffers, 
      // sizes, and the number of LEDs
      led_strip.p_buffer = led_strip_buffer;
      led_strip.buffer_sz = LED_STRIP_BUFF_SZ;
      led_strip.p_stream = led_strip_stream;
      led_strip.stream_sz = LED_STRIP_STREAM_SZ;
      led_strip.led_count = LED_STRIP_LED_COUNT;
    
      // Pass the desired strip and the desired clock to use
      // This init does verification and returns a true upon success
      // False if there is an issue with the structure passed in
      ws2812b_data_init(&led_strip, WS2812B_INIT_2p5MHz);
    
      // Write he desired color (RGB).  This does red.
      uint8_t red = 0;
      ws2812b_data_set_all(&led_strip, red, 0, 0);
      
      // Generate the stream to pass to SPI driver
      ws2812b_update_stream_2p5mhz(&led_strip);
      
      // Loop forever increasing the brightness of red until it rolls over
      while(true)
      {
            // Sleep - MCU dependent
            // There is no "reset" added to the stream of data
            // It is up to the app to delay between spi writes
            // Arbritary 10ms chosen for example
            sleep_delay(10ms); 
              
            // Increment red, it will roll over 
            ws2812b_data_set_all(&led_strip, ++red, 0, 0);
              
            // Generate the new stream to pass to SPI driver
            ws2812b_update_stream_2p5mhz(&led_strip);
              
            // Write out the data
            spi_write(led_strip.p_stream, led_strip.p_stream_size);
      }
      
      return 0;
}
```

