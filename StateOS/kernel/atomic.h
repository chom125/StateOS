/******************************************************************************

    @file    State Machine OS: osatomic.h
    @author  Rajmund Szymanski
    @date    18.12.2015
    @brief   This file defines set of atomic functions for the Cortex-M devices.

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
/** FOR FUTURE USE *** FOR FUTURE USE *** FOR FUTURE USE *** FOR FUTURE USE **/

#pragma once

#include <osbase.h>

#if    __CORTEX_M >= 3

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

static inline __attribute__((always_inline))
unsigned port_atomic_set( unsigned *a, unsigned v )
{
	unsigned r;
	volatile uint32_t *p = (volatile uint32_t *)a;
	do r = __LDREXW(p); while ((r == 0) && __STREXW(v, p)); __CLREX();
	return r;
}

/* -------------------------------------------------------------------------- */

static inline __attribute__((always_inline))
unsigned port_atomic_clr( unsigned *a, unsigned v )
{
	unsigned r;
	volatile uint32_t *p = (volatile uint32_t *)a;
	do r = __LDREXW(p); while ((r == v) && __STREXW(0, p)); __CLREX();
	return r;
}

/* -------------------------------------------------------------------------- */

static inline __attribute__((always_inline))
unsigned port_atomic_swp( unsigned *a, unsigned v )
{
	unsigned r;
	volatile uint32_t *p = (volatile uint32_t *)a;
	do r = __LDREXW(p); while (__STREXW(v, p));
	return r;
}

/* -------------------------------------------------------------------------- */

static inline __attribute__((always_inline))
unsigned port_atomic_add( unsigned *a, unsigned v )
{
	unsigned r;
	volatile uint32_t *p = (volatile uint32_t *)a;
	do r = __LDREXW(p); while (__STREXW(r + v, p));
	return r;
}

/* -------------------------------------------------------------------------- */

static inline __attribute__((always_inline))
unsigned port_atomic_sub( unsigned *a, unsigned v )
{
	unsigned r;
	volatile uint32_t *p = (volatile uint32_t *)a;
	do r = __LDREXW(p); while (__STREXW(r - v, p));
	return r;
}

/* -------------------------------------------------------------------------- */

static inline __attribute__((always_inline))
unsigned port_atomic_inc( unsigned *a, unsigned v )
{
	unsigned r;
	volatile uint32_t *p = (volatile uint32_t *)a;
	do r = __LDREXW(p); while ((r < v) && __STREXW(r + 1, p)); __CLREX();
	return r;
}

/* -------------------------------------------------------------------------- */

static inline __attribute__((always_inline))
unsigned port_atomic_dec( unsigned *a )
{
	unsigned r;
	volatile uint32_t *p = (volatile uint32_t *)a;
	do r = __LDREXW(p); while ((r > 0) && __STREXW(r - 1, p)); __CLREX();
	return r;
}

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif
