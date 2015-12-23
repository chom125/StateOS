/******************************************************************************

    @file    State Machine OS: os_cnd.c
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
cnd_id CND_create( void )
/* -------------------------------------------------------------------------- */
{
	cnd_id cnd;

	cnd = core_sys_alloc(sizeof(cnd_t));

	return cnd;
}

/* -------------------------------------------------------------------------- */
void CND_kill( cnd_id cnd )
/* -------------------------------------------------------------------------- */
{
	core_all_wakeup(cnd, E_STOPPED);
}

/* -------------------------------------------------------------------------- */
static inline __attribute__((always_inline))
unsigned CND_wait( cnd_id cnd, mtx_id mtx, unsigned time, unsigned(*wait)() )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	if ((event = mtx_give(mtx))           == E_SUCCESS)
	if ((event = OSCall(wait, cnd, time)) == E_SUCCESS)
	     event = mtx_wait(mtx);

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned cnd_waitUntil( cnd_id cnd, mtx_id mtx, unsigned time )
/* -------------------------------------------------------------------------- */
{
	return CND_wait(cnd, mtx, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned cnd_waitFor( cnd_id cnd, mtx_id mtx, unsigned delay )
/* -------------------------------------------------------------------------- */
{
	return CND_wait(cnd, mtx, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
void CND_give( cnd_id cnd, bool all )
/* -------------------------------------------------------------------------- */
{
	if (all) core_all_wakeup(cnd, E_SUCCESS);
	else     core_one_wakeup(cnd, E_SUCCESS);
}

/* -------------------------------------------------------------------------- */
