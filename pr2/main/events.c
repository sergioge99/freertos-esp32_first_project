/* 
	events.c
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "events.h"
#include "esp_log.h"

static const char* TAG = "SE2_TP6";
const int GPIO_TIMER = 16;
TimerHandle_t timer;

void timerHandler(TimerHandle_t t){
	if(!gpio_get_level(GPIO_TIMER)){
		if(gpio_set_level(GPIO_TIMER,1)!=ESP_OK) ESP_LOGE(TAG,"GPIO set error!");
	}
}

void timerInit(unsigned int T){
	//Init GPIO
	gpio_pad_select_gpio(GPIO_TIMER);
	gpio_set_direction(GPIO_TIMER,GPIO_MODE_OUTPUT);
	
	//Init timer
	timer = xTimerCreate("Timer",T,pdTRUE,(void*)0,timerHandler);
	if(timer==NULL){
		ESP_LOGE(TAG,"Timer creation error!");
	}else{
		if(xTimerStart(timer,0)!=pdPASS) ESP_LOGE(TAG,"Timer start error");
	}
}