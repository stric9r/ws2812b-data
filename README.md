# ws2812b-driver

## ws2812b_data
This module generates a stream of data for a WSS2812B LED strip to stream over SPI.
It only works with 2.5Mhz or 5Mhz SPI.  (Note that 5Mhz is not tested yet).
If you need it to work with a different clock speed then you'll need to create a 
``` ws2812b_update_stream_?Mhz(ws2812b_t * p_instance) ``` function.

This works by allowing the app to update the storage buffer with the colors per LED in the strip.
Once ready, the app calls the ```ws2812b_update_stream...``` function and then the passes the 
```ws2812b_t::p_stream``` and ```ws2812b_t::stream_size``` to the platform's SPI write function.
After the write completes, there should be a reset delay added by the app.  
The stream does not contain the reset delay.  It is up the the app to implement this.

## ws2812b_draw
Is an optional add on that treats a pixel or multiple pixels as "object" that need to be "drawn"
by the ws2812b_data module.  It provides methods to draw objects as solids, or blink them.  It also
provides various methods to update an objects attributes such as the length, blink rate, motion/direction, etc.
See the doxygen documentation in the module for more info.

## ws2812b_draw_common.h
Various macros and structures used by the ws2812b modules.

## ws2812b.h
Main include for the project, it includes all headers needed for this project.

## Example Video:
Example of it working [here](https://www.youtube.com/watch?v=ARf2NLlesRc)

## Usage
Using this module is pretty simple.  Below is psuedo-ish code to help understand, it is a very simple example to convey how to use this module.  This example shows the basics of setting up the ws2812b modules, and blink and led that is moving accross a strip while changing the color.

All functions are doxygenated if you want to do more complex led operations.

```
#include "wsb2812b.h"

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

/// Descriptive enum to track index of objects to draw.
/// Think of objects as a single LED point that can
/// have it position movied, it's length expanded, etc.
/// Generic names below, but developer should update to their task.
typedef enum
{
  DRAW_OBJECTS_0,
  DRAW_OBJECTS_1,
  DRAW_OBJECTS_2,
  ...
  DRAW_OBJECTS_N,
  DRAW_OBJECTS_COUNT

}draw_objects_t; // descriptive name only, can be what ever developer wants


/// Array of objects to draw, will be passed to the ws2812b_draw module
ws2812b_draw_object_t objects_buffer[DRAW_OBJECTS_COUNT] = {0};

/// Store of data used by the ws2812b_draw module
static ws2812b_draw_objects_store_t objects_store;

static void example_draw_moving_led(draw_objects_t const object_index);

int main(void)
{
      // MCU init - MCU Dependent
      mcu_init();
      
      // SPI init - MCU dependent - Assumes an SPI clock of 2.5Mhz
      spi_init(2.5Mhz)
    
      // Setup the object store with the desired object array
      // and how many.  Objects are things being drawn.
      // Could also refer to them as led sprites
      objects_store.p_objects = objects_buffer;
      objects_store.object_count = DRAW_OBJECTS_COUNT;
    
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
    
      // Setup the pointers and wipe the slate
      ws2812b_draw_setup(&objects_store, &led_strip1);
      ws2812b_draw_clear_objects();
      
      // Example object to draw
      example_draw_moving_led();
      
      // Generate the stream to pass to SPI driver
      ws2812b_update_stream_2p5mhz(&led_strip);
      
      // Loop forever increasing the brightness of red until it rolls over
      // Update the app game
      while(true)
      {
            // While the timer is not expired - perform updates to the objects
            // ...
            // Every 10 ms update the color to cycle through all colors
            ws2812b_draw_effect_transition_colors(DRAW_OBJECTS_0, 10, 1);
      
            if(timer_1ms_expired())
            {
                // Draw - best if the tick rate is between 1 and 10ms
                ws2812b_draw(APP_TICKS_MS);
    
                // Check if SPI is ready
                if(spi_write_complete())
                {
                    // Create SPI stream
                    ws2812b_update_stream_2p5mhz(&led_strip1);
    
                    // MCU dependent - Write out to SPI - recommend DMA to SPI
                    spi_write(led_strip.p_stream,
                              led_strip.stream_sz);
                }
            }
       }
      
      return 0;
}

/// Setup 2 yellow LEDs to move back and forth across the 
/// entire LED strip while blinking every 100 ms.
static void example_draw_moving_led(draw_objects_t const object_index)
{
    size_t length = 2;
    size_t position = 1u; // position is 1 based
    int32_t blink_rate_ms = 100;
    
    ws2812b_draw_set_action(object_index, DRAW_ACTION_BLINK_BLACK);
    ws2812b_draw_set_position(object_index, position);
    ws2812b_draw_set_length(object_index, length);
    ws2812b_draw_set_duration(object_index, WS2812B_DRAW_FOREVER);
    ws2812b_draw_set_color(object_index, WS2812B_YELLOW);
}
```

