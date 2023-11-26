/// ws2812b_draw_common
///
/// This haader has all the common items shared between the ws2812b data/draw
/// and main application

#ifndef WS2812B_DRAW_COMMON_H_
#define WS2812B_DRAW_COMMON_H_

#define WS2812_HALF_POWER_VAL 0x7F
#define WS2812_LOW_POWER_VAL 0x3F

// Set to your needs - The lower the value the less bright the LED
// but also the less current consumed by your application
#define WS2812_POWER_VAL WS2812_LOW_POWER_VAL

// RGB
#define WS2812B_RED        WS2812_POWER_VAL, 0u, 0u
#define WS2812B_GREEN      0u, WS2812_POWER_VAL, 0u
#define WS2812B_BLUE       0u, 0u, WS2812_POWER_VAL
#define WS2812B_WHITE      WS2812_POWER_VAL, WS2812_POWER_VAL, WS2812_POWER_VAL
#define WS2812B_YELLOW     WS2812_POWER_VAL, WS2812_POWER_VAL, 0u
#define WS2812B_VIOLET     WS2812_POWER_VAL, 0u, WS2812_POWER_VAL
#define WS2812B_TEAL       0u, WS2812_POWER_VAL, WS2812_POWER_VAL
#define WS2812B_BLACK      0u, 0u, 0u

/// Forever is ~49 days if tracking by the milli-second
#define WS2812B_DRAW_FOREVER -1

typedef enum
{
    DIRECTION_NOT_MOVING, ///< Object not moving
    DIRECTION_FORWARD,    ///< Object moving from start to end
    DIRECTION_REVERSE,    ///< Object moving from end to start
} ws2812b_direction_t;

typedef enum
{
    DRAW_ACTION_NO_DRAW,           ///< Object is not to be drawn
    DRAW_ACTION_SOLID,             ///< Object is drawn solid, no blink
    DRAW_ACTION_BLINK_TRANSPARENT, ///< Object blink OFF state is transparent
    DRAW_ACTION_BLINK_BLACK        ///< Object blink OFF state is black
} ws2812b_draw_action_t;

typedef enum
{
    BLINK_STATE_ON,   ///< Blink state - LED ON
    BLINK_STATE_OFF,  ///< Blink state - LED OFF
} ws2812b_blink_state_t;

typedef struct
{
  ws2812b_draw_action_t action;      ///< Action to draw a solid, blink, or nothing

  ws2812b_blink_state_t blink_state; ///< When action is blink, Blink state on or off
  int32_t blink_rate_ms;             ///< When action is blink, How often to blink in ms

  int32_t duration_ms;               ///< How long to draw in ms
  size_t length;                     ///< Length of object
  size_t position;                   ///< Current position

  uint8_t red;                       ///< The color red
  uint8_t green;                     ///< The color red
  uint8_t blue;                      ///< The color red

  ws2812b_direction_t direction;     ///< When in motion, direction of travel
  size_t increment_rate_ms;          ///< When in motion, How often to increment position
  bool b_grow;                       ///< When in motion, grow the length?
  bool b_reverse;                    ///< When in motion, reverse direction if hit an end?
  size_t start_position;             ///< When in motion, start position
  size_t end_position;               ///< When in motion, end position
  bool   b_hit_end;                  ///< When in motion, check if the led met a end fo the strip
} ws2812b_draw_object_t;


/// Passed to the ws2812b_draw module for drawing
typedef struct
{
  ws2812b_draw_object_t * p_objects; ///< Pointer to array of objects to draw
  size_t object_count;               ///< The number of objects
} ws2812b_draw_objects_store_t;

#endif /* WS2812B_DRAW_COMMON_H_ */
