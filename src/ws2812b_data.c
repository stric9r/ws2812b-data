/// ws2812b_data
///
/// This module tracks user changes and updates a stream buffer used by SPI

#include "ws2812b_data.h"


/// Initialize a ws2812b_t structure
///
/// Checks if buffer can accommodate the number of LEDs
///
/// @param p_instance pointer to a ws2812b_t instance
/// @param desired_spi_clk Enum value that selects the desired clock to evaluate
///                        stream buffer size for
///
/// @return TRUE if structure is correct, FALSE otherwise
bool ws2812b_data_init(ws2812b_t * const p_instance,
                       ws2812b_init_state_t const desired_spi_clk)
{
    if(NULL != p_instance)
    {
        p_instance->init_state = WS2812B_INIT_FAILED;

        if((NULL != p_instance->p_buffer) && (NULL != p_instance->p_stream))
        {
            // Verify the buffer size is large enough to account for all LED's
            bool b_size_check =
                (p_instance->led_count * WS2812B_BYTES_PER_LED) <= (p_instance->buffer_sz);

            if(b_size_check)
            {
                size_t const desired = (WS2812B_INIT_2p5MHz == desired_spi_clk) ?
                    WS2812_BYTES_PER_LED_2P5MHZ : WS2812_BYTES_PER_LED_5MHZ;

                b_size_check =
                    (p_instance->led_count * desired) <= (p_instance->stream_sz);

                p_instance->init_state = b_size_check ?
                    desired_spi_clk : WS2812B_INIT_FAILED;

            }
        }
    }

    return (p_instance->init_state != WS2812B_INIT_FAILED);
}

/// Set values for X LED's the ws2912b_t instance
///
/// @param p_instance      The instance of a ws2912b_t structure (LED string)
/// @param led_num_start   The LED start position to update (1 based)
/// @param led_num_to_set  The number of LEDs to update from led_num_start
/// @param red             The red value
/// @param green           The blue value
/// @param blue            The green value
///
/// @return TRUE on success, FALSE otherwise
bool ws2812b_data_set_x(ws2812b_t * const p_instance,
                        size_t const led_num_start,
                        size_t const led_num_to_set,
                        uint8_t const red,
                        uint8_t const green,
                        uint8_t const blue)
{
    bool b_result = false;


    if(0 < led_num_start)
    {
        if(p_instance->init_state != WS2812B_INIT_FAILED)
        {
            size_t const led_idx = (led_num_start - 1u);

            // Verify not beyond bounds
            if(p_instance->led_count > (led_idx + led_num_to_set - 1u))
            {
                size_t const start_idx = (led_idx * WS2812B_BYTES_PER_LED);

                size_t const num_of_bytes =
                      start_idx + (led_num_to_set * WS2812B_BYTES_PER_LED);

                for(size_t idx = start_idx; idx < num_of_bytes; idx += WS2812B_BYTES_PER_LED)
                {
                    p_instance->p_buffer[idx]      = green;
                    p_instance->p_buffer[idx + 1u] = red;
                    p_instance->p_buffer[idx + 2u] = blue;
                }

                b_result = true;
            }
        }
    }
    return b_result;
}

/// Set value for a LED in the ws2912b_t instance
///
/// @param p_instance      The instance of a ws2912b_t structure (LED string)
/// @param led_num         The LED to update (1 based)
/// @param red             The red value
/// @param green           The blue value
/// @param blue            The green value
///
/// @return TRUE on success, FALSE otherwise
bool ws2812b_data_set(ws2812b_t * const p_instance,
                      size_t const led_num,
                      uint8_t const red,
                      uint8_t const green,
                      uint8_t const blue)
{
    return ws2812b_data_set_x(p_instance, led_num, 1u, red, green, blue);
}

/// Set value for all LEDs in the ws2912b_t instance
///
/// @param p_instance      The instance of a ws2912b_t structure (LED string)
/// @param red             The red value
/// @param green           The blue value
/// @param blue            The green value
///
/// @return TRUE on success, FALSE otherwise
bool ws2812b_data_set_all(ws2812b_t * const p_instance,
                          uint8_t const red,
                          uint8_t const green,
                          uint8_t const blue)
{
    return ws2812b_data_set_x(p_instance, 1u, p_instance->led_count, red, green, blue);
}

