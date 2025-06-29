#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "../helpers/setup_init/setup_init.h"
#include "../helpers/tasks_logic/tasks_logic.h"
#include <stdio.h>


// Handles das tarefas
TaskHandle_t task1Handle = NULL;
TaskHandle_t task2Handle = NULL;
TaskHandle_t task3Handle = NULL;


int main()
{
    //inicialização do sistema
    setup_init();

    printf("Iniciando o Sistema FreeRTOS no Pico W...\n");

    // Criando a Task1
    xTaskCreate(task1, "Task3", 512, NULL, 1, &task1Handle);
    // Criando a Task2
    xTaskCreate(task2, "Task2", 512, NULL, 2, &task2Handle);
    // Criando a Task3
    xTaskCreate(task3, "Task1", 512, NULL, 3, &task3Handle);
    
    printf("Task1, Task2 e Task3 criadas com sucesso!\n");

    vTaskStartScheduler(); // Inicia o escalonador do FreeRTOS

    while (1)
    {
    } // Nunca deve chegar aqui
}
