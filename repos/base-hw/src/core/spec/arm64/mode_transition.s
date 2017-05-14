/*
 * \brief  Transition between kernel/userland
 * \author Stefan Kalkowski
 * \date   2017-05-14
 */

/*
 * Copyright (C) 2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

.section .text

	/*
	 * Page aligned base of mode transition code.
	 *
	 * This position independent code switches between a kernel context and a
	 * user context and thereby between their address spaces. Due to the latter
	 * it must be mapped executable to the same region in every address space.
	 * To enable such switching, the kernel context must be stored within this
	 * region, thus one should map it solely accessable for privileged modes.
	 */
	.p2align 12
	.global _mt_begin
	_mt_begin:

	/*
	 * On user exceptions the CPU has to jump to one of the following
	 * seven entry vectors to switch to a kernel context.
	 */
	.global _mt_kernel_entry_pic
	_mt_kernel_entry_pic:

	/****************************************************************
	 ** Code that switches from a kernel context to a user context **
	 ****************************************************************/

	.global _mt_master_context_begin
	_mt_master_context_begin:
	.space 8

	.global _mt_client_context_ptr
	_mt_client_context_ptr:
	.space 8

	.p2align 2
	.global _mt_user_entry_pic
	_mt_user_entry_pic:

	/* end of the mode transition code */
	.global _mt_end
	_mt_end:
