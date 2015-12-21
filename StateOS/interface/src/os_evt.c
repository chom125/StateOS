/******************************************************************************

    @file    State Machine OS: os_evt.c
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
evt_id svc_evt_create( void )
/* -------------------------------------------------------------------------- */
{
	evt_id evt;

	evt = core_sys_alloc(sizeof(evt_t));

	return evt;
}

/* -------------------------------------------------------------------------- */
void svc_evt_kill( evt_id evt )
/* -------------------------------------------------------------------------- */
{
	core_all_wakeup(evt, E_STOPPED);
}

/* -------------------------------------------------------------------------- */
static inline __attribute__((always_inline))
unsigned priv_evt_wait( evt_id evt, unsigned time, unsigned(*wait)() )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	event = wait(evt, time);

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned svc_evt_waitUntil( evt_id evt, unsigned time )
/* -------------------------------------------------------------------------- */
{
	return priv_evt_wait(evt, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned svc_evt_waitFor( evt_id evt, unsigned delay )
/* -------------------------------------------------------------------------- */
{
	return priv_evt_wait(evt, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
void svc_evt_give( evt_id evt, unsigned event )
/* -------------------------------------------------------------------------- */
{
	core_all_wakeup(evt, event);
}

/* -------------------------------------------------------------------------- */
