#include "tasks_logic.h" // Importa definições relacionadas

void monitoring_task(void *parameter)
{   

    while (1)
    {

        float x = read_joystick_x();
        float y = read_joystick_y();

        printf("=-=-=-=-=-=-=-=-=-=-=-\n");
        printf("Joystick x(ADC0): %f\n", x);
        printf("Joystick y(ADC1): %f\n", y);
        printf("microfone (ADC2): %d\n", read_adc_micro());

        if(x > 3.00f || y > 3.00f){
            active_beep();
        }else{
            desactive_beep();
        }
        vTaskDelay(pdMS_TO_TICKS(50)); // Espera
    }
}

void alive_task(void *parameter)
{

    while (1)
    {
        turn_on_led(1, 0, 0); // Liga vermelho
        printf("Aceso led VERMELHO\n");
        vTaskDelay(pdMS_TO_TICKS(500)); // Espera
        turn_on_led(0, 0, 0);           // desliga vermelho
        printf("Desligado led VERMELHO\n");
        vTaskDelay(pdMS_TO_TICKS(500)); // Espera
    }
}

void self_test(void *parameter)
{
    
    turn_on_led(1, 0, 0); // Liga vermelho
    printf("Aceso led VERMELHO\n");
    vTaskDelay(pdMS_TO_TICKS(500)); // Espera
    turn_on_led(0, 0, 0);           // desliga vermelho
    printf("Desligado led VERMELHO\n");
    vTaskDelay(pdMS_TO_TICKS(500)); // Espera

    turn_on_led(0, 1, 0); // Liga verde
    printf("Aceso led VERDE\n");
    vTaskDelay(pdMS_TO_TICKS(500)); // Espera
    turn_on_led(0, 0, 0);           // desliga verde
    printf("Desligado led VERDE\n");
    vTaskDelay(pdMS_TO_TICKS(700)); // Espera

    turn_on_led(0, 0, 1); // Liga AZUL
    printf("Aceso led AZUL\n");
    vTaskDelay(pdMS_TO_TICKS(500)); // Espera
    turn_on_led(0, 0, 0);           // desliga AZUL
    printf("Desligado led AZUL\n");
    vTaskDelay(pdMS_TO_TICKS(700)); // Espera

    printf("Testando Buzzer ...\n");
    touch_buzzer();
    printf("Buzzer Testado!\n");
    vTaskDelay(pdMS_TO_TICKS(700)); // Espera

    printf("Aperte o Botão A para continuar, voce tem 3s\n");
    vTaskDelay(pdMS_TO_TICKS(100)); // Espera

    // Pega o tempo atual + 3000 milissegundos
    absolute_time_t target_time = make_timeout_time_ms(3000);
    // Loop até o tempo atual ultrapassar o tempo alvo
    while (absolute_time_diff_us(get_absolute_time(), target_time) > 0)
    {
        if (!gpio_get(BOTAO_A))
        {
            printf("Botão A pressionado!\n");
            vTaskDelay(pdMS_TO_TICKS(100)); // Espera
            break;
        }
        tight_loop_contents(); // evita que o compilador otimize o loop
    }

    printf("Aperte o Botão B para continuar, voce tem 3s\n");
    vTaskDelay(pdMS_TO_TICKS(100)); // Espera

    target_time = make_timeout_time_ms(3000);
    // Loop até o tempo atual ultrapassar o tempo alvo
    while (absolute_time_diff_us(get_absolute_time(), target_time) > 0)
    {
        if (!gpio_get(BOTAO_B))
        {
            printf("Botão B pressionado!\n");
            break;
        }
        tight_loop_contents(); // evita que o compilador otimize o loop
    }

    printf("Aperte o Botão SW para continuar, voce tem 3s\n");
    vTaskDelay(pdMS_TO_TICKS(100)); // Espera

    target_time = make_timeout_time_ms(3000);
    // Loop até o tempo atual ultrapassar o tempo alvo
    while (absolute_time_diff_us(get_absolute_time(), target_time) > 0)
    {
        if (!gpio_get(BOTAO_SW))
        {
            printf("Botão SW pressionado!\n");
            break;
        }
        tight_loop_contents(); // evita que o compilador otimize o loop
    }

    printf("Joystick x(ADC0): %d\n", read_joystick_x());
    vTaskDelay(pdMS_TO_TICKS(700)); // Espera
    printf("Joystick y(ADC1): %d\n", read_joystick_y());
    vTaskDelay(pdMS_TO_TICKS(700)); // Espera
    printf("microfone (ADC2): %d\n", read_adc_micro());
    vTaskDelay(pdMS_TO_TICKS(700)); // Espera

    vTaskDelete(NULL); // Autodeleta o próprio gerenciador
}
