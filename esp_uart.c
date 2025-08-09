#include "esp_uart.h"

#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_err.h"

esp_uart_t *uart_open(int port_num, int tx_pin, int rx_pin, int baud_rate, size_t buf_size, int queue_size) {
    esp_uart_t *uart = malloc(ESP_UART_T_SIZE);
    if (uart == NULL) {
        return NULL;
    }

    uart->port_num = port_num;
    uart->baud_rate = baud_rate;
    uart->buf_size = buf_size;
    uart->rx_pin = rx_pin;
    uart->tx_pin = tx_pin;
    uart->queue = NULL;

    uart_config_t uart_config = {
        .baud_rate = baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk= UART_SCLK_DEFAULT
    };

    esp_err_t err;
    err = uart_driver_install(port_num, buf_size, buf_size, queue_size, uart->queue, 0);
    if (err != ESP_OK) {
        free(uart);
        return NULL;
    }

    err = uart_param_config(port_num, &uart_config);
    if (err != ESP_OK) {
        uart_driver_delete(port_num);
        free(uart);
        return NULL;
    }

    err = uart_set_pin(port_num, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if (err != ESP_OK) {
        uart_driver_delete(port_num);
        free(uart);
        return NULL;
    }

    return uart;
}


void uart_close(esp_uart_t *uart) {
    if (uart == NULL) {
        return;
    }
    uart_driver_delete(uart->port_num);
    free(uart);
}


int uart_readline(esp_uart_t *uart, char *buf, size_t max_len, TickType_t timeout) {
    int bytes_read = 0;

    return bytes_read;
}


int uart_write(esp_uart_t *uart, const char *data) {
    int bytes_written = 0;

    return bytes_written;
}


int uart_writef(esp_uart_t *uart, const char *fmt, ...) {
    
}


bool uart_available(esp_uart_t *uart) {
    return false;
}


void uart_flush_input(esp_uart_t *uart) {
    
}