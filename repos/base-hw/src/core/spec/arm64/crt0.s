/**
 * \brief   Startup code for core on ARM64
 * \author  Stefan Kalkowski
 * \date    2017-05-14
 */

/*
 * Copyright (C) 2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */


/**************************
 ** .text (program code) **
 **************************/

.section ".text"

	/* program entry-point */
	.global _core_start
	_core_start:

	/* create proper environment for main thread */
	bl init_main_thread

	/* apply environment that was created by init_main_thread */
	ldr x0, =init_main_thread_result
	ldr x0, [x0]
	mov sp, x0

	/* jump into init C code instead of calling it as it should never return */
	b _main

