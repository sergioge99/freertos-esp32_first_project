/* 
	servidores.c
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_system.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "computos.h"

static const char* TAG = "SE2_TP6";
SemaphoreHandle_t xSemaphore1,xSemaphore2;

void serverInit(){
	xSemaphore1 = xSemaphoreCreateMutex();
	xSemaphore2 = xSemaphoreCreateMutex();
	if(xSemaphore1 == NULL || xSemaphore2 == NULL) ESP_LOGE(TAG,"Mutex creation error");
}

void S11(){
	
	if(xSemaphoreTake(xSemaphore1,portMAX_DELAY) == pdTRUE){
		CS(10);
		xSemaphoreGive(xSemaphore1);
	}
	
}

void S12(){
	if(xSemaphoreTake(xSemaphore1,portMAX_DELAY) == pdTRUE){
		CS(20);
		xSemaphoreGive(xSemaphore1);
	}
}

void S21(){
	if(xSemaphoreTake(xSemaphore2,portMAX_DELAY) == pdTRUE){
		CS(30);
		xSemaphoreGive(xSemaphore2);
	}
}

void S22(){
	if(xSemaphoreTake(xSemaphore2,portMAX_DELAY) == pdTRUE){
		CS(20);
		xSemaphoreGive(xSemaphore2);
	}
}
