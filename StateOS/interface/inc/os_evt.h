/******************************************************************************

    @file    State Machine OS: os_evt.h
    @author  Rajmund Szymanski
    @date    23.12.2015
    @brief   This file contains definitions for StateOS.

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

#pragma once

#include <oskernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

// deklaracja zdarzenia 'evt'

#define OS_EVT( evt )                         \
               evt_t evt##__evt = _EVT_INIT(); \
               evt_id evt = & evt##__evt

#define static_EVT( evt )                     \
        static evt_t evt##__evt = _EVT_INIT(); \
        static evt_id evt = & evt##__evt

/* -------------------------------------------------------------------------- */

// utworzenie obiektu typu zdarzenie
// zwraca adres utworzonego obiektu, lub 0
              evt_id   os_evt_create( void );
static inline evt_id      evt_create( void ) { return (evt_id) OSCall(os_evt_create); }

// reset obiektu 'evt'
// wszystkie procesy oczekuj¹ce zostaj¹ wybudzone
// zostaje do nich wys³any komunikat E_STOPPED
              void     os_evt_kill( evt_id evt );
static inline void        evt_kill( evt_id evt ) { OSCall(os_evt_kill, evt); }

// zawieszenie wykonywania aktualnego procesu do czasu 'time'
// lub do wybudzenia przez obiekt 'evt'
// zwraca odebrany komunikat o zdarzeniu, E_STOPPED lub E_TIMEOUT
              unsigned os_evt_waitUntil( evt_id evt, unsigned time );
static inline unsigned    evt_waitUntil( evt_id evt, unsigned time ) { return OSCall(os_evt_waitUntil, evt, time); }

// zawieszenie wykonywania aktualnego procesu na czas 'delay'
// lub do wybudzenia przez obiekt 'evt'
// zwraca odebrany komunikat o zdarzeniu, E_STOPPED lub E_TIMEOUT
              unsigned os_evt_waitFor( evt_id evt, unsigned delay );
static inline unsigned    evt_waitFor( evt_id evt, unsigned delay ) { return OSCall(os_evt_waitFor, evt, delay); }

// zawieszenie wykonywania aktualnego procesu
// do czasu wybudzenia przez obiekt 'evt'
// zwraca odebrany komunikat o zdarzeniu lub E_STOPPED
static inline unsigned    evt_wait( evt_id evt ) { return evt_waitFor(evt, INFINITE); }

// wys³anie komunikatu o zdarzeniu 'event' do obiektu 'evt'
              void     os_evt_give   ( evt_id evt, unsigned event );
static inline void        evt_give   ( evt_id evt, unsigned event ) { OSCall(os_evt_give, evt, event); }
static inline void        evt_giveISR( evt_id evt, unsigned event ) { evt_give(evt, event); }

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

#include <string.h>

// definicja klasy zdarzenia

class Event : public evt_t
{
public:

	 Event( void ): evt_t(_EVT_INIT()) {}

	~Event( void ) { evt_kill(this); }

	unsigned waitUntil( unsigned _time  ) { return evt_waitUntil(this, _time);  }
	unsigned waitFor  ( unsigned _delay ) { return evt_waitFor  (this, _delay); }
	unsigned wait     ( void )            { return evt_wait     (this);         }
	void     give     ( unsigned _event ) {        evt_give     (this, _event); }
	void     giveISR  ( unsigned _event ) {        evt_giveISR  (this, _event); }
};

#endif

/* -------------------------------------------------------------------------- */
