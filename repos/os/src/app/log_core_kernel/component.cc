/*
 * \brief  Component transforming core and kernel output to Genode LOG output
 */

/*
 * Copyright (C) 2016 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/* base includes */
#include <base/component.h>
#include <log_session/connection.h>
#include <util/reconstructible.h>

/* os includes */
#include <base/attached_rom_dataspace.h>
#include <timer_session/connection.h>


class Log
{
	private:

		Genode::Attached_rom_dataspace _rom_ds;
		Genode::Log_connection         _log;

		char           _buffer [Genode::Log_session::MAX_STRING_LEN + 1];
		unsigned short _buf_pos { 0 };
		unsigned       _rom_pos { 0 };

		unsigned log_size() const { return _rom_ds.size() - sizeof(_rom_pos); }

		char const * char_from_rom(unsigned offset = 0) const
		{
			return _rom_ds.local_addr<char const>() + sizeof(_rom_pos) +
			       (_rom_pos + offset) % log_size();
		}

		unsigned next_pos(unsigned pos) const {
			return (pos + 1) % log_size(); }

		unsigned end_pos() const {
			return *_rom_ds.local_addr<unsigned volatile>() % log_size(); }

		void _rom_to_log(unsigned const last_pos)
		{
			unsigned up_to_pos = last_pos;

			for (; _rom_pos != next_pos(up_to_pos)
				 ; _rom_pos = next_pos(_rom_pos), up_to_pos = end_pos()) {

				char const c = *char_from_rom();

				_buffer[_buf_pos++] = c;

				if (_buf_pos + 1U < sizeof(_buffer) && c != '\n')
					continue;

				_buffer[_buf_pos] = 0;
				_log.write(Genode::Log_session::String(_buffer));
				_buf_pos = 0;
			}
		}

	public:

		Log (Genode::Env &env, char const * const rom_name,
		     char const * const log_name)
		: _rom_ds(env, rom_name), _log(env, log_name)
		{
			unsigned const pos = end_pos();

			/* initial check whether already log wrapped at least one time */
			enum { COUNT_TO_CHECK_FOR_WRAP = 8 };
			for (unsigned i = 1; i <= COUNT_TO_CHECK_FOR_WRAP; i++) {
				if (*char_from_rom(pos + i) == 0)
					continue;

				/* wrap detected, set pos behind last known pos */
				_rom_pos = next_pos(pos + 1) % log_size();
				break;
			}

			_rom_to_log(pos);
		}

		void log() { _rom_to_log(end_pos()); }
};

struct Monitor
{
	Genode::Env &env;

	Genode::Constructible<Log> output_core;
	Genode::Constructible<Log> output_kernel;

	Timer::Connection timer { env };

	Genode::Signal_handler<Monitor> interval { env.ep(), *this, &Monitor::check };

	Monitor(Genode::Env &env) : env(env)
	{
		try {
			output_core.construct(env, "core_log", "core");
		} catch (...) {
			Genode::warning ("Core output is not available.");
		}

		try {
			output_kernel.construct(env, "kernel_log", "kernel");
		} catch (...) {
			Genode::warning ("Kernel output is not available.");
		}

		timer.sigh(interval);

		enum { EVERY_SECOND = 1000UL * 1000 };
		timer.trigger_periodic(EVERY_SECOND);
	}

	void check()
	{
		if (output_kernel.constructed())
			output_kernel->log();

		if (output_core.constructed())
			output_core->log();
	}
};


void Component::construct(Genode::Env &env) { static Monitor output(env); }
