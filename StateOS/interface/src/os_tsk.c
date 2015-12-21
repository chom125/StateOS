/******************************************************************************

    @file    State Machine OS: os_tsk.c
    @author  Rajmund Szymanski
    @date    21.12.2015
    @brief   This file provides set of functions for StateOS.

 ******************************************************************************

    StateOS - Copyright (C) 2013 Rajmund Szymanski.

    This file is part of StateOS distribution.

    StateOS is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published
    by the Free Software Foundation; either version 3 of the License,
    or (at your option) any later version.

    StateOS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#include <os.h>

/* -------------------------------------------------------------------------- */
tsk_id svc_tsk_create( unsigned prio, fun_id state, unsigned size )
/* -------------------------------------------------------------------------- */
{
	size = ASIZE(sizeof(tsk_t) + size);

	tsk_id tsk;

	tsk = core_sys_alloc(size);

	if (tsk)
	{
		tsk->prio  = prio;
		tsk->bprio = prio;
		tsk->state = state;
		tsk->top   = (char *)tsk + size;

		core_tsk_insert(tsk);
	}

	return tsk;
}

/* -------------------------------------------------------------------------- */
void svc_tsk_start( tsk_id tsk )
/* -------------------------------------------------------------------------- */
{
	if (tsk->id == ID_STOPPED)
	{
		tsk->sp = 0;

		core_tsk_insert(tsk);
	}
}

/* -------------------------------------------------------------------------- */
void svc_tsk_stop( void )
/* -------------------------------------------------------------------------- */
{
//	while (System.cur->mlist) mtx_kill(System.cur->mlist);

	core_tsk_remove(System.cur);
	core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
void svc_tsk_kill( tsk_id tsk )
/* -------------------------------------------------------------------------- */
{
//	while (tsk->mlist) mtx_kill(tsk->mlist);

	switch (tsk->id)
	{
	case ID_READY:
		if (tsk != System.cur) // instead use tsk_stop
		core_tsk_remove(tsk);
		break;
	case ID_DELAYED:
		core_tsk_unlink((tsk_id)tsk, E_STOPPED);
		core_tmr_remove((tmr_id)tsk);
		break;
	}
}

/* -------------------------------------------------------------------------- */
void tsk_flip( fun_id state )
/* -------------------------------------------------------------------------- */
{
	System.cur->state = state;

	core_tsk_loop();
}

/* -------------------------------------------------------------------------- */
void svc_tsk_prio( unsigned prio )
/* -------------------------------------------------------------------------- */
{
	System.cur->bprio = prio;

	core_tsk_prio(System.cur, prio);
}

/* -------------------------------------------------------------------------- */
static inline __attribute__((always_inline))
unsigned priv_tsk_sleep( unsigned time, unsigned(*wait)() )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	event = wait(System.cur, time);

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned svc_tsk_sleepUntil( unsigned time )
/* -------------------------------------------------------------------------- */
{
	return priv_tsk_sleep(time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned svc_tsk_sleepFor( unsigned delay )
/* -------------------------------------------------------------------------- */
{
	return priv_tsk_sleep(delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
void svc_tsk_resume( tsk_id tsk, unsigned event )
/* -------------------------------------------------------------------------- */
{
	if (tsk->id == ID_DELAYED)
	core_tsk_wakeup(tsk, event);
}

/* -------------------------------------------------------------------------- */
