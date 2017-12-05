/*
 * \brief   Schedules CPU contexts
 * \author  Martin Stein
 * \author  Stefan Kalkowski
 * \date    2014-10-09
 */

/*
 * Copyright (C) 2014-2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _CORE__KERNEL__SCHEDULER_H_
#define _CORE__KERNEL__SCHEDULER_H_

/* core includes */
#include <util/list.h>
#include <util/misc_math.h>
#include <kernel/configuration.h>

namespace Kernel { class Scheduler; }


class Kernel::Scheduler
{
	public:

		enum Priority {
			MIN_PRIORITY = 0,
			MAX_PRIORITY = 3,
		};

		class Context
		{
			private:

				friend class Scheduler;

				Genode::List_element<Context> _claim_le { this };
				Genode::List_element<Context> _fill_le  { this };

				unsigned const _prio;
				unsigned       _quota;
				unsigned       _claim;
				unsigned       _fill;
				bool           _ready = false;

			public:

				/**
				 * Constructor
				 *
				 * \param p  claimed priority
				 * \param q  claimed quota
				 */
				Context(unsigned const p, unsigned const q)
				: _prio(p), _quota(q), _claim(q) { }

				bool ready() const           { return _ready; }
				void quota(unsigned const q) { _quota = q;    }

				bool operator == (Context & other) const {
					return this == &other; }
		};

	private:

		class Context_list
		{
			public:

				using Element = Genode::List_element<Context>;

				void insert_head(Element & e);
				void insert_tail(Element & e);
				void remove(Element & c);
				void to_tail(Element & c);
				void head_to_tail();
				Context * head();

				template <typename F>
				void for_each(F f)
				{
					for (Element * i = _list.first(); i; i = i->next())
						f(*i->object());
				}

			private:

				Genode::List<Element> _list;
				Element *             _last = nullptr;
		};

		Context_list    _rcl[MAX_PRIORITY + 1]; /* ready claims */
		Context_list    _ucl[MAX_PRIORITY + 1]; /* unready claims */
		Context_list    _fills;                 /* ready fills */
		Context &       _idle;
		Context *       _head;
		unsigned        _head_quota;
		bool            _head_claims;
		bool            _head_yields;
		unsigned const  _quota;
		unsigned        _residual;
		unsigned const  _fill;

		void     _next_round();
		void     _consumed(unsigned const q);
		void     _set_head(Context & c, unsigned const q, bool const cl);
		void     _next_fill();
		void     _head_claimed(unsigned const r);
		void     _head_filled(unsigned const r);
		bool     _claim_for_head();
		bool     _fill_for_head();
		unsigned _trim_consumption(unsigned & q);

		/**
		 * Context 'c' becomes a claim due to a quota donation
		 */
		void _quota_introduction(Context & c);

		/**
		 * Context 'c' looses its state as claim due to quota revokation
		 */
		void _quota_revokation(Context & c);

		/**
		 * The quota of context 'c' changes to 'q'
		 */
		void _quota_adaption(Context & c, unsigned const q);

	public:

		/**
		 * Constructor
		 *
		 * \param i  Gets scheduled with static quota when no other context
		 *           is schedulable. Unremovable. All values get ignored.
		 * \param q  total amount of time quota that can be claimed by shares
		 * \param f  time-slice length of the fill round-robin
		 */
		Scheduler(Context & c, unsigned const q, unsigned const f);

		/**
		 * Update head according to the consumption of quota 'q'
		 */
		void update(unsigned q);

		/**
		 * Set context 'c' ready and return wether this outdates current head
		 */
		bool ready_check(Context & c);

		/**
		 * Set context 'c' ready
		 */
		void ready(Context & c);

		/**
		 * Set context 'c' unready
		 */
		void unready(Context & c);

		/**
		 * Current context looses its current claim/fill for this round
		 */
		void yield();

		/**
		 * Remove context 'c' from scheduler
		 */
		void remove(Context & c);

		/**
		 * Insert context 'c' into scheduler
		 */
		void insert(Context & c);

		/**
		 * Set quota of context 'c' to 'q'
		 */
		void quota(Context & c, unsigned const q);

		/*
		 * Accessors
		 */

		Context * head() const { return _head; }
		unsigned head_quota() const {
			return Genode::min(_head_quota, _residual); }
		unsigned quota() const { return _quota; }
		unsigned residual() const { return _residual; }
};

#endif /* _CORE__KERNEL__SCHEDULER_H_ */
