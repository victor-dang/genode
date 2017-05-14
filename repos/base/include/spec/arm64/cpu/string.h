/*
 * \brief  ARM64-specific memcpy
 * \author Stefan Kalkowski
 * \date   2017-05-13
 */

/*
 * Copyright (C) 2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _INCLUDE__SPEC__ARM64__CPU__STRING_H_
#define _INCLUDE__SPEC__ARM64__CPU__STRING_H_

namespace Genode {

	/**
	 * Copy memory block
	 *
	 * \param dst   destination memory block
	 * \param src   source memory block
	 * \param size  number of bytes to copy
	 *
	 * \return      Number of bytes not copied
	 */
	inline size_t memcpy_cpu(void *dst, const void *src, size_t size)
	{
		unsigned char * d = (unsigned char *)dst;
		unsigned char * s = (unsigned char *)src;
		for (unsigned i = 0; i < size; i++) d[i] = s[i];
		return 0;
	}
}

#endif /* _INCLUDE__SPEC__ARM64__CPU__STRING_H_ */
