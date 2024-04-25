#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hc06.h"

// Button pin definitions
#define BLUE_BTN 20
#define RED_BTN 21
#define GREEN_BTN 14
#define YELLOW_BTN 15

// ADC pin definitions
#define XPIN 26
#define YPIN 27

typedef struct adc {
    int axis;
    int val;
} adc_t;

#define MOVING_AVERAGE_SIZE 5

extern QueueHandle_t xQueueAdc;
extern QueueHandle_t xQueueBtn;

void btn_callback(uint gpio, uint32_t events);
void x_task(void *p);
void y_task(void *p);
void write(adc_t data);
void uart_task(void *p);
void btn_task(void *p);
void hc06_task(void *p);

#endif
