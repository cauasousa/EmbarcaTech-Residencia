#include <stdio.h>
#include "pico/stdlib.h"
#include "tusb.h"
#include "hardware/gpio.h"

#include "hardware/pwm.h"
#include "hardware/clocks.h"

#include "hardware/i2c.h"

#include "inc/ssd1306.h"


#include "helpers/setup/setup.h"
#include "helpers/leds/leds.h"
#include "helpers/buzzer/buzzer.h"
#include "helpers/display_oled/display_oled.h"

int main() {

    // Inicializa as principais configurações
    setup();

    // Aguarda a conexão USB com o host
    while (!tud_cdc_connected()) {
        sleep_ms(100);
    }
    printf("USB conectado!\n");

    // Loop principal
    while (true) {
        if (tud_cdc_available()) { // Verifica se há dados disponíveis
            char buf[64]; // Buffer para armazenar os dados recebidos
            
            uint32_t count = tud_cdc_read(buf, sizeof(buf)); // Lê os dados
            buf[count] = '\0'; // Adiciona terminador de string

            // Verifica os valores recebidos e acende o LED correspondente
            if (strcmp(buf, "vermelho") == 0) {
                turn_on_led(1,0,0);
            } else if (strcmp(buf, "verde") == 0) {
                turn_on_led(0,1,0);
            } else if (strcmp(buf, "azul") == 0) {
                turn_on_led(0,0,1);
            } else if (strcmp(buf, "som") == 0) {
                touch_buzzer();
            }
            write_display(buf);
            // Ecoa os dados recebidos de volta ao host
            tud_cdc_write(buf, count);
            tud_cdc_write_flush();
        }
        tud_task(); // Executa tarefas USB
    }

    return 0;
}