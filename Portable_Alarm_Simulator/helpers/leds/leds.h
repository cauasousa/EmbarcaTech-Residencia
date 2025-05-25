#ifndef SETUP_LEDS
#define SETUP_LEDS

// Define os pinos dos LEDs
#define LED_RED 13
#define LED_GREEN 11
#define LED_BLUE 12

void leds_setup(); // CONFIGURAR OS LEDS
void turn_on_led(int led_red, int led_green, int led_blue); // Ativa os leds

#endif