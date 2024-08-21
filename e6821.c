#include "e6821.h"
#include <string.h>

static void e6821_reset(e6821_t* this);
static void e6821_write(e6821_t* this, int rs, uint8_t data);
static uint8_t e6821_read(e6821_t* this, int rs);
static void e6821_input(e6821_t* this, e6821_port_t port, uint8_t data);
static void e6821_set_irq(e6821_t* this, e6821_port_t port, e6821_irq_line_t line);

void e6821_init(e6821_t* this)
{
    if(!this)
        return;
    memset(this,0,sizeof(e6821_t));
    this->iface.reset = e6821_reset;
    this->iface.write = e6821_write;
    this->iface.read = e6821_read;
    this->iface.input = e6821_input;
    this->iface.set_irq = e6821_set_irq;
}

static void e6821_reset(e6821_t* this)
{
    memset(&this->regs,0,sizeof(this->regs));
}

/**
 * @param rs bit1: rs1, bit0: rs0
 * @param data data
 */
static void e6821_write(e6821_t* this, int rs, uint8_t data)
{
    if (this->callbacks.lock)
        this->callbacks.lock(this->callbacks.lock_ctx);
    switch (rs)
    {
    case 0b00:{
        if(this->regs.control_A.bits.DDR_access)
        {
            /** data A */
            this->regs.data_A = (data & this->regs.direction_A) | (this->regs.data_A & (~this->regs.direction_A));
            if(this->callbacks.write_to_device_A)
            {
                this->callbacks.write_to_device_A(data, this->callbacks.ctx_A);
            }
        }
        else
        {
            /** data direction A, does not matter */
            this->regs.direction_A = data;
        }
    } break;
    case 0b01:{
        this->regs.control_A.byte = data;
    } break;
    case 0b10:{
        if(this->regs.control_B.bits.DDR_access)
        {
            /** data B */
            this->regs.data_B = (data & this->regs.direction_B) | (this->regs.data_B & (~this->regs.direction_B));
            if(this->callbacks.write_to_device_B)
            {
                this->callbacks.write_to_device_B(data, this->callbacks.ctx_B);
            }
        }
        else
        {
            /** data direction B, does not matter */
            this->regs.direction_B = data;
        }
    } break;
    case 0b11:{
        this->regs.control_B.byte = data;
    } break;
    default:
        break;
    }
    if (this->callbacks.unlock)
        this->callbacks.unlock(this->callbacks.lock_ctx);
}

/**
 * @param rs bit1: rs1, bit0: rs0
 */
static uint8_t e6821_read(e6821_t* this, int rs)
{
    uint8_t ret = 0;
    if (this->callbacks.lock)
        this->callbacks.lock(this->callbacks.lock_ctx);
    switch (rs)
    {
    case 0b00:{
        if(this->regs.control_A.bits.DDR_access)
        {
            /** data A */
            this->regs.control_A.bits.IRQ1 = 0;
            this->regs.control_A.bits.IRQ2 = 0;
            ret = this->regs.data_A;
        }
        else
        {
            /** data direction A, does not matter */
            ret = this->regs.direction_A;
        }
    } break;
    case 0b01:{
        /** control A */
        ret = this->regs.control_A.byte;
    } break;
    case 0b10:{
        if(this->regs.control_B.bits.DDR_access)
        {
            /** data B */
            this->regs.control_B.bits.IRQ1 = 0;
            this->regs.control_B.bits.IRQ2 = 0;
            ret = this->regs.data_B;
        }
        else
        {
            /** data direction B, does not matter */
            ret = this->regs.direction_B;
        }
    } break;
    case 0b11:{
        /** control B */
        ret = this->regs.control_B.byte;
    } break;
    default:
        break;
    }
    if (this->callbacks.unlock)
        this->callbacks.unlock(this->callbacks.lock_ctx);
    return ret;
}

static void e6821_input(e6821_t* this, e6821_port_t port, uint8_t data)
{
    if (this->callbacks.lock)
        this->callbacks.lock(this->callbacks.lock_ctx);
    if(port == E6821_PORT_A)
    {
        this->regs.data_A = (data & (~this->regs.direction_A)) | (this->regs.data_A & this->regs.direction_A);
    }
    else if(port == E6821_PORT_B)
    {
        this->regs.data_B = (data & (~this->regs.direction_B)) | (this->regs.data_B & this->regs.direction_B);
    }
    if (this->callbacks.unlock)
        this->callbacks.unlock(this->callbacks.lock_ctx);
}

static void e6821_set_irq(e6821_t* this, e6821_port_t port, e6821_irq_line_t line)
{
    if (this->callbacks.lock)
        this->callbacks.lock(this->callbacks.lock_ctx);
    if(port == E6821_PORT_A)
    {
        if(line == E6821_IRQ_LINE_1)
        {
            this->regs.control_A.bits.IRQ1 = 1;
        }
        else if(line == E6821_IRQ_LINE_2 && (this->regs.control_A.bits.C2_output==0))
        {
            this->regs.control_A.bits.IRQ2 = 1;
        }
    }
    else if(port == E6821_PORT_B)
    {
        if(line == E6821_IRQ_LINE_1)
        {
            this->regs.control_B.bits.IRQ1 = 1;
        }
        else if(line == E6821_IRQ_LINE_2 && (this->regs.control_B.bits.C2_output==0))
        {
            this->regs.control_B.bits.IRQ2 = 1;
        }
    }
    if (this->callbacks.unlock)
        this->callbacks.unlock(this->callbacks.lock_ctx);
}
