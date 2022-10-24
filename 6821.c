#include "6821.h"
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
        s6821_output_to_device_t write_to_device_A;
        void* ctx_A;
        s6821_output_to_device_t write_to_device_B;
        void* ctx_B;
    } callbacks;
} s6821_t;

static s6821_t s6821 = {0};

void s6821_reset()
{
    memset(&s6821,0,sizeof(s6821));
}

void s6821_set_device_hook(s6821_port_t port, s6821_output_to_device_t write_to_device, void* ctx)
{
    if(port == S6821_PORT_A)
    {
        s6821.callbacks.write_to_device_A = write_to_device;
        s6821.callbacks.ctx_A = ctx;
    }
    else if(port == S6821_PORT_B)
    {
        s6821.callbacks.write_to_device_B = write_to_device;
        s6821.callbacks.ctx_B = ctx;
    }
}

/**
 * @param rs bit1: rs1, bit0: rs0
 * @param data data
 */
void s6821_write(int rs, uint8_t data)
{
    switch (rs)
    {
    case 0b00:{
        if(s6821.control_A.bits.DDR_access)
        {
            /** data A */
            s6821.data_A = data;
            if(s6821.callbacks.write_to_device_A)
            {
                s6821.callbacks.write_to_device_A(data,s6821.callbacks.ctx_A);
            }
        }
        else
        {
            /** data direction A, does not matter */
            s6821.direction_A = data;
        }
    } break;
    case 0b01:{
        s6821.control_A.byte = data;
    } break;
    case 0b10:{
        if(s6821.control_B.bits.DDR_access)
        {
            /** data B */
            s6821.data_B = data;
            if(s6821.callbacks.write_to_device_B)
            {
                s6821.callbacks.write_to_device_B(data,s6821.callbacks.ctx_B);
            }
        }
        else
        {
            /** data direction B, does not matter */
            s6821.direction_B = data;
        }
    } break;
    case 0b11:{
        s6821.control_B.byte = data;
    } break;
    default:
        break;
    }
}

/**
 * @param rs bit1: rs1, bit0: rs0
 */
uint8_t s6821_read(int rs)
{
    switch (rs)
    {
    case 0b00:{
        if(s6821.control_A.bits.DDR_access)
        {
            /** data A */
            s6821.control_A.bits.IRQ1 = 0;
            s6821.control_A.bits.IRQ2 = 0;
            return s6821.data_A;
        }
        else
        {
            /** data direction A, does not matter */
            return s6821.direction_A;
        }
    } break;
    case 0b01:{
        /** control A */
        return s6821.control_A.byte;
    } break;
    case 0b10:{
        if(s6821.control_B.bits.DDR_access)
        {
            /** data B */
            s6821.control_B.bits.IRQ1 = 0;
            s6821.control_B.bits.IRQ2 = 0;
            return s6821.data_B;
        }
        else
        {
            /** data direction B, does not matter */
            return s6821.direction_B;
        }
    } break;
    case 0b11:{
        /** control B */
        return s6821.control_B.byte;
    } break;
    default:
        break;
    }
}

void s6821_input_from_device(s6821_port_t port, uint8_t data)
{
    if(port == S6821_PORT_A)
    {
        s6821.data_A = data;
    }
    else if(port == S6821_PORT_B)
    {
        s6821.data_B = data;
    }
}

void s6821_set_irq(s6821_port_t port, s6821_irq_line_t line)
{
    if(port == S6821_PORT_A)
    {
        if(line == S6821_IRQ_LINE_1)
        {
            s6821.control_A.bits.IRQ1 = 1;
        }
        else if(line == S6821_IRQ_LINE_2 && (s6821.control_A.bits.C2_output==0))
        {
            s6821.control_A.bits.IRQ2 = 1;
        }
    }
    else if(port == S6821_PORT_B)
    {
        if(line == S6821_IRQ_LINE_1)
        {
            s6821.control_B.bits.IRQ1 = 1;
        }
        else if(line == S6821_IRQ_LINE_2 && (s6821.control_B.bits.C2_output==0))
        {
            s6821.control_B.bits.IRQ2 = 1;
        }
    }
}

void s6821_dump(s6821_reg_dump_t* dump)
{
    if(dump)
    {
        dump->data_A = s6821.data_A;
        dump->direction_A = s6821.direction_A;
        dump->contorl_A = s6821.control_A.byte;
        dump->data_B = s6821.data_B;
        dump->direction_B = s6821.direction_B;
        dump->contorl_B = s6821.control_B.byte;
    }
}
