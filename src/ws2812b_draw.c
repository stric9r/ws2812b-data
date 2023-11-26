/// ws2812b_draw
///
/// This module draws objects and places them in a ws2812b_data buffer

#include "ws2812b_draw.h"
#include "ws2812b_data.h"

#include <stdbool.h>
#include <stdint.h>


// Store pointers locally to just make life easier
// This is an effort to make this module work with multiple
// LED strips while keeping the parameter overhead low for all the function calls.
// I don't know if I care for it, BUT I've already committed and it works.
static ws2812b_t * p_strip;
static ws2812b_draw_object_t * p_objs;
static size_t objects_count = 0;

static int32_t tick_ms_elapsed = 0;
static int32_t tick_ms_value = 0;


static bool ws2812b_position_in_range(size_t const led_count, size_t const position);
static void ws2812b_update_position(size_t const element);
static void ws2812b_draw_object(size_t element);

/// Draw the objects, update the tick counter
///
/// @param tick_ms The amount of ticks in milli-seconds that have elapsed
void ws2812b_draw(int32_t tick_ms)
{

    tick_ms_elapsed += tick_ms;
    tick_ms_value = tick_ms;

    if( (NULL != p_objs) &&
        (NULL != p_strip) &&
        (0 < objects_count) )
      {
          // Clear out last draw
          ws2812b_data_clear_all(p_strip);

          // Draw all the elements
          for(size_t idx = 0; idx < objects_count; idx++)
          {
              ws2812b_draw_object((size_t)idx);
              ws2812b_update_position((size_t)idx);
          }
      }
}

/// Update internal pointers to use the instances specified here
///
/// @param p_objects_store  The draw objects to initialize
/// @param p_instance       The instance to the ws2812b_t data
void ws2812b_draw_setup(ws2812b_draw_objects_store_t * const p_objects_store,
                        ws2812b_t * const p_instance)
{
    if( (NULL != p_objects_store) &&
        (NULL != p_instance) &&
        (0 < p_objects_store->object_count) )

      {
          p_strip = p_instance;
          p_objs = p_objects_store->p_objects;
          objects_count = p_objects_store->object_count;
      }
}

/// Initialize all objects to not draw
void ws2812b_draw_clear_objects()
{
    if( (NULL != p_objs) &&
        (NULL != p_strip) &&
        (0 < objects_count) )

      {
          // Disable everything
          for(size_t idx = 0; idx < objects_count; idx++)
          {
              p_objs[idx].action = DRAW_ACTION_NO_DRAW;
              p_objs[idx].direction = DIRECTION_NOT_MOVING;
              p_objs[idx].blink_state = BLINK_STATE_OFF;
              p_objs[idx].duration_ms = 0;
              p_objs[idx].blink_rate_ms = 0;
              p_objs[idx].length = 0;
              p_objs[idx].increment_rate_ms = 0;
              p_objs[idx].position = 0;
              p_objs[idx].red = 0;
              p_objs[idx].green = 0;
              p_objs[idx].blue = 0;
              p_objs[idx].start_position = 0;
              p_objs[idx].end_position = 0;
              p_objs[idx].b_grow = false;
              p_objs[idx].b_reverse = false;
              p_objs[idx].b_hit_end = false;
          }
      }
}

/// Set object property action
///
/// The action property defines if an object is to be drawn
/// a solid, blinking, or not drawn
///
/// @param element The object element to update
/// @param action  The property value to set
void ws2812b_draw_set_action(size_t const element, ws2812b_draw_action_t action)
{
    if( (NULL != p_objs) &&
        (element < objects_count) )

      {
          p_objs[element].action = action;
      }
}

/// Set object property direction
///
/// The direction property defines if an object is not moving, or
/// moving forward or backwards
///
/// @param element The object element to update
/// @param direction  The property value to set
void ws2812b_draw_set_direction(size_t const element, ws2812b_direction_t direction)
{
    if( (NULL != p_objs) &&
        (element < objects_count) )
      {
          p_objs[element].direction = direction;
      }
}

/// Set object property blink state
///
/// The blink state property defines if an objects blink state
/// is ON or OFF.  This is really used to tweak the init state.
/// By default it starts with OFF. The DRAW_ACTION_BLINK... actions
/// will update this automatically.
///
/// @param element The object element to update
/// @param state  The property value to set
void ws2812b_draw_set_blink_state(size_t const element, ws2812b_blink_state_t const state)
{
    if( (NULL != p_objs) &&
        (element < objects_count) )
      {
          p_objs[element].blink_state = state;
      }
}

