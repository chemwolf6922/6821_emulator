#pragma once

#include <stdint.h>

/** singleton library */

/**
 * This lib is intended to be used in a apple 1 emulator.
 * Thus interrupts are not implemented.
 * e stands for emulator. 
 */

typedef enum
{
    E6821_PORT_A,
    E6821_PORT_B
} e6821_port_t;

typedef enum
{
    E6821_IRQ_LINE_1,
    E6821_IRQ_LINE_2
} e6821_irq_line_t;

typedef struct e6821_s e6821_t;
struct e6821_s
{
    struct
    {
        void (*reset)(e6821_t* self);
        /**
         * @param rs bit1: rs1, bit0: rs0
         * @param data data
         */
        void (*write)(e6821_t* self, int rs, uint8_t data);
        /**
         * @param rs bit1: rs1, bit0: rs0
         */
        uint8_t (*read)(e6821_t* self, int rs);
        void (*input)(e6821_t* self, e6821_port_t port, uint8_t data);
        void (*set_irq)(e6821_t* self, e6821_port_t port, e6821_irq_line_t line);
    } iface;
    struct
    {
        void (*write_to_device_A)(uint8_t data, void* ctx);
        void* ctx_A;
        void (*write_to_device_B)(uint8_t data, void* ctx);
        void* ctx_B;
        void (*lock)(void* ctx);
        void (*unlock)(void* ctx);
        void* lock_ctx;
    } callbacks;
    struct
    {
        uint8_t data_A;
        uint8_t direction_A;
        union
        {
            struct {
                uint8_t C1_enable:1;
                uint8_t C1_edge:1;
                uint8_t DDR_access:1;
                uint8_t C2_b3:1;
                uint8_t C2_b4:1;
                uint8_t C2_output:1;
                uint8_t IRQ2:1;
                uint8_t IRQ1:1;
            } __attribute__((packed)) bits;
            uint8_t byte; 
        } control_A;

        uint8_t data_B;
        uint8_t direction_B;
        union
        {
            struct {
                uint8_t C1_enable:1;
                uint8_t C1_edge:1;
                uint8_t DDR_access:1;
                uint8_t C2_b3:1;
                uint8_t C2_b4:1;
                uint8_t C2_output:1;
                uint8_t IRQ2:1;
                uint8_t IRQ1:1;
            } __attribute__((packed)) bits;
            uint8_t byte; 
        } control_B;
    } regs;
};

void e6821_init(e6821_t* self);
