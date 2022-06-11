.global kernel_load_idt
kernel_load_idt:
   mov 4(%esp), %eax     /* Get the pointer to the IDT, passed as a parameter. */
   lidt (%eax)           /* Load the new IDT pointer */
   ret

.macro ISR_NOERR index
    .global isr\index
    .type isr\index, @function
    isr\index:
        push $0x00
        push $\index
        jmp isr_common
.endm

.macro ISR_ERR index
    .global isr\index
    .type isr\index, @function
    isr\index:
        push $\index
        jmp isr_common
.endm

/* Interrupt Requests */
ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR   8
ISR_NOERR 9
ISR_ERR   10
ISR_ERR   11
ISR_ERR   12
ISR_ERR   13
ISR_ERR   14
ISR_NOERR 15
ISR_NOERR 16
ISR_NOERR 17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_NOERR 30
ISR_NOERR 31

.extern isr_handler
.type isr_handler, @function

/*
This is our common ISR stub. It saves the processor state, sets
up for kernel mode segments, calls the C-level fault handler,
and finally restores the stack frame.
*/
isr_common:
    pusha                    /* Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax */

    mov %ds, %ax               /* Lower 16-bits of eax = ds. */
    push %eax                /* save the data segment descriptor */

    mov $0x10, %ax       /* load the kernel data segment descriptor */
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    call isr_handler

    pop %eax        /* reload the original data segment descriptor */
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    popa                     /* Pops edi,esi,ebp... */
    add $8, %esp    /* Cleans up the pushed error code and pushed ISR number */
    sti
    iret           /* pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP */


.macro IRQ index
    .global irq\index
    .type irq\index, @function
    irq\index:
        push $\index
        jmp irq_common
.endm

IRQ 0
IRQ 1
IRQ 2
IRQ 3
IRQ 4
IRQ 5
IRQ 6
IRQ 7
IRQ 8
IRQ 9
IRQ 10
IRQ 11
IRQ 12
IRQ 13
IRQ 14
IRQ 15

/*
This is our common IRQ stub. It saves the processor state, sets
up for kernel mode segments, calls the C-level fault handler,
and finally restores the stack frame. 
*/
irq_common:
    pusha                    /* Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax */

    mov %ds, %ax               /* Lower 16-bits of eax = ds. */
    push %eax                /* save the data segment descriptor */

    mov $0x10, %ax       /* load the kernel data segment descriptor */
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    call irq_handler

    pop %eax        /* reload the original data segment descriptor */
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    popa                     /* Pops edi,esi,ebp... */
    add $8, %esp    /* Cleans up the pushed error code and pushed ISR number */
    sti
    iret           /* pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP */

