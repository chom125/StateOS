/******************************************************************************

    @file    State Machine OS: os_tmr.h
    @author  Rajmund Szymanski
    @date    22.12.2015
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

// deklaracja zegara 'tmr'

#define OS_TMR( tmr )                         \
               tmr_t tmr##__tmr = _TMR_INIT(); \
               tmr_id tmr = & tmr##__tmr

#define static_TMR( tmr )                     \
        static tmr_t tmr##__tmr = _TMR_INIT(); \
        static tmr_id tmr = & tmr##__tmr

/* -------------------------------------------------------------------------- */

// utworzenie obiektu typu zegar
// zwraca adres utworzonego obiektu, lub 0
              tmr_id   svc_tmr_create( void );
static inline tmr_id       tmr_create( void ) { return (tmr_id) SVCall(svc_tmr_create); }

// zatrzymanie zegara 'tmr'
// wszystkie procesy oczekuj�ce zostaj� wybudzone
// zostaje do nich wys�any komunikat E_STOPPED
              void     svc_tmr_kill( tmr_id tmr );
static inline void         tmr_kill( tmr_id tmr ) { SVCall(svc_tmr_kill, tmr); }

// uruchomienie zegara 'tmr'
// o czasie 'time' wszystkie procesy oczekuj�ce zostaj� wybudzone
// i zostanie wykonana procedura 'proc' (je�li 'proc' != 0)
              void     svc_tmr_startUntil( tmr_id tmr, unsigned time, fun_id proc );
static inline void         tmr_startUntil( tmr_id tmr, unsigned time, fun_id proc ) { SVCall(svc_tmr_startUntil, tmr, time, proc); }

// uruchomienie zegara 'tmr'
// po czasie 'delay' wszystkie procesy oczekuj�ce zostaj� wybudzone
// i zostanie wykonana procedura 'proc' (je�li 'proc' != 0)
              void     svc_tmr_startFor( tmr_id tmr, unsigned delay, fun_id proc );
static inline void         tmr_startFor( tmr_id tmr, unsigned delay, fun_id proc ) { SVCall(svc_tmr_startFor, tmr, delay, proc); }

// uruchomienie zegara 'tmr' z okresem 'period'
// po czasie 'period' wszystkie procesy oczekuj�ce zostaj� wybudzone
// i zostanie wykonana procedura 'proc' (je�li 'proc' != 0)
              void     svc_tmr_startPeriodic( tmr_id tmr, unsigned period, fun_id proc );
static inline void         tmr_startPeriodic( tmr_id tmr, unsigned period, fun_id proc ) { SVCall(svc_tmr_startPeriodic, tmr, period, proc); }

// zawieszenie wykonywania aktualnego procesu do czas 'time'
// lub do wybudzenia przez obiekt 'tmr'
// zwraca E_SUCCESS, E_STOPPED lub E_TIMEOUT
              unsigned svc_tmr_waitUntil( tmr_id tmr, unsigned time );
static inline unsigned     tmr_waitUntil( tmr_id tmr, unsigned time ) { return SVCall(svc_tmr_waitUntil, tmr, time); }

// zawieszenie wykonywania aktualnego procesu na czas 'delay'
// lub do wybudzenia przez obiekt 'tmr'
// zwraca E_SUCCESS, E_STOPPED lub E_TIMEOUT
              unsigned svc_tmr_waitFor( tmr_id tmr, unsigned delay );
static inline unsigned     tmr_waitFor( tmr_id tmr, unsigned delay ) { return SVCall(svc_tmr_waitFor, tmr, delay); }

// zawieszenie wykonywania aktualnego procesu
// do czasu wybudzenia przez obiekt 'tmr'
// zwraca E_SUCCESS lub E_STOPPED
static inline unsigned     tmr_wait( tmr_id tmr ) { return tmr_waitFor(tmr, INFINITE); }

// czy zegar zako�czy� odliczanie?
// zwraca E_SUCCESS, E_TIMEOUT
static inline unsigned     tmr_take   ( tmr_id tmr ) { return tmr_waitFor(tmr, IMMEDIATE); }
static inline unsigned     tmr_takeISR( tmr_id tmr ) { return tmr_waitFor(tmr, IMMEDIATE); }

// zmiana procedury aktualnie uruchomionego zegara
// funkcja mo�e byc wywo�ana tylko i wy��cznie z wn�trza procedury zegara
static inline void         tmr_flipISR( fun_id proc ) { System.tmr->state = proc; }

// zmiana parametr�w czasowych aktualnie uruchomionego zegara
// funkcja mo�e byc wywo�ana tylko i wy��cznie z wn�trza procedury zegara
// (delay == 0) => zegar zostanie zatrzymany
static inline void         tmr_delayISR( unsigned delay ) { System.tmr->delay = delay; }

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

#include <string.h>

// definicja klasy zegara

class Timer : public tmr_t
{
public:

	 Timer( fun_id _state = 0 ): tmr_t(_TMR_INIT()) { state = _state; }

	~Timer( void ) { tmr_kill(this); }

	void startUntil   ( unsigned _time   )                {        tmr_startUntil   (this, _time,   this->state); }
	void startUntil   ( unsigned _time,   fun_id _state ) {        tmr_startUntil   (this, _time,        _state); }
	void startFor     ( unsigned _delay  )                {        tmr_startFor     (this, _delay,  this->state); }
	void startFor     ( unsigned _delay,  fun_id _state ) {        tmr_startFor     (this, _delay,       _state); }
	void startPeriodic( unsigned _period )                {        tmr_startPeriodic(this, _period, this->state); }
	void startPeriodic( unsigned _period, fun_id _state ) {        tmr_startPeriodic(this, _period,      _state); }

	unsigned waitUntil( unsigned _time  )                 { return tmr_waitUntil    (this, _time);                }
	unsigned waitFor  ( unsigned _delay )                 { return tmr_waitFor      (this, _delay);               }
	unsigned wait     ( void )                            { return tmr_wait         (this);                       }
	unsigned take     ( void )                            { return tmr_take         (this);                       }
	unsigned takeISR  ( void )                            { return tmr_takeISR      (this);                       }
};

#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

namespace ThisTimer
{
// poni�sze funkcje mog� by� wywo�ywana tylko z poziomu procedury obs�ugi zegara
	void flipISR      ( fun_id   _state )                 {        tmr_flipISR            (_state);               }
	void delayISR     ( unsigned _delay )                 {        tmr_delayISR           (_delay);               }
};

#endif

/* -------------------------------------------------------------------------- */
