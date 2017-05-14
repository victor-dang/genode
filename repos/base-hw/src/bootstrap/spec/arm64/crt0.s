/*
 * \brief   Startup code for bootstrap
 * \author  Stefan Kalkowski
 * \date    2017-05-14
 */

/*
 * Copyright (C) 2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

.set STACK_SIZE, 8 * 16 * 1024

.section ".text.crt0"

	.global _start
	_start:


	/***************************
	 ** Zero-fill BSS segment **
	 ***************************/

	adr x0, _bss_local_start
	adr x1, _bss_local_end
	nop
	nop
	ldr x0, [x0]
	ldr x1, [x1]
	1:
	cmp x1, x0
	ble 2f
	str xzr, [x0]
	add x0, x0, #8
	b   1b
	2:


	/********************************
	 ** Setup kernel stack-pointer **
	 ********************************/

	adr x0, _start_stack        /* load stack address into r0 */
	adr x1, _start_stack_size   /* load stack size per cpu into r1 */
	ldr x1, [x1]
	add sp, x0, x1


	/************************************
	 ** Jump to high-level entry point **
	 ************************************/

	b init

	_bss_local_start:
	.quad _bss_start

	_bss_local_end:
	.quad _bss_end

	_start_stack_size:
	.quad STACK_SIZE

	.align 4
	_start_stack:
	.space STACK_SIZE
