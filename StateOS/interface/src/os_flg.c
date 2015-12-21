/******************************************************************************

    @file    State Machine OS: os_flg.c
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
flg_id svc_flg_create( unsigned mask )
/* -------------------------------------------------------------------------- */
{
	flg_id flg;

	flg = core_sys_alloc(sizeof(flg_t));

	if (flg)
	{
		flg->mask = mask;
	}

	return flg;
}

/* -------------------------------------------------------------------------- */
void svc_flg_kill( flg_id flg )
/* -------------------------------------------------------------------------- */
{
	core_all_wakeup(flg, E_STOPPED);
}

/* -------------------------------------------------------------------------- */
static inline __attribute__((always_inline))
unsigned priv_flg_wait( flg_id flg, unsigned flags, unsigned mode, unsigned time, unsigned(*wait)() )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_SUCCESS;

	tsk_id tsk = System.cur;

	tsk->mode  =  mode;
	tsk->flags = (mode & flgAccept) ? flags & ~flg->flags : flags;
	flg->flags &= ~flags | flg->mask;

	if (tsk->flags && ((mode & flgAll) || (tsk->flags == flags)))
	event = wait(flg, time);

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned svc_flg_waitUntil( flg_id flg, unsigned flags, unsigned mode, unsigned time )
/* -------------------------------------------------------------------------- */
{
	return priv_flg_wait(flg, flags, mode, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned svc_flg_waitFor( flg_id flg, unsigned flags, unsigned mode, unsigned delay )
/* -------------------------------------------------------------------------- */
{
	return priv_flg_wait(flg, flags, mode, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
void svc_flg_give( flg_id flg, unsigned flags )
/* -------------------------------------------------------------------------- */
{
	flags = flg->flags |= flags;

	for (tsk_id tsk = flg->queue; tsk; tsk = tsk->queue)
	{
		if (tsk->flags & flags)
		{
			flg->flags &= ~tsk->flags | flg->mask;
			tsk->flags &= ~flags;
			if (tsk->flags && (tsk->mode & flgAll)) continue;
			core_one_wakeup(tsk = tsk->back, E_SUCCESS);
		}
	}
}

/* -------------------------------------------------------------------------- */