/// Set object property blink rate
///
/// The blink rate property defines how often a blink occurs
/// for an object
///
/// @param element The object element to update
/// @param rate_ms  The property value to set
void ws2812b_draw_set_blink_rate(size_t const element, int32_t const rate_ms)
{
    if( (NULL != p_objs) &&
        (element < objects_count) )
      {
          p_objs[element].blink_rate_ms = rate_ms;
      }
}

/// Set object property duration
///
/// The duration property defines how long an object will be
/// drawn
///
/// @param element The object element to update
/// @param duration  The property value to set
void ws2812b_draw_set_duration(size_t const element, int32_t const duration_ms)
{
    if( (NULL != p_objs) &&
        (element < objects_count) )
      {
          if(0 <= duration_ms )
          {
              p_objs[element].duration_ms = tick_ms_elapsed + duration_ms;
          }
          else
          {
              p_objs[element].duration_ms = 0x7FFFFFFF;
          }
      }
}

/// Set object property length
///
/// The length property defines how long an object is.
///
/// @param element The object element to update
/// @param length  The property value to set
void ws2812b_draw_set_length(size_t const element, size_t const length)
{
    if( (NULL != p_objs) &&
        (element < objects_count) )
      {
          p_objs[element].length = length;
      }
}

/// Set object property increment rate
///
/// The increment rate property defines how often to update the
/// position based on the ticks.  This is only used when traveling
/// in a direction
///
/// @param element The object element to update
/// @param rate_ms  The property value to set
void ws2812b_draw_set_increment_rate(size_t const element, size_t const rate_ms)
{
  if( (NULL != p_objs) &&
      (element < objects_count) )
      {
          p_objs[element].increment_rate_ms = rate_ms;
      }
}

/// Set object property position
///
/// The position property defines the current position
///
/// @param element The object element to update
/// @param position  The property value to set
void ws2812b_draw_set_position(size_t const element, size_t const position)
{
    if( (NULL != p_objs) &&
        (element < objects_count) )
      {
          p_objs[element].position = position;
      }
}

/// Set object property color (RGB)
///
/// The color property defines the RGB value that each object is
///
/// @param element The object element to update
/// @param red    The property value to set
/// @param green  The property value to set
/// @param blue   The property value to set
void ws2812b_draw_set_color(size_t const element,
                    uint8_t const red,
                    uint8_t const green,
                    uint8_t const blue)
{
    if( (NULL != p_objs) &&
        (element < objects_count) )
      {
          p_objs[element].red = red;
          p_objs[element].green = green;
          p_objs[element].blue = blue;
      }
}

/// Set object property start position
///
/// The start position property defines the start position of an object
/// This is only used when object is traveling in a direction.
///
/// @param element The object element to update
/// @param position The property value to set
void ws2812b_draw_set_start_position(size_t const element, size_t const position)
{
    if( (NULL != p_objs) &&
        (element < objects_count) )
      {
          p_objs[element].start_position = position;
      }
}

/// Set object property end position
///
/// The end position property defines the end position of an object
/// This is only used when object is traveling in a direction.
///
/// @param element The object element to update
/// @param position The property value to set
void ws2812b_draw_set_end_position(size_t const element, size_t const position)
{
    if( (NULL != p_objs) &&
        (element < objects_count) )
      {
          p_objs[element].end_position = position;
      }
}

/// Set object property grow
///
/// The grow property defines if an object will grow when traveling
/// instead of updating the position.  An example would be an equalizer
/// of a single color
///
/// If not set, then the object position is updated.
///
/// @param element The object element to update
/// @param b_grow The property value to set
void ws2812b_draw_set_grow(size_t const element, bool const b_grow)
{
    if( (NULL != p_objs) &&
        (element < objects_count) )
      {
          p_objs[element].b_grow = b_grow;
      }
}

/// Set object property reverse
///
/// The reverse property defines if an object will reverse direction upon
/// meeting the start/end position.
///
/// If not set the object will stay at the last position for it's direction
///
/// @param element The object element to update
/// @param b_resverse The property value to set
void ws2812b_draw_set_reverse(size_t const element, bool const b_reverse)
{
    if( (NULL != p_objs) &&
        (element < objects_count) )
      {
          p_objs[element].b_reverse = b_reverse;
      }
}

