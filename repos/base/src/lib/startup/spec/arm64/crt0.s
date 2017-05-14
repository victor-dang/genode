/**
 * \brief   Startup code for Genode applications on ARM
 * \author  Norman Feske
 * \author  Martin Stein
 * \date    2007-04-28
 */

/*
 * Copyright (C) 2007-2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */


/**************************
 ** .text (program code) **
 **************************/

.section ".text.crt0"

	/* program entry-point */
	.global _start
	_start:
	.global _start_initial_stack
	_start_initial_stack:

	/* make initial value of some registers available to higher-level code */
	ldr x4, =__initial_sp
	mov x3, sp
	str x3, [x4]

	/*
	 * Install initial temporary environment that is replaced later by the
	 * environment that init_main_thread creates.
	 */
	ldr x4, =_stack_high
	mov sp, x4

	/* if this is the dynamic linker, init_rtld relocates the linker */
	bl init_rtld

	/* create proper environment for main thread */
	bl init_main_thread

	/* apply environment that was created by init_main_thread */
	ldr x4, =init_main_thread_result
	ldr x4, [x4]
	mov sp, x4

	/* jump into init C code instead of calling it as it should never return */
	b _main


/*********************************
 ** .bss (non-initialized data) **
 *********************************/

.section ".bss"

	/* stack of the temporary initial environment */
	.p2align 4
	.global __initial_stack_base
	__initial_stack_base:
	.space 8*1024
	_stack_high:

	/* initial value of the SP register */
	.global __initial_sp
	__initial_sp:
	.space 8
