#include "functions.h"

int main() {
    stdio_init_all();
    adc_init();

    adc_gpio_init(XPIN);
    adc_gpio_init(YPIN);

    gpio_init(BLUE_BTN);
    gpio_set_dir(BLUE_BTN, GPIO_IN);
    gpio_pull_up(BLUE_BTN);

    gpio_init(RED_BTN);
    gpio_set_dir(RED_BTN, GPIO_IN);
    gpio_pull_up(RED_BTN);

    gpio_init(GREEN_BTN);
    gpio_set_dir(GREEN_BTN, GPIO_IN);
    gpio_pull_up(GREEN_BTN);

    gpio_init(YELLOW_BTN);
    gpio_set_dir(YELLOW_BTN, GPIO_IN);
    gpio_pull_up(YELLOW_BTN);

    gpio_set_irq_enabled_with_callback(RED_BTN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &btn_callback);
    gpio_set_irq_enabled_with_callback(BLUE_BTN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &btn_callback);
    gpio_set_irq_enabled_with_callback(GREEN_BTN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &btn_callback);
    gpio_set_irq_enabled_with_callback(YELLOW_BTN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &btn_callback);

    xQueueAdc = xQueueCreate(32, sizeof(adc_t));
    xQueueBtn = xQueueCreate(32, sizeof(adc_t));

    printf("Starting tasks\n");

    xTaskCreate(hc06_task, "UART_Task 1", 4096, NULL, 1, NULL);
    xTaskCreate(uart_task, "uart_task", 4096, NULL, 1, NULL);
    xTaskCreate(x_task, "x_task", 4096, NULL, 1, NULL);
    xTaskCreate(y_task, "y_task", 4096, NULL, 1, NULL);
    xTaskCreate(btn_task, "btn_task", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true);
}
