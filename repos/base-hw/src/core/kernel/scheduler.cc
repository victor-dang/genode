/*
 * \brief   Schedules CPU shares for the execution time of a CPU
 * \author  Martin Stein
 * \date    2014-10-09
 */

/*
 * Copyright (C) 2014-2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#include <base/log.h>
#include <hw/assert.h>
#include <kernel/scheduler.h>

using namespace Kernel;

void Scheduler::Context_list::insert_head(Element & le)
{
	_list.insert(&le);
	if (!_last) _last = &le;
}


void Scheduler::Context_list::insert_tail(Element & le)
{
	_list.insert(&le, _last);
	_last = &le;
}


void Scheduler::Context_list::remove(Element & le)
{
	_list.remove(&le);
	if (_last != &le) return;

	for (_last = _list.first(); _last && _last->next();
	     _last = _last->next()) ;
}


void Scheduler::Context_list::to_tail(Element & le)
{
	remove(le);
	insert_tail(le);
}


void Scheduler::Context_list::head_to_tail() {
	if (_list.first()) to_tail(*_list.first()); }


Scheduler::Context * Scheduler::Context_list::head() {
	return _list.first() ? _list.first()->object() : nullptr; }


void Scheduler::_next_round()
{
	_residual = _quota;

	for (unsigned p = MIN_PRIORITY; p <= MAX_PRIORITY; p++) {
		_rcl[p].for_each([] (Scheduler::Context & c) { c._claim = c._quota; });
		_ucl[p].for_each([] (Scheduler::Context & c) { c._claim = c._quota; });
	}
}


void Scheduler::_consumed(unsigned const q)
{
	if (_residual > q) { _residual -= q; }
	else { _next_round(); }
}


void Scheduler::_set_head(Context & c, unsigned const q, bool const cl)
{
	_head_quota = q;
	_head_claims = cl;
	_head = &c;
}


void Scheduler::_next_fill()
{
	_head->_fill = _fill;
	_fills.head_to_tail();
}


void Scheduler::_head_claimed(unsigned const r)
{
	if (!_head->_quota) { return; }
	_head->_claim = r > _head->_quota ? _head->_quota : r;
	if (_head->_claim || !_head->_ready) { return; }
	_rcl[_head->_prio].to_tail(_head->_claim_le);
}


void Scheduler::_head_filled(unsigned const r)
{
	if (_fills.head() != _head) { return; }
	if (r) { _head->_fill = r; }
	else { _next_fill(); }
}


bool Scheduler::_claim_for_head()
{
	for (unsigned p = MAX_PRIORITY;; p--) {
		Context * const c = _rcl[p].head();
		if (c && c->_claim) {
			_set_head(*c, c->_claim, true);
			return true;
		}
		if (p == MIN_PRIORITY) break;
	}
	return 0;
}


bool Scheduler::_fill_for_head()
{
	Context * const c = _fills.head();
	if (!c) { return 0; }
	_set_head(*c, c->_fill, false);
	return 1;
}


unsigned Scheduler::_trim_consumption(unsigned & q)
{
	q = Genode::min(Genode::min(q, _head_quota), _residual);
	if (!_head_yields) { return _head_quota - q; }
	_head_yields = 0;
	return 0;
}


void Scheduler::_quota_introduction(Context & c)
{
	if (c._ready) { _rcl[c._prio].insert_tail(c._claim_le); }
	else { _ucl[c._prio].insert_tail(c._claim_le); }
}


void Scheduler::_quota_revokation(Context & c)
{
	if (c._ready) { _rcl[c._prio].remove(c._claim_le); }
	else { _ucl[c._prio].remove(c._claim_le); }
}


void Scheduler::_quota_adaption(Context & c, unsigned const q)
{
	if (q) { if (c._claim > q) { c._claim = q; } }
	else { _quota_revokation(c); }
}


void Scheduler::update(unsigned q)
{
	/* do not detract the quota if the head context was removed even now */
	if (_head) {
		unsigned const r = _trim_consumption(q);
		if (_head_claims) { _head_claimed(r); }
		else              { _head_filled(r);  }
		_consumed(q);
	}

	if (_claim_for_head()) { return; }
	if (_fill_for_head())  { return; }
	_set_head(_idle, _fill, false);
}


bool Scheduler::ready_check(Context & c)
{
	assert(_head);

	ready(c);

	if (!c._claim)               { return *_head == _idle; }
	if (!_head_claims)           { return true; }
	if (c._prio != _head->_prio) { return c._prio > _head->_prio; }

	for (Context_list::Element * e = &_head->_claim_le; e; e = e->next())
	     if (*e->object() == c) return false;
	return true;
}


void Scheduler::ready(Context & c)
{
	assert(!c._ready && !(c == _idle));

	c._ready = true;
	c._fill  = _fill;
	_fills.insert_tail(c._fill_le);
	if (!c._quota) { return; }
	_ucl[c._prio].remove(c._claim_le);
	if (c._claim) { _rcl[c._prio].insert_head(c._claim_le); }
	else          { _rcl[c._prio].insert_tail(c._claim_le); }
}


void Scheduler::unready(Context & c)
{
	assert(c._ready && !(c == _idle));

	c._ready = false;
	_fills.remove(c._fill_le);
	if (!c._quota) { return; }
	_rcl[c._prio].remove(c._claim_le);
	_ucl[c._prio].insert_tail(c._claim_le);
}


void Scheduler::yield() { _head_yields = 1; }


void Scheduler::remove(Context & c)
{
	assert(!(c == _idle));

	if (&c == _head) _head = nullptr;
	if (c._ready) { _fills.remove(c._fill_le); }
	if (!c._quota) { return; }
	if (c._ready) { _rcl[c._prio].remove(c._claim_le); }
	else { _ucl[c._prio].remove(c._claim_le); }
}


void Scheduler::insert(Context & c)
{
	assert(!c._ready);
	if (!c._quota) { return; }
	c._claim = c._quota;
	_ucl[c._prio].insert_head(c._claim_le);
}


void Scheduler::quota(Context & c, unsigned const q)
{
	assert(!(c == _idle));
	if (c._quota) { _quota_adaption(c, q); }
	else if (q) { _quota_introduction(c); }
	c._quota = q;
}


Scheduler::Scheduler(Context & i, unsigned const q,
                             unsigned const f)
: _idle(i), _head_yields(0), _quota(q), _residual(q), _fill(f)
{ _set_head(_idle, _fill, false); }
