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
	private:

		template <typename T>
		class List
		{
			private:

				Genode::List<T> _list;
				T *             _last = nullptr;

			public:

				using Element = typename Genode::List<T>::Element;

				void insert_head(T * le)
				{
					_list.insert(le);
					if (!_last) _last = le;
				}

				void insert_tail(T * le)
				{
					_list.insert(le, _last);
					_last = le;
				}

				void remove(T * le)
				{
					_list.remove(le);
					if (_last != le) return;

					for (_last = _list.first(); _last && _last->next();
					     _last = _last->next()) ;
				}

				void to_tail(T * le)
				{
					remove(le);
					insert_tail(le);
				}

				void head_to_tail() { to_tail(head()); }

				T * head() { return _list.first(); }

				template <typename F>
				void for_each(F f) {
					for (T * i = _list.first(); i; i = i->next()) { f(i); } }
		};

		struct Claim : List<Claim>::Element { };
		struct Fill  : List<Fill>::Element { };

	public:

		enum {
			MIN_PRIORITY = 0,
			MAX_PRIORITY = 3,
		};

		class Context : public Claim, public Fill
		{
			private:

				friend class Scheduler;

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

				bool ready() const           { return _ready;        }
				void quota(unsigned const q) { _quota = q;           }
				Claim * next_claim()         { return Claim::next(); }
		};

	private:

		List<Claim>     _rcl[MAX_PRIORITY + 1]; /* ready claims */
		List<Claim>     _ucl[MAX_PRIORITY + 1]; /* unready claims */
		List<Fill>      _fills;                 /* ready fills */
		Context &       _idle;
		Context *       _head;
		unsigned        _head_quota;
		bool            _head_claims;
		bool            _head_yields;
		unsigned const  _quota;
		unsigned        _residual;
		unsigned const  _fill;

		template <typename T>
		static Context * _share(T * const t) { return static_cast<Context *>(t); }

		static void _reset(Claim * const c);

		void     _reset_claims(unsigned const p);
		void     _next_round();
		void     _consumed(unsigned const q);
		void     _set_head(Context * const c, unsigned const q, bool const cl);
		void     _next_fill();
		void     _head_claimed(unsigned const r);
		void     _head_filled(unsigned const r);
		bool     _claim_for_head();
		bool     _fill_for_head();
		unsigned _trim_consumption(unsigned & q);

		/**
		 * Fill 's' becomes a claim due to a quota donation
		 */
		void _quota_introduction(Context * const s);

		/**
		 * Claim 's' looses its state as claim due to quota revokation
		 */
		void _quota_revokation(Context * const s);

		/**
		 * The quota of claim 's' changes to 'q'
		 */
		void _quota_adaption(Context * const s, unsigned const q);

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
		 * Set Context 'c' ready and return wether this outdates current head
		 */
		bool ready_check(Context * const c);

		/**
		 * Set context 'c' ready
		 */
		void ready(Context * const c);

		/**
		 * Set context 'c' unready
		 */
		void unready(Context * const c);

		/**
		 * Current head looses its current claim/fill for this round
		 */
		void yield();

		/**
		 * Remove context 'c' from scheduler
		 */
		void remove(Context * const c);

		/**
		 * Insert context 'c' into scheduler
		 */
		void insert(Context * const c);

		/**
		 * Set quota of share 'c' to 'q'
		 */
		void quota(Context * const c, unsigned const q);

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
