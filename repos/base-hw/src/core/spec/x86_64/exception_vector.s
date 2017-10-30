/*
 * \brief  Transition between kernel/userland
 * \author Adrian-Ken Rueegsegger
 * \author Martin Stein
 * \author Reto Buerki
 * \author Stefan Kalkowski
 * \date   2011-11-15
 */

/*
 * Copyright (C) 2011-2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

.include "hw/spec/x86_64/gdt.s"

/* offsets of member variables in a CPU context */
.set IP_OFFSET, 17 * 8
.set SP_OFFSET, 20 * 8

/* tss segment constants */
.set TSS_LIMIT, 0x68
.set TSS_TYPE,  0x8900

/* virtual addresses */
.set BASE, 0xffffffc000000000
.set TSS,  BASE + (__tss - _begin)
.set ISR,  BASE
.set ISR_ENTRY_SIZE, 12

.set IDT_FLAGS_PRIVILEGED,   0x8e01
.set IDT_FLAGS_UNPRIVILEGED, 0xee01

.macro _isr_entry
	.align 4, 0x90
.endm

.macro _exception vector
	_isr_entry
	push $0
	push $\vector
	jmp _kernel_entry
.endm

.macro _exception_with_code vector
	_isr_entry
	nop
	nop
	push $\vector
	jmp _kernel_entry
.endm

.macro _idt_entry addr flags
	.word \addr & 0xffff
	.word 0x0008
	.word \flags
	.word (\addr >> 16) & 0xffff
	.long \addr >> 32
	.long 0
.endm

.macro _load_address label reg
	mov \label@GOTPCREL(%rip), %\reg
.endm

.section ".text.crt0"

	_begin:

	/*
	 * On user exceptions the CPU has to jump to one of the following
	 * Interrupt Service Routines (ISRs) to switch to a kernel context.
	 */
	_exception           0
	_exception           1
	_exception           2
	_exception           3
	_exception           4
	_exception           5
	_exception           6
	_exception           7
	_exception_with_code 8
	_exception           9
	_exception_with_code 10
	_exception_with_code 11
	_exception_with_code 12
	_exception_with_code 13
	_exception_with_code 14
	_exception           15
	_exception           16
	_exception_with_code 17
	_exception           18
	_exception           19

	.set vec, 20
	.rept 236
	_exception vec
	.set vec, vec + 1
	.endr

	_kernel_entry:

	pushq %rbp
	pushq %rsi
	pushq %rdi
	pushq %rdx
	pushq %rcx
	pushq %rbx
	pushq %rax
	pushq %r15
	pushq %r14
	pushq %r13
	pushq %r12
	pushq %r11
	pushq %r10
	pushq %r9
	pushq %r8

	/* Restore kernel stack and continue kernel execution */
	_load_address kernel_stack rax
	_load_address kernel_stack_size rbx
	_load_address kernel rcx
	add (%rbx), %rax
	mov %rax, %rsp
	jmp *%rcx


	/*****************************************
	 ** Interrupt Descriptor Table (IDT)    **
	 ** See Intel SDM Vol. 3A, section 6.10 **
	 *****************************************/

	.global __idt
	.align 8
	__idt:

	/* first 128 entries */
	.set isr_addr, ISR
	.rept 0x80
	_idt_entry isr_addr IDT_FLAGS_PRIVILEGED
	.set isr_addr, isr_addr + ISR_ENTRY_SIZE
	.endr

	/* syscall entry 0x80 */
	_idt_entry isr_addr IDT_FLAGS_UNPRIVILEGED
	.set isr_addr, isr_addr + ISR_ENTRY_SIZE

	/* remaing entries */
	.rept 127
	_idt_entry isr_addr IDT_FLAGS_PRIVILEGED
	.set isr_addr, isr_addr + ISR_ENTRY_SIZE
	.endr

	/****************************************
	 ** Task State Segment (TSS)           **
	 ** See Intel SDM Vol. 3A, section 7.7 **
	 ****************************************/

	.global __tss
	.align 8
	__tss:
	.space 36
	.global __tss_client_context_ptr
	__tss_client_context_ptr:
	.space 64

	_define_gdt TSS


.section .text

	/*******************************
	 ** idle loop for idle thread **
	 *******************************/

	.global idle_thread_main
	idle_thread_main:
	pause
	jmp idle_thread_main
