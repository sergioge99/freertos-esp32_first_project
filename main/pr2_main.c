/* 
	PR2_MAIN.C
	
	Autores:	Oscar Baselga Lahoz
				Sergio García Esteban
				
*/
#include <stdio.h>
#include "sdkconfig.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "servidores.h"
#include "events.h"
#include "computos.h"


/*--- Global variables ---*/
static const char* TAG = "SE2_TP6";
const int GPIO_IRQ = 17;

QueueHandle_t xMailbox;
TimerHandle_t clock1, clock2, clock3;
SemaphoreHandle_t sem1, sem2, sem3;

/*---    Code    ---*/
void tarea1_release(TimerHandle_t t)
{
	xSemaphoreGive(sem1);
}
void tarea1(void *pvParameter)
{
    while(1) {
		vTaskDelay(1 / portTICK_PERIOD_MS);
		if(xSemaphoreTake(sem1,portMAX_DELAY) == pdTRUE){
			ESP_LOGI(TAG,"Empieza tarea 1");
			CS(10);
			S11();
			ESP_LOGI(TAG,"Termina tarea 1");
		}
    }
    vTaskDelete(NULL);
}

void tarea2_release(TimerHandle_t t)
{
	xSemaphoreGive(sem2);
}
void tarea2(void *pvParameter)
{
    while(1) {
		vTaskDelay(1 / portTICK_PERIOD_MS);
		if(xSemaphoreTake(sem2,portMAX_DELAY) == pdTRUE){
			ESP_LOGI(TAG,"Empieza tarea 2");
			CS(60);
			S21();
			ESP_LOGI(TAG,"Termina tarea 2");
		}
    }
    vTaskDelete(NULL);
}

void tarea3_release(TimerHandle_t t)
{
	xSemaphoreGive(sem3);
}
void tarea3(void *pvParameter)
{
    while(1) {
		vTaskDelay(1 / portTICK_PERIOD_MS);
		if(xSemaphoreTake(sem3,portMAX_DELAY) == pdTRUE){
			ESP_LOGI(TAG,"Empieza tarea 3");
			S22();
			CS(30);
			S12();
			ESP_LOGI(TAG,"Termina tarea 3");
		}
    }
    vTaskDelete(NULL);
}

void SS(void *pvParameter)
{
	uint32_t nextstart, timestamp=0, activationtime;
	uint32_t period = 120;
	long d;
	nextstart = xTaskGetTickCountFromISR();
    while(1) {
		xQueueReceive(xMailbox,&timestamp,portMAX_DELAY);
		ESP_LOGI(TAG,"Empieza SS");
		
		CS(15);

		if(timestamp>nextstart) activationtime = timestamp;
		else activationtime = nextstart;
		nextstart = activationtime + (period / portTICK_PERIOD_MS);
		d = (long)nextstart - (long)xTaskGetTickCountFromISR();
		if(d<0) d=0;
		vTaskDelay(d);
		ESP_LOGI(TAG,"Termina SS");
    }
    vTaskDelete(NULL);
}

void IRAM_ATTR myISR(void* arg){
	uint32_t timestamp = xTaskGetTickCountFromISR();
	xQueueSendFromISR(xMailbox,&timestamp,pdFALSE);
	gpio_set_level(GPIO_TIMER,0);
}

void app_main()
{
	//Init GPIO con interrupciones
	gpio_pad_select_gpio(GPIO_IRQ);
	if(gpio_set_direction(GPIO_IRQ,GPIO_MODE_INPUT)!=ESP_OK) ESP_LOGE(TAG,"GPIO error 1!");
	if(gpio_install_isr_service(0)!=ESP_OK) ESP_LOGE(TAG,"GPIO error 2!");
	if(gpio_isr_handler_add(GPIO_IRQ,myISR,NULL)!=ESP_OK) ESP_LOGE(TAG,"GPIO error 3!");
	if(gpio_set_intr_type(GPIO_IRQ,GPIO_INTR_POSEDGE)!=ESP_OK) ESP_LOGE(TAG,"GPIO error 4!");
	if(gpio_intr_enable(GPIO_IRQ)!=ESP_OK) ESP_LOGE(TAG,"GPIO error 5!");
	
	//Init Events
	timerInit(120 / portTICK_PERIOD_MS);
	xMailbox = xQueueCreate(100,4);
	
	//Init Servers
	serverInit();
	
	//Init Tareas
	clock1 = xTimerCreate("Timer1",100/portTICK_PERIOD_MS,pdTRUE,(void*)0,tarea1_release);
	if(xTimerStart(clock1,0)!=pdPASS) ESP_LOGE(TAG,"Timer1 start error!");
	sem1 = xSemaphoreCreateBinary();
    xTaskCreate(&tarea1, "tarea1", 8192, NULL, 5, NULL);
	
	clock2 = xTimerCreate("Timer2",200/portTICK_PERIOD_MS,pdTRUE,(void*)0,tarea2_release);
	if(xTimerStart(clock2,0)!=pdPASS) ESP_LOGE(TAG,"Timer2 start error!");
	sem2 = xSemaphoreCreateBinary();
    xTaskCreate(&tarea2, "tarea2", 8192, NULL, 3, NULL);
	
	clock3 = xTimerCreate("Timer3",400/portTICK_PERIOD_MS,pdTRUE,(void*)0,tarea3_release);
	if(xTimerStart(clock3,0)!=pdPASS) ESP_LOGE(TAG,"Timer3 start error!");
	sem3 = xSemaphoreCreateBinary();
    xTaskCreate(&tarea3, "tarea3", 8192, NULL, 1, NULL);
	
	xTaskCreate(&SS, "SS", 8192, NULL, 4, NULL);
	
	ESP_LOGI(TAG,"TODO INICIALIZADO!");
}