/// Clear (0's) values for all LEDs in the ws2912b_t instance
///
/// @param p_instance      The instance of a ws2912b_t structure (LED string)
///
/// @return TRUE on success, FALSE otherwise
bool ws2812b_data_clear_all(ws2812b_t * const p_instance)
{
    return ws2812b_data_set_all(p_instance, 0u, 0u, 0u);
}


/// Populate 2.5Mhz stream buffer with storage buffer
///
/// Every 1 bit is converted to a stream of  3 bits
/// So a 1 bit will be 110 and a 0 bit will be 100
///
/// @note that the reset is not part of the stream
/// It is up to the application to delay before sending another stream
///
/// @param p_instance      The instance of a ws2912b_t structure (LED string)
void ws2812b_update_stream_2p5mhz(ws2812b_t * const p_instance)
{
    if(p_instance->init_state == WS2812B_INIT_2p5MHz)
    {
        // Loop through each byte
        size_t const buffer_size = p_instance->buffer_sz;
        //size_t const stream_size = p_instance->stream_sz;
        uint8_t const * const p_buffer = p_instance->p_buffer;
        uint8_t * const p_stream = p_instance->p_stream;

        size_t stream_index = 0;
        uint8_t current_byte = 0;
        int bit_in_byte = 0; // Tracks the bit position in the current byte of p_stream

        for (size_t i = 0; i < buffer_size; i++)
        {
            for (int bit = 7; bit >= 0; bit--)
            {
                uint8_t original_bit = (p_buffer[i] >> bit) & 1;

                // For each bit in the original byte, we will add 3 bits to p_stream
                uint8_t bits_to_add[6] = {1, 1, 0, 1, 0, 0};

                for (int j = 0; j < 3; j++)
                {
                    int k = (original_bit == 1) ? j : (j + 3);
                    current_byte = (current_byte << 1) | bits_to_add[k];
                    bit_in_byte++;

                    if (bit_in_byte == 8)
                    {
                        // Just as a safety check, don't go beyond the stream size
                        //if(stream_index < stream_size)
                        {
                            // If the current byte is full, move to the next byte in p_stream
                            p_stream[stream_index++] = current_byte;
                            current_byte = 0;
                            bit_in_byte = 0;
                        }
                    }
                }
            }
        }

        // Handle the last byte if it's not full
        if (bit_in_byte != 0)
        {
            current_byte <<= (8 - bit_in_byte); // Shift to align to the most significant bit
            p_stream[stream_index] = current_byte;
        }
    }
}

/// Populate 5Mhz stream buffer with storage buffer
///
/// Every 1 bit is converted to a stream of  6 bits
/// So a 1 bit will be 111100 and a 0 bit will be 110000
///
/// @note that the reset is not part of the stream
/// It is up to the application to delay before sending another stream
///
/// @param p_instance      The instance of a ws2912b_t structure (LED string)
void ws2812b_update_stream_5mhz(ws2812b_t * const p_instance)
{
    if(p_instance->init_state == WS2812B_INIT_5MHz)
    {
        // Loop through each byte
        size_t const buffer_size = p_instance->buffer_sz;
        //size_t const stream_size = p_instance->stream_sz;
        uint8_t const * const p_buffer = p_instance->p_buffer;
        uint8_t * const p_stream = p_instance->p_stream;

        size_t stream_index = 0;
        uint8_t current_byte = 0;
        int bit_in_byte = 0; // Tracks the bit position in the current byte of p_stream

        for (size_t i = 0; i < buffer_size; i++)
        {
            for (int bit = 7; bit >= 0; bit--)
            {
                uint8_t original_bit = (p_buffer[i] >> bit) & 1;

                // For each bit in the original byte, we will add 6 bits to p_stream
                uint8_t bits_to_add[12] = {1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0};

                for (int j = 0; j < 6; j++)
                {
                    int k = (original_bit == 1) ? j : (j + 6);
                    current_byte = (current_byte << 1) | bits_to_add[k];
                    bit_in_byte++;

                    if (bit_in_byte == 8)
                    {
                        // Just as a safety check, don't go beyond the stream size
                        //if(stream_index < stream_size)
                        {
                            // If the current byte is full, move to the next byte in p_stream
                            p_stream[stream_index++] = current_byte;
                            current_byte = 0;
                            bit_in_byte = 0;
                        }
                    }
                }
            }
        }

        // Handle the last byte if it's not full
        if (bit_in_byte != 0)
        {
            current_byte <<= (8 - bit_in_byte); // Shift to align to the most significant bit
            p_stream[stream_index] = current_byte;
        }
    }
}
