#ifndef PIC_H
#define PIC_H

// PIC ports
#define PIC1_COMMAND    0x20
#define PIC1_DATA       0x21
#define PIC2_COMMAND    0xA0
#define PIC2_DATA       0xA1

// PIC initialization commands
#define PIC_EOI         0x20    // End of interrupt
#define PIC_ICW1_INIT   0x10    // Initialization command word 1
#define PIC_ICW1_ICW4   0x01    // ICW4 needed
#define PIC_ICW4_8086   0x01    // 8086/88 mode

// Remap PIC to interrupts 32-47 (to avoid conflicts with exceptions 0-31)
void pic_remap(void);

// Send End of Interrupt to PIC
void pic_send_eoi(unsigned char irq);

// Enable/disable specific IRQ
void pic_enable_irq(unsigned char irq);
void pic_disable_irq(unsigned char irq);

// Disable all PIC interrupts
void pic_disable(void);

#endif // PIC_H

