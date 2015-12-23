/******************************************************************************

    @file    State Machine OS: os_tmr.c
    @author  Rajmund Szymanski
    @date    23.12.2015
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
tmr_id os_tmr_create( void )
/* -------------------------------------------------------------------------- */
{
	tmr_id tmr;

	tmr = core_sys_alloc(sizeof(tmr_t));

	return tmr;
}

/* -------------------------------------------------------------------------- */
void os_tmr_kill( tmr_id tmr )
/* -------------------------------------------------------------------------- */
{
	if (tmr->id != ID_STOPPED)
	{
		core_all_wakeup(tmr, E_STOPPED);
		core_tmr_remove(tmr);
	}
}

/* -------------------------------------------------------------------------- */
static
void os_tmr_start( tmr_id tmr )
/* -------------------------------------------------------------------------- */
{
	if (tmr->id != ID_STOPPED)
	core_tmr_remove(tmr);
	core_tmr_insert(tmr, ID_TIMER);
}

/* -------------------------------------------------------------------------- */
void os_tmr_startUntil( tmr_id tmr, unsigned time, fun_id proc )
/* -------------------------------------------------------------------------- */
{
	tmr->state  = proc;
	tmr->start  = Counter;
	tmr->delay  = time - tmr->start;
	tmr->period = 0;

	os_tmr_start(tmr);
}

/* -------------------------------------------------------------------------- */
void os_tmr_startFor( tmr_id tmr, unsigned delay, fun_id proc )
/* -------------------------------------------------------------------------- */
{
	tmr->state  = proc;
	tmr->start  = Counter;
	tmr->delay  = delay;
	tmr->period = 0;

	os_tmr_start(tmr);
}

/* -------------------------------------------------------------------------- */
void os_tmr_startPeriodic( tmr_id tmr, unsigned period, fun_id proc )
/* -------------------------------------------------------------------------- */
{
	tmr->state  = proc;
	tmr->start  = Counter;
	tmr->delay  = period;
	tmr->period = period;

	os_tmr_start(tmr);
}

/* -------------------------------------------------------------------------- */
static inline __attribute__((always_inline))
unsigned os_tmr_wait( tmr_id tmr, unsigned time, unsigned(*wait)() )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_SUCCESS;

	if (tmr->id != ID_STOPPED)
	{
		event = wait(tmr, time);
	}

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned os_tmr_waitUntil( tmr_id tmr, unsigned time )
/* -------------------------------------------------------------------------- */
{
	return os_tmr_wait(tmr, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned os_tmr_waitFor( tmr_id tmr, unsigned delay )
/* -------------------------------------------------------------------------- */
{
	return os_tmr_wait(tmr, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
