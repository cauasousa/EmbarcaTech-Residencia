#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "../helpers/setup/setup.h"
#include "../helpers/tasks_logic/tasks_logic.h"
#include <stdio.h>


// Handles das tarefas
TaskHandle_t task1Handle = NULL;
TaskHandle_t task2Handle = NULL;
TaskHandle_t task3Handle = NULL;

int main()
{
    //inicialização do sistema
    setup_init_sys();

    printf("Iniciando o Sistema FreeRTOS no Pico W...\n");

    // Criando a Task1
    xTaskCreate(self_test, "Self_test", 1024, NULL, 1, &task1Handle);
    // Criando a Task2
    xTaskCreate(alive_task, "Alive_task", 512, NULL, 1, &task2Handle);
    // Criando a Task3
    xTaskCreate(monitoring_task, "Monitoring_task", 512, NULL, 1, &task3Handle);
    
    printf("Task1, Task2 e Task3 criadas com sucesso!\n");

    sleep_ms(1000);        // garantir a estabilidade  
    vTaskStartScheduler(); // Inicia o escalonador do FreeRTOS

    while (1)
    {
    } // Nunca deve chegar aqui
}
