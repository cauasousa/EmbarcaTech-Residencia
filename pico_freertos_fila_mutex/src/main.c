#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "../helpers/button/button.h"
#include "../helpers/buzzer/buzzer.h"
#include "../helpers/joystick/joystick.h"

// Enum para identificar o tipo de evento enviado pela fila
typedef enum
{
    EVENTO_BOTAO,
    EVENTO_JOYSTICK
} TipoEvento;

// Estrutura para representar o conteúdo da fila
typedef struct
{
    TipoEvento tipo; // Tipo do evento
    union
    {
        struct
        {
            uint16_t x; // Valor do eixo X do joystick
            uint16_t y;// Valor do eixo Y do joystick
        } joystick;
    };
} Fila_tipo;

// --- Handles globais ---
QueueHandle_t task_queue;
SemaphoreHandle_t usb_mutex;
SemaphoreHandle_t buzzer_semaphore;

// --- Handles de tarefas ---
TaskHandle_t axis_handle, button_handle, process_handle, actuator_handle;

// --- Tarefa: Leitura do Joystick ---
void read_axis_task_1(void *param)
{

    joystick_setup();

    while (1)
    {

        Fila_tipo data;
        data.tipo = EVENTO_JOYSTICK;
        data.joystick.x = read_joystick_x(); // Lê eixo X
        data.joystick.y = read_joystick_y();// Lê eixo Y

        xQueueSend(task_queue, &data, 0); // Envia os dados para a fila

        vTaskDelay(pdMS_TO_TICKS(100)); // Aguarda 100ms
    }
}

// --- Tarefa: Leitura do botão ---
void read_button_task_2(void *param)
{
    button_setup();

    while (1)
    {
        if (read_button_sw()) // Se botão foi pressionado
        {

            Fila_tipo tipo;
            tipo.tipo = EVENTO_BOTAO;   
            xQueueSend(task_queue, &tipo, 0); // Envia o evento para a fila

            vTaskDelay(pdMS_TO_TICKS(50));
        }
        else
        {
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
}

// --- Tarefa 3: Processamento dos dados recebidos da fila ---
void read_process_task_3(void *param)
{

    Fila_tipo data;

    while (1)
    {
        // Recebe evento da fila com timeout de 10ms
        if (xQueueReceive(task_queue, &data, pdMS_TO_TICKS(10)) == pdTRUE)
        {
            switch (data.tipo)
            {
            case EVENTO_BOTAO:
            {
                // Buzzer autorizado
                xSemaphoreGive(buzzer_semaphore);
                break;
            }
            case EVENTO_JOYSTICK:
            {
                // Protege o printf com mutex
                if (xSemaphoreTake(usb_mutex, pdMS_TO_TICKS(100)))
                {
                    printf("Joystick - VRX: %d, VRY: %d\n", data.joystick.x, data.joystick.y);
                    xSemaphoreGive(usb_mutex);
                }
                // Aciona o buzzer se o movimento for forte
                if ((data.joystick.x > 3000 || data.joystick.x < 1000 || data.joystick.y > 3000 || data.joystick.y < 1000))
                {
                    xSemaphoreGive(buzzer_semaphore);
                }
                break;
            }
            default:{
                break;
            }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(100)); // Espera 100ms 
    }
}

// --- Tarefa 4: Controle do buzzer ---
void controller_buzzer_task_4(void *param)
{

    buzzer_setup();

    while (1)
    {
        // Toca o buzzer se houver permissão (semáforo liberado)
        if (xSemaphoreTake(buzzer_semaphore, 0) == pdTRUE)
        {

            active_beep();                   // Liga o buzzer
            vTaskDelay(pdMS_TO_TICKS(100));  // Toca por 100ms
            desactive_beep();                // Desliga o buzzer
        }else{
            
            vTaskDelay(pdMS_TO_TICKS(50));
        }

    }
}

int main()
{
    stdio_init_all();
    sleep_ms(2000); // Aguarda conexão USB

    
    task_queue = xQueueCreate(5, sizeof(Fila_tipo)); // Cria a fila para troca de mensagens entre tarefas
    usb_mutex = xSemaphoreCreateMutex(); // Cria o mutex para acesso serial (USB)
    buzzer_semaphore = xSemaphoreCreateCounting(2, 0); // Cria semáforo contador para buzzer (2 acessos simultâneos permitidos)

    // Criação das tarefas
    xTaskCreate(read_axis_task_1, "Axis", 256, NULL, 1, &axis_handle);
    xTaskCreate(read_button_task_2, "Button", 256, NULL, 1, &button_handle);
    xTaskCreate(read_process_task_3, "Process", 256, NULL, 1, &process_handle);
    xTaskCreate(controller_buzzer_task_4, "Actuator", 256, NULL, 1, &actuator_handle);

    // Afinidade de núcleo (SMP)
    vTaskCoreAffinitySet(axis_handle, (1 << 0));     // Core 0
    vTaskCoreAffinitySet(button_handle, (1 << 0));   // Core 0
    vTaskCoreAffinitySet(process_handle, (1 << 0));  // Core 1
    vTaskCoreAffinitySet(actuator_handle, (1 << 1)); // Core 1
    vTaskStartScheduler();

    while (1);
}