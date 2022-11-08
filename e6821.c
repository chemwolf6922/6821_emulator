#include "e6821.h"
#include <string.h>

typedef struct
{
    uint8_t data_A;
    uint8_t direction_A;
    union
    {
        struct {
            uint8_t IRQ1:1;
            uint8_t IRQ2:1;
            uint8_t C2_output:1;
            uint8_t C2_b4:1;
            uint8_t C2_b3:1;
            uint8_t DDR_access:1;
            uint8_t C1_edge:1;
            uint8_t C1_enable:1;
        } __attribute__((packed)) bits;
        uint8_t byte; 
    } control_A;

    uint8_t data_B;
    uint8_t direction_B;
    union
    {
        struct {
            uint8_t IRQ1:1;
            uint8_t IRQ2:1;
            uint8_t C2_output:1;
            uint8_t C2_b4:1;
            uint8_t C2_b3:1;
            uint8_t DDR_access:1;
            uint8_t C1_edge:1;
            uint8_t C1_enable:1;
        } __attribute__((packed)) bits;
        uint8_t byte; 
    } control_B;

    struct
    {
        e6821_output_to_device_t write_to_device_A;
        void* ctx_A;
        e6821_output_to_device_t write_to_device_B;
        void* ctx_B;
    } callbacks;
} e6821_t;

static e6821_t e6821 = {0};

void e6821_reset()
{
    memset(&e6821,0,sizeof(e6821));
}

void e6821_set_device_hook(e6821_port_t port, e6821_output_to_device_t write_to_device, void* ctx)
{
    if(port == E6821_PORT_A)
    {
        e6821.callbacks.write_to_device_A = write_to_device;
        e6821.callbacks.ctx_A = ctx;
    }
    else if(port == E6821_PORT_B)
    {
        e6821.callbacks.write_to_device_B = write_to_device;
        e6821.callbacks.ctx_B = ctx;
    }
}

/**
 * @param rs bit1: rs1, bit0: rs0
 * @param data data
 */
void e6821_write(int rs, uint8_t data)
{
    switch (rs)
    {
    case 0b00:{
        if(e6821.control_A.bits.DDR_access)
        {
            /** data A */
            e6821.data_A = (data & e6821.direction_A) | (e6821.data_A & (~e6821.direction_A));
            if(e6821.callbacks.write_to_device_A)
            {
                e6821.callbacks.write_to_device_A(data,e6821.callbacks.ctx_A);
            }
        }
        else
        {
            /** data direction A, does not matter */
            e6821.direction_A = data;
        }
    } break;
    case 0b01:{
        e6821.control_A.byte = data;
    } break;
    case 0b10:{
        if(e6821.control_B.bits.DDR_access)
        {
            /** data B */
            e6821.data_B = (data & e6821.direction_B) | (e6821.data_B & (~e6821.direction_B));
            if(e6821.callbacks.write_to_device_B)
            {
                e6821.callbacks.write_to_device_B(data,e6821.callbacks.ctx_B);
            }
        }
        else
        {
            /** data direction B, does not matter */
            e6821.direction_B = data;
        }
    } break;
    case 0b11:{
        e6821.control_B.byte = data;
    } break;
    default:
        break;
    }
}

/**
 * @param rs bit1: rs1, bit0: rs0
 */
uint8_t e6821_read(int rs)
{
    switch (rs)
    {
    case 0b00:{
        if(e6821.control_A.bits.DDR_access)
        {
            /** data A */
            e6821.control_A.bits.IRQ1 = 0;
            e6821.control_A.bits.IRQ2 = 0;
            return e6821.data_A;
        }
        else
        {
            /** data direction A, does not matter */
            return e6821.direction_A;
        }
    } break;
    case 0b01:{
        /** control A */
        return e6821.control_A.byte;
    } break;
    case 0b10:{
        if(e6821.control_B.bits.DDR_access)
        {
            /** data B */
            e6821.control_B.bits.IRQ1 = 0;
            e6821.control_B.bits.IRQ2 = 0;
            return e6821.data_B;
        }
        else
        {
            /** data direction B, does not matter */
            return e6821.direction_B;
        }
    } break;
    case 0b11:{
        /** control B */
        return e6821.control_B.byte;
    } break;
    default:
        break;
    }
}

void e6821_input_from_device(e6821_port_t port, uint8_t data)
{
    if(port == E6821_PORT_A)
    {
        e6821.data_A = (data & (~e6821.direction_A)) | (e6821.data_A & e6821.direction_A);
    }
    else if(port == E6821_PORT_B)
    {
        e6821.data_B = (data & (~e6821.direction_B)) | (e6821.data_B & e6821.direction_B);
    }
}

void e6821_set_irq(e6821_port_t port, e6821_irq_line_t line)
{
    if(port == E6821_PORT_A)
    {
        if(line == E6821_IRQ_LINE_1)
        {
            e6821.control_A.bits.IRQ1 = 1;
        }
        else if(line == E6821_IRQ_LINE_2 && (e6821.control_A.bits.C2_output==0))
        {
            e6821.control_A.bits.IRQ2 = 1;
        }
    }
    else if(port == E6821_PORT_B)
    {
        if(line == E6821_IRQ_LINE_1)
        {
            e6821.control_B.bits.IRQ1 = 1;
        }
        else if(line == E6821_IRQ_LINE_2 && (e6821.control_B.bits.C2_output==0))
        {
            e6821.control_B.bits.IRQ2 = 1;
        }
    }
}

void e6821_dump(e6821_reg_dump_t* dump)
{
    if(dump)
    {
        dump->data_A = e6821.data_A;
        dump->direction_A = e6821.direction_A;
        dump->contorl_A = e6821.control_A.byte;
        dump->data_B = e6821.data_B;
        dump->direction_B = e6821.direction_B;
        dump->contorl_B = e6821.control_B.byte;
    }
}