/// Get if the position hit the start position or end position
///
/// This only happens when the object is traveling in a direction
///
/// @param element The object element to update
///
/// @return True if the current position equals the start or end position
bool ws2812b_draw_get_hit(size_t const element)
{
    bool b_ret_val = false;

    if( (NULL != p_objs) &&
        (element < objects_count) )
      {
          if(p_objs[element].b_hit_end)
          {
              b_ret_val = true;
              p_objs[element].b_hit_end = false;
          }
      }

    return b_ret_val;
}

/// Get if the direction of the object
///
/// @param element The object element to update
///
/// @return The direction of the object
ws2812b_direction_t ws2812b_draw_get_direction(size_t const element)
{
    ws2812b_direction_t ret_val = DIRECTION_NOT_MOVING;

    if( (NULL != p_objs) &&
        (element < objects_count) )
      {
          ret_val = p_objs[element].direction;
      }

    return ret_val;
}

/// Check if two objects overlap at any point
///
/// @param element_1 The element for an object to check
/// @param element_2 The element for an object to check
///
/// @return True if overlap, false otherwise
bool ws2812b_draw_get_obj_overlap(size_t const element_1,
                                  size_t const element_2)
{
    bool b_ret_val = false;

    if((NULL != p_objs) &&
      (objects_count > element_1) &&
      (objects_count > element_2))
      {
          size_t const e1_start = p_objs[element_1].position;
          size_t const e1_end = e1_start + p_objs[element_1].length-1u;

          size_t const e2_start = p_objs[element_2].position;
          size_t const e2_end = e2_start + p_objs[element_2].length-1u;

          // Check if either end is within the other
          b_ret_val = ((e1_start <= e2_start) && (e1_end >= e2_start)) ||
                      ((e1_start <= e2_end) && (e1_end >= e2_end));
      }

    return b_ret_val;
}

/// Effect that transitions through all RGB colors
/// Call more often than the update_rate_ms
///
/// @param element The object element to update
/// @param update_rate_ms How often to update to colors
/// @param step    How much to increment the color per update time
///                The lower the number the smooth the transitions
void ws2812b_draw_effect_transition_colors(size_t const element,
                                           uint32_t const update_rate_ms,
                                           uint32_t const step)
{
    static uint8_t red = WS2812_POWER_VAL;
    static uint8_t green = 0x0;
    static uint8_t blue = 0x0;

    if( (NULL != p_objs) &&
        (element < objects_count) )
    {
        if( 0 == (tick_ms_elapsed % update_rate_ms))
        {

            // Transition logic
            if (red == WS2812_POWER_VAL && green < WS2812_POWER_VAL && blue == 0)
            {
                green += step;
            }
            else if (green == WS2812_POWER_VAL && red > 0)
            {
                red -= step;
            }
            else if (green == WS2812_POWER_VAL && blue < WS2812_POWER_VAL)
            {
                blue += step;
            }
            else if (blue == WS2812_POWER_VAL && green > 0)
            {
                green -= step;
            }
            else if (blue == WS2812_POWER_VAL && red < WS2812_POWER_VAL)
            {
                red += step;
            }
            else if (red == WS2812_POWER_VAL && blue > 0)
            {
                blue -= step;
            }

            // step on them
            red =   (WS2812_POWER_VAL < red)   ? WS2812_POWER_VAL : red;
            green = (WS2812_POWER_VAL < green) ? WS2812_POWER_VAL : green;
            blue =  (WS2812_POWER_VAL < blue)  ? WS2812_POWER_VAL : blue;

            ws2812b_draw_set_color(element, red, green, blue);
        }
    }
}

