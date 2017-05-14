/**
 * \brief  arm64 specific relocations
 * \author Stefan Kalkowski
 * \date   2017-05-14
 */

/*
 * Copyright (C) 2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _LIB__LDSO__SPEC__ARM64__RELOCATION_H_
#define _LIB__LDSO__SPEC__ARM64__RELOCATION_H_

#include <relocation_generic.h>
#include <dynamic_generic.h>

namespace Linker {

	/**
	 * Relocation types
	 */
	enum Reloc_types {
		R_JMPSLOT  = 7, /* jump slot                      */
	};

	class Reloc_non_plt;

	typedef Reloc_plt_generic<Elf::Rela, DT_RELA, R_JMPSLOT> Reloc_plt;
	typedef Reloc_jmpslot_generic<Elf::Rela, DT_RELA, false> Reloc_jmpslot;
	typedef Reloc_bind_now_generic<Elf::Rela, DT_RELA>       Reloc_bind_now;
};

class Linker::Reloc_non_plt : public Reloc_non_plt_generic
{
	public:

		Reloc_non_plt(Dependency const &dep, Elf::Rela const *rel, unsigned long size,
		              bool second_pass)
		: Reloc_non_plt_generic(dep)
		{
			error("FIXME: Reloc_non_plt");
		}

		Reloc_non_plt(Dependency const &dep, Elf::Rel const *, unsigned long, bool)
		: Reloc_non_plt_generic(dep)
		{
			error("LD: DT_REL not supported");
			throw Incompatible();
		}
};

#endif /* _LIB__LDSO__SPEC__ARM64__RELOCATION_H_ */
