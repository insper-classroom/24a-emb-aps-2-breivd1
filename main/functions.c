#include "functions.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

QueueHandle_t xQueueAdc;
QueueHandle_t xQueueBtn;

void btn_callback(uint gpio, uint32_t events) {
    int red_rise = 11;
    int red_fall = 10;
    int blue_rise = 21;
    int blue_fall = 20;
    int green_rise = 31;
    int green_fall = 30;
    int yellow_rise = 41;
    int yellow_fall = 40;

    if (events == 0x4) {
        if (gpio == RED_BTN) {
            xQueueSendFromISR(xQueueBtn, &red_rise, 0);  // Send to queue
        } else if (gpio == BLUE_BTN) {
            xQueueSendFromISR(xQueueBtn, &blue_rise, 0);  // Send to queue
        } else if (gpio == GREEN_BTN) {
            xQueueSendFromISR(xQueueBtn, &green_rise, 0);  // Send to queue
        } else if (gpio == YELLOW_BTN) {
            xQueueSendFromISR(xQueueBtn, &yellow_rise, 0);  // Send to queue
        }
    }

    if (events == 0x8) {
        if (gpio == RED_BTN) {
            xQueueSendFromISR(xQueueBtn, &red_fall, 0);
        } else if (gpio == BLUE_BTN) {
            xQueueSendFromISR(xQueueBtn, &blue_fall, 0);
        } else if (gpio == GREEN_BTN) {
            xQueueSendFromISR(xQueueBtn, &green_fall, 0);
        } else if (gpio == YELLOW_BTN) {
            xQueueSendFromISR(xQueueBtn, &yellow_fall, 0);
        }
    }
}

void x_task(void *p) {
    int x_buffer[MOVING_AVERAGE_SIZE] = {0};
    int x_index = 0;
    adc_t x_read;
    x_read.axis = 0;

    while (1) {
        adc_select_input(0);  // Select ADC channel for X axis
        int current_read = adc_read();
        if ((current_read - 2047) / 8 > -30 && (current_read - 2047) / 8 < 30) {
            x_buffer[x_index] = 0;  // Dead zone
        } else {
            x_buffer[x_index] = (current_read - 2047) / 256;
        }

        int sum = 0;
        for (int i = 0; i < MOVING_AVERAGE_SIZE; i++) {
            sum += x_buffer[i];
        }
        x_read.val = sum / MOVING_AVERAGE_SIZE;

        xQueueSend(xQueueAdc, &x_read, portMAX_DELAY);
        x_index = (x_index + 1) % MOVING_AVERAGE_SIZE;

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void y_task(void *p) {
    int y_buffer[MOVING_AVERAGE_SIZE] = {0};
    int y_index = 0;
    adc_t y_read;
    y_read.axis = 1;

    while (1) {
        adc_select_input(1);
        int current_read = adc_read();
        if ((current_read - 2047) / 8 > -30 && (current_read - 2047) / 8 < 30) {
            y_buffer[y_index] = 0;  // Dead zone
        } else {
            y_buffer[y_index] = (current_read - 2047) / 256;
        }

        int sum = 0;
        for (int i = 0; i < MOVING_AVERAGE_SIZE; i++) {
            sum += y_buffer[i];
        }
        y_read.val = sum / MOVING_AVERAGE_SIZE;

        xQueueSend(xQueueAdc, &y_read, portMAX_DELAY);
        y_index = (y_index + 1) % MOVING_AVERAGE_SIZE;

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void write(adc_t data) {
    int val = data.val;
    int msb = val >> 8;
    int lsb = val & 0xFF;

    uart_putc_raw(uart1, data.axis);
    uart_putc_raw(uart1, lsb);
    uart_putc_raw(uart1, msb);
    uart_putc_raw(uart1, -1);
}

void uart_task(void *p) {
    adc_t data;

    while (1) {
        if (xQueueReceive(xQueueAdc, &data, portMAX_DELAY)) {
            write(data);
        }
    }
}

void btn_task(void *p) {
    int btn;
    adc_t data;

    while (1) {
        if (xQueueReceive(xQueueBtn, &btn, portMAX_DELAY)) {
            data.axis = 2;
            data.val = btn;
            write(data);
        }
    }
}

void hc06_task(void *p) {
    uart_init(HC06_UART_ID, HC06_BAUD_RATE);
    gpio_set_function(HC06_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(HC06_RX_PIN, GPIO_FUNC_UART);
    hc06_init("breivid", "1234");

    while (true) {
        
    }
}
