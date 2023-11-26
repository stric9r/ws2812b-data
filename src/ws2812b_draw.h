/// ws2812b_draw
///
/// This module draws objects and places them in a ws2812b_data buffer

#ifndef WS2812B_DRAW_H_
#define WS2812B_DRAW_H_

#include "ws2812b_data.h"
#include "ws2812b_draw_common.h"

#include <stdint.h>


void ws2812b_draw_setup(ws2812b_draw_objects_store_t * const p_objects_store,
                        ws2812b_t * const p_instance);

void ws2812b_draw(int32_t tick_ms);

void ws2812b_draw_clear_objects(void);

void ws2812b_draw_set_action(size_t const element, ws2812b_draw_action_t action);
void ws2812b_draw_set_direction(size_t const element, ws2812b_direction_t direction);
void ws2812b_draw_set_blink_state(size_t const element, ws2812b_blink_state_t const state);
void ws2812b_draw_set_blink_rate(size_t const element, int32_t const rate_ms);
void ws2812b_draw_set_duration(size_t const element, int32_t const duration_ms);
void ws2812b_draw_set_length(size_t const element, size_t const length);
void ws2812b_draw_set_increment_rate(size_t const element, size_t const rate_ms);
void ws2812b_draw_set_position(size_t const element, size_t const position);
void ws2812b_draw_set_color(size_t const element,
                    uint8_t const red,
                    uint8_t const green,
                    uint8_t const blue);
void ws2812b_draw_set_start_position(size_t const element, size_t const position);
void ws2812b_draw_set_end_position(size_t const element, size_t const position);
void ws2812b_draw_set_grow(size_t const element, bool const b_grow);
void ws2812b_draw_set_reverse(size_t const element, bool const b_reverse);

bool ws2812b_draw_get_hit(size_t const element);
ws2812b_direction_t ws2812b_draw_get_direction(size_t const element);
bool ws2812b_draw_get_obj_overlap(size_t const element_1,
                                  size_t const element_2);

void ws2812b_draw_effect_transition_colors(size_t const element,
                                           uint32_t const update_rate_ms,
                                           uint32_t const step);


#endif /* WS2812B_DRAW_H_ */
