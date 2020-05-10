/*  AMiRo LED Wrapper 
 *
 *              Author: Viki (a) Vignesh Natarajan (vikiworks.io)
 */

typedef enum{
    LED_COLOR_GREEN,
    LED_COLOR_RED,
    LED_COLOR_BLUE
}led_color_t;

typedef enum{
    LED_OFF,
    LED_ON
}led_status_t

typedef enum{
    LED_FRONT,
    LED_BACK,
    LED_LEFT,
    LED_RIGH
}led_location_t

SetAllLed(led_color_t color, led_status_t status);
SetLed(led_location_t location, led_color_t color, led_status_t status);
