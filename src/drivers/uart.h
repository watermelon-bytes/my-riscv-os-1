#pragma once
#include <types.h>
// Collects info on UART from device tree; initializes and configures device
// driver to match current platform's specs
// Returns:
// 0 on success,
// 1 if UART node could not be found in device tree,
// < 0 libfdt's error code
int init_uart(const void* fdt);

// Outputs string to UART and adds newline
void uart_println(const char[]);

// Copies string to UART output byte-by-byte
void uart_print(const char*);

// Outputs a single character to UART register
// (only lowest 8 bits of the parameter `c`)
void _putchar(char c);
#define putchar(c) _putchar((c))

enum uart_registers {
    UART_RECEIVER_BUFF = 0x0,
    UART_TRANSMITTER_HOLDING,
    UART_INTERRUPT_ENABLE,
    UART_INTERRUPT_ID,
    UART_FIFO_CONTROL,
    UART_LINE_CONTROL,
    UART_MODEM_CONTROL,
    UART_LINE_STATUS,
    UART_MODEM_STATUS,
    UART_SCRATCH,
    UART_DIVISOR_LATCH
};

union uart_line_control_reg {
    u8 raw_value;
    __attribute__((packed)) struct {
        enum {
            UART_CHAR_LEN_5,
            CHAR_LEN_6,
            CHAR_LEN_7,
            CHAR_LEN_8
        } char_bit : 2;
        u8 stop_bits : 1;  // Specifies
        u8 parity_enable : 1;
        u8 even_parity_select : 1;
        u8 stick_parity : 1;
        u8 break_control : 1;
        u8 divisor_latch_access : 1;
    };
};

union uart_line_status_reg {
    u8 raw_value;
    __attribute__((packed)) struct {
        u8 data_ready : 1;
        u8 overrun_error : 1; /**< Gets set to 1 if a received data byte was not
                                 read before */
        u8 parity_error : 1;
        u8 framing_error : 1;
        u8 transmitter_ready : 1; /**< Transmitter Holding Register is Empty */
        u8 transmitter_empty : 1; /**< Transmitter's both Holding Register and
                                     Shift Register are Empty */
    };
};

typedef u8 uart_data_reg_t;

union uart_interrupt_enable_reg {
    u8 raw_value;
    __attribute__((packed)) struct {
        u8 intrp_received_data_available : 1;
        u8 intrp_transmitter_holding_is_empty : 1;
        u8 intrp_reciver_line_status : 1;
        u8 intr_modem_status : 1;
        u8 : 0;
    };
};
