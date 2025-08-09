#pragma once

#include <stddef.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"

#define DEFAULT_EVENT_QUEUE_SIZE 5;

#define ESP_UART_T_SIZE sizeof(esp_uart_t)

typedef struct {
    int port_num;
    int tx_pin;
    int rx_pin;
    int baud_rate;
    size_t buf_size;
    QueueHandle_t *queue;
} esp_uart_t;

esp_uart_t *uart_open(int port_num, int tx_pin, int rx_pin, int baud_rate, size_t buf_size, int queue_size);

void uart_close(esp_uart_t *uart);

int uart_readline(esp_uart_t *uart, char *buf, size_t max_len, TickType_t timeout);

int uart_write(esp_uart_t *uart, const char *data);

int uart_writef(esp_uart_t *uart, const char *fmt, ...);

bool uart_available(esp_uart_t *uart);

void uart_flush_input(esp_uart_t *uart);
