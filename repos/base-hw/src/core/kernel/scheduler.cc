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

void Scheduler::_reset(Claim * const c) {
	_share(c)->_claim = _share(c)->_quota; }


void Scheduler::_reset_claims(unsigned const p)
{
	_rcl[p].for_each([&] (Claim * const c) { _reset(c); });
	_ucl[p].for_each([&] (Claim * const c) { _reset(c); });
}


void Scheduler::_next_round()
{
	_residual = _quota;
	for (unsigned p = MAX_PRIORITY;; p--) {
		_reset_claims(p);
		if (p == MIN_PRIORITY) return;
	}
}


void Scheduler::_consumed(unsigned const q)
{
	if (_residual > q) { _residual -= q; }
	else { _next_round(); }
}


void Scheduler::_set_head(Context * const s, unsigned const q, bool const c)
{
	_head_quota = q;
	_head_claims = c;
	_head = s;
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
	_rcl[_head->_prio].to_tail(_head);
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
		Context * const s = _share(_rcl[p].head());
		if (s && s->_claim) {
			_set_head(s, s->_claim, 1);
			return true;
		}
		if (p == MIN_PRIORITY) break;
	}
	return 0;
}


bool Scheduler::_fill_for_head()
{
	Context * const s = _share(_fills.head());
	if (!s) { return 0; }
	_set_head(s, s->_fill, 0);
	return 1;
}


unsigned Scheduler::_trim_consumption(unsigned & q)
{
	q = Genode::min(Genode::min(q, _head_quota), _residual);
	if (!_head_yields) { return _head_quota - q; }
	_head_yields = 0;
	return 0;
}


void Scheduler::_quota_introduction(Context * const s)
{
	if (s->_ready) { _rcl[s->_prio].insert_tail(s); }
	else { _ucl[s->_prio].insert_tail(s); }
}


void Scheduler::_quota_revokation(Context * const s)
{
	if (s->_ready) { _rcl[s->_prio].remove(s); }
	else { _ucl[s->_prio].remove(s); }
}


void Scheduler::_quota_adaption(Context * const s, unsigned const q)
{
	if (q) { if (s->_claim > q) { s->_claim = q; } }
	else { _quota_revokation(s); }
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
	if (_fill_for_head()) { return; }
	_set_head(&_idle, _fill, 0);
}


bool Scheduler::ready_check(Context * const s1)
{
	assert(_head);

	ready(s1);
	Context * s2 = _head;
	if (!s1->_claim) { return s2 == &_idle; }
	if (!_head_claims) { return 1; }
	if (s1->_prio != s2->_prio) { return s1->_prio > s2->_prio; }
	for (; s2 && s2 != s1; s2 = _share(s2->next_claim())) ;
	return !s2;
}


void Scheduler::ready(Context * const s)
{
	assert(!s->_ready && s != &_idle);

	s->_ready = 1;
	s->_fill = _fill;
	_fills.insert_tail(s);
	if (!s->_quota) { return; }
	_ucl[s->_prio].remove(s);
	if (s->_claim) { _rcl[s->_prio].insert_head(s); }
	else { _rcl[s->_prio].insert_tail(s); }
}


void Scheduler::unready(Context * const s)
{
	assert(s->_ready && s != &_idle);
	s->_ready = 0;
	_fills.remove(s);
	if (!s->_quota) { return; }
	_rcl[s->_prio].remove(s);
	_ucl[s->_prio].insert_tail(s);
}


void Scheduler::yield() { _head_yields = 1; }


void Scheduler::remove(Context * const s)
{
	assert(s != &_idle);

	if (s == _head) _head = nullptr;
	if (s->_ready) { _fills.remove(s); }
	if (!s->_quota) { return; }
	if (s->_ready) { _rcl[s->_prio].remove(s); }
	else { _ucl[s->_prio].remove(s); }
}


void Scheduler::insert(Context * const s)
{
	assert(!s->_ready);
	if (!s->_quota) { return; }
	s->_claim = s->_quota;
	_ucl[s->_prio].insert_head(s);
}


void Scheduler::quota(Context * const s, unsigned const q)
{
	assert(s != &_idle);
	if (s->_quota) { _quota_adaption(s, q); }
	else if (q) { _quota_introduction(s); }
	s->_quota = q;
}


Scheduler::Scheduler(Context & i, unsigned const q,
                             unsigned const f)
: _idle(i), _head_yields(0), _quota(q), _residual(q), _fill(f)
{ _set_head(&i, f, 0); }
