#include "uart_utils.h"

#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_err.h"

#include <string.h>
#include <stdlib.h>

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

    bool is_installed = uart_is_driver_installed(port_num);
    if (is_installed == true) {
        free(uart);
        return NULL;
    }

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
    if (uart != NULL) {
        ruart_driver_delete(uart->port_num);
        free(uart);
    }
}


int uart_readline(esp_uart_t *uart, char *buf, size_t max_len, TickType_t timeout) {
    if (uart == NULL || buf == NULL) {
        return 0;
    }

    size_t bytes_read = 0;
    while (bytes_read != max_len) {
        char ch;
        int read = uart_read_bytes(uart->port_num, &ch, 1, timeout);
        if (read == 1) {
            if (ch == '\n' || ch == '\r') {
                break;
            }

            buf[bytes_read] = ch;
            bytes_read++;
        }
        else {
            break;
        }
    }

    buf[bytes_read] = '\0';
    return bytes_read;
}


int uart_write(esp_uart_t *uart, const char *data) {
    int bytes_written = -1;
    if (uart != NULL) {
        bytes_written = uart_write_bytes(uart->port_num, data, strlen(data));
    }

    return bytes_written;
}


int uart_writef(esp_uart_t *uart, const char *fmt, ...) {
    return 0;
}


bool uart_available(esp_uart_t *uart) {
    if (uart == NULL) {
        return false;
    }

    bool result = false;
    size_t size;

    esp_err_t err;
    err = uart_get_buffered_data_len(uart->port_num, &size);
    if (err == ESP_OK && size != 0) {
        result = true;
    }

    return result;
}