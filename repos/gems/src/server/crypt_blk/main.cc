/*
 * \brief  Block-level de/en-cryption service
 * \author Stefan Kalkowski
 * \date   2016-02-12
 */

/*
 * Copyright (C) 2016 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/* Genode includes */
#include <os/config.h>
#include <block_session/connection.h>
#include <block/component.h>
namespace Crypto {
#include <sodium.h>
}

class Driver : public Block::Driver
{
	private:

		Genode::Allocator_avl      _alloc;
		Block::Connection          _blk_srv;
		Genode::size_t             _blk_size;
		Block::sector_t            _blk_cnt;
		Block::Session::Operations _ops;

	public:

		Driver() : _alloc(Genode::env()->heap()), _blk_srv(&_alloc) {
			_blk_srv.info(&_blk_cnt, &_blk_size, &_ops); }


		/*******************************
		 **  Block::Driver interface  **
		 *******************************/

		Genode::size_t  block_size()     { return _blk_size; }
		Block::sector_t block_count()    { return _blk_cnt;  }
		Block::Session::Operations ops() { return _ops;      }

		//void read(Block::sector_t           block_nr,
		//          Genode::size_t            block_count,
		//          char                     *buffer,
		//          Block::Packet_descriptor &packet)
		//{
		//}
};


struct Factory : Block::Driver_factory
{
	Block::Driver *create() {
		return new (Genode::env()->heap()) Driver(); }

	void destroy(Block::Driver *driver) {
		Genode::destroy(Genode::env()->heap(), driver); }
};


struct Main
{
	Server::Entrypoint &ep;
	struct Factory      factory;
	Block::Root         root;

	class Library_initialization_fault : Genode::Exception {};

	Main(Server::Entrypoint &ep)
	: ep(ep), root(ep, Genode::env()->heap(), factory)
	{
		if (Crypto::sodium_init() == -1) throw Library_initialization_fault();

		Genode::env()->parent()->announce(ep.manage(root));
	}
};


/************
 ** Server **
 ************/

namespace Server {
	char const *name()             { return "crypt_blk_ep";    }
	size_t stack_size()            { return 2048*sizeof(long); }
	void construct(Entrypoint &ep) { static Main server(ep);   }
}