/// Up the position of a moving object
///
/// If an object is moving in a direction, this function
/// will update the position / length based on the objects configuration
///
/// @param element The object element to update
static void ws2812b_update_position(size_t const element)
{
    bool b_hit_end = false;
    ws2812b_draw_object_t * p_obj = &p_objs[element];

    size_t led_count = (int32_t)p_strip->led_count;

    bool b_ok = (DIRECTION_NOT_MOVING != p_obj->direction) &&
                (0 < p_obj->increment_rate_ms) &&
                ws2812b_position_in_range(led_count, p_obj->start_position) &&
                ws2812b_position_in_range(led_count, p_obj->end_position);

    if(b_ok)
    {
        // Can we increment/decrement?
        b_ok = (0 == (tick_ms_elapsed % p_obj->increment_rate_ms));

        // Update direction if moving
        if(b_ok)
        {
            if(DIRECTION_FORWARD == p_obj->direction)
            {
                // Keep position, move length
                if(p_obj->b_grow)
                {
                    p_obj->position = p_obj->start_position;
                    ++p_obj->length;

                    size_t const curr_pos =
                          p_obj->position + p_obj->length;
                    size_t const end_length =
                        p_obj->end_position - p_obj->start_position;

                    p_obj->length =
                        ws2812b_position_in_range(led_count, curr_pos) ?
                            p_obj->length : end_length;

                    if(p_obj->end_position <= curr_pos)
                    {
                        p_obj->direction = p_obj->b_reverse ?
                            DIRECTION_REVERSE : DIRECTION_FORWARD;

                        b_hit_end = true;
                    }
                }
                // Keep length, move position
                else
                {
                    ++p_obj->position;

                    p_obj->position =
                        ws2812b_position_in_range(led_count, p_obj->position) ?
                            p_obj->position : p_obj->end_position;

                    if(p_obj->end_position <= p_obj->position)
                    {
                        p_obj->direction = p_obj->b_reverse ?
                            DIRECTION_REVERSE : DIRECTION_FORWARD;
                        b_hit_end = true;
                    }
                }
            }
            else if(DIRECTION_REVERSE == p_obj->direction)
            {
                // Keep position, move length
                if(p_obj->b_grow)
                {
                    p_obj->position = p_obj->start_position;
                    --p_obj->length;

                    size_t curr_pos = (p_obj->position + p_obj->length);

                    p_obj->length =
                        ws2812b_position_in_range(led_count, curr_pos) ?
                            p_obj->length : 0;

                    if(p_obj->start_position >= curr_pos)
                    {
                        p_obj->direction = p_obj->b_reverse ?
                            DIRECTION_FORWARD : DIRECTION_REVERSE;
                        b_hit_end = true;
                    }
                }
                // Keep length, move position
                else
                {
                    --p_obj->position;

                    p_obj->position =
                        ws2812b_position_in_range(led_count, p_obj->position) ?
                            p_obj->position : p_obj->start_position;

                    if(p_obj->start_position >= p_obj->position)
                    {
                        p_obj->direction = p_obj->b_reverse ?
                            DIRECTION_FORWARD : DIRECTION_REVERSE;
                        b_hit_end = true;
                    }
                }
            }
        }
    }

    // Latch, only reset when read
    if(b_hit_end)
    {
        p_obj->b_hit_end = b_hit_end;
    }
}

/// Check if a position is in the range of the current led strip
///
/// @param led_count The length of the led strip
/// @param position  The current position
static bool ws2812b_position_in_range(size_t const led_count, size_t const position)
{
    return (led_count >= position); //no need to worry about negative
}

/// Draw the objects
///
/// @param element The object element to draw
static void ws2812b_draw_object(size_t const element)
{
    if(objects_count > element)
    {
        ws2812b_draw_object_t * p_obj = &p_objs[element];

        if(DRAW_ACTION_NO_DRAW != p_obj->action)
        {
            bool b_draw = false;

            bool b_expired = (tick_ms_elapsed >= p_obj->duration_ms) ;

            // Only draw if duration specified
            if(!b_expired)
            {
                // Draw the object
                if(DRAW_ACTION_SOLID == p_obj->action)
                {
                    b_draw = true;
                }
                // If blink, check if it's time to blink
                else if( (DRAW_ACTION_BLINK_BLACK == p_obj->action) ||
                         (DRAW_ACTION_BLINK_TRANSPARENT == p_obj->action) )
                {
                    // Update blink state based on blink rate
                    if(0 == (tick_ms_elapsed % p_obj->blink_rate_ms))
                    {
                        // Toggle on/off
                        p_obj->blink_state =
                            (BLINK_STATE_ON == p_obj->blink_state) ?
                                BLINK_STATE_OFF : BLINK_STATE_ON;
                    }

                    b_draw = (BLINK_STATE_ON == p_obj->blink_state);
                }
                // Don't draw anything
                else
                {
                    b_draw = false;
                }

                if(b_draw)
                {
                    ws2812b_data_set_x(p_strip,
                                       p_obj->position,
                                       p_obj->length,
                                       p_obj->red,
                                       p_obj->green,
                                       p_obj->blue);
                }
                else
                {
                    // Only fill in black if not wanting to show any data
                    // Check if transparent blink, if so, what ever was in
                    // this led spot will stay
                    if((DRAW_ACTION_BLINK_TRANSPARENT != p_obj->action))
                    {
                        ws2812b_data_set_x(p_strip,
                                           p_obj->position,
                                           p_obj->length,
                                           WS2812B_BLACK);
                    }
                }
            }
        }
    }
}
