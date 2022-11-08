#ifndef _6821_H
#define _6821_H

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

typedef struct
{
    uint8_t data_A;
    uint8_t direction_A;
    uint8_t contorl_A;
    uint8_t data_B;
    uint8_t direction_B;
    uint8_t contorl_B;
} e6821_reg_dump_t;

typedef void(*e6821_output_to_device_t)(uint8_t data ,void* ctx);

void e6821_reset();
void e6821_set_device_hook(e6821_port_t port, e6821_output_to_device_t write_to_device, void* ctx);
/**
 * @param rs bit1: rs1, bit0: rs0
 * @param data data
 */
void e6821_write(int rs, uint8_t data);
/**
 * @param rs bit1: rs1, bit0: rs0
 */
uint8_t e6821_read(int rs);
void e6821_input_from_device(e6821_port_t port, uint8_t data);

void e6821_set_irq(e6821_port_t port, e6821_irq_line_t line);

void e6821_dump(e6821_reg_dump_t* dump);

#endif
