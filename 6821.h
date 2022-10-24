#ifndef _6821_H
#define _6821_H

#include <stdint.h>

/** singleton library */

/**
 * This lib is intended to be used in a apple 1 simulator.
 * Thus interrupts are not implemented.
 * s stands for simulator. 
 */

typedef enum
{
    S6821_PORT_A,
    S6821_PORT_B
} s6821_port_t;

typedef enum
{
    S6821_IRQ_LINE_1,
    S6821_IRQ_LINE_2
} s6821_irq_line_t;

typedef struct
{
    uint8_t data_A;
    uint8_t direction_A;
    uint8_t contorl_A;
    uint8_t data_B;
    uint8_t direction_B;
    uint8_t contorl_B;
} s6821_reg_dump_t;

typedef void(*s6821_output_to_device_t)(uint8_t data ,void* ctx);

void s6821_reset();
void s6821_set_device_hook(s6821_port_t port, s6821_output_to_device_t write_to_device, void* ctx);
/**
 * @param rs bit1: rs1, bit0: rs0
 * @param data data
 */
void s6821_write(int rs, uint8_t data);
/**
 * @param rs bit1: rs1, bit0: rs0
 */
uint8_t s6821_read(int rs);
void s6821_input_from_device(s6821_port_t port, uint8_t data);

void s6821_set_irq(s6821_port_t port, s6821_irq_line_t line);

void s6821_dump(s6821_reg_dump_t* dump);

#endif
