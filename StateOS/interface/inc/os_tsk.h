/******************************************************************************

    @file    State Machine OS: os_tsk.h
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

// deklaracja procesu 'tsk'
// state: stan procesu (procedura wykonywana przez proces)
// size:  wielkoœæ przydzielonego stosu (w bajtach)
// procedura nie musi byæ typu 'noreturn'; dziêki implementacji wewnêtrznej
// pêtli systemowej, procedura wracaj¹ca (return) bêdzie wywo³ywana ponownie

#define OS_WRK( tsk, prio, state, size )                                            \
               char tsk##__stack[ASIZE(size)] __osalign;                             \
               tsk_t tsk##__tsk = _TSK_INIT(prio, state, &tsk##__stack[ASIZE(size)]); \
               tsk_id tsk = & tsk##__tsk

#define OS_TSK( tsk, prio, state ) \
        OS_WRK( tsk, prio, state, OS_STACK_SIZE )

#define OS_DEF( tsk, prio )                \
        static void tsk##__startup( void ); \
        OS_TSK( tsk, prio, tsk##__startup ); \
        static void tsk##__startup( void )

#define static_WRK( tsk, prio, state, size )                                        \
        static char tsk##__stack[ASIZE(size)] __osalign;                             \
        static tsk_t tsk##__tsk = _TSK_INIT(prio, state, &tsk##__stack[ASIZE(size)]); \
        static tsk_id tsk = & tsk##__tsk

#define static_TSK( tsk, prio, state ) \
        static_WRK( tsk, prio, state, OS_STACK_SIZE )

#define static_DEF( tsk, prio )                \
        static void tsk##__startup( void );     \
        static_TSK( tsk, prio, tsk##__startup ); \
        static void tsk##__startup( void )

/* -------------------------------------------------------------------------- */

// utworzenie przestrzeni roboczej dla procesu ze stosem 'size'
// i uruchomienie z priorytetem 'prio', od stanu pocz¹tkowego 'state'
// zwraca adres TCB (task control block) utworzonego procesu
              tsk_id   TSK_create( unsigned prio, fun_id state, unsigned size );
static inline tsk_id   tsk_create( unsigned prio, fun_id state, unsigned size ) { return (tsk_id) OSCall(TSK_create, prio, state, size); }

// utworzenie przestrzeni roboczej dla procesu ze stosem 'OS_STACK_SIZE'
// i uruchomienie z priorytetem 'prio', od stanu pocz¹tkowego 'state'
// zwraca adres TCB (task control block) utworzonego procesu
static inline tsk_id   tsk_new( unsigned prio, fun_id state ) { return tsk_create(prio, state, OS_STACK_SIZE); }

// reinicjacja i uruchomienie procesu 'tsk'
              void     TSK_start( tsk_id tsk );
static inline void     tsk_start( tsk_id tsk ) { OSCall(TSK_start, tsk); }

// zatrzymanie aktualnego procesu (usuniêcie z listy zadañ)
              void     TSK_stop( void );
static inline void     tsk_stop( void ) { OSCall(TSK_stop); }

// zatrzymanie procesu 'tsk'
              void     TSK_kill( tsk_id tsk );
static inline void     tsk_kill( tsk_id tsk ) { OSCall(TSK_kill, tsk); }

// przekazanie sterowania do schedulera (nastêpnego procesu)
static inline void     tsk_pass ( void ) { OSCall(core_ctx_switch); }
static inline void     tsk_yield( void ) { OSCall(core_ctx_switch); }

// przekazanie sterowania do schedulera (nastêpnego procesu)
// restart procesu ze zmian¹ stanu/procedury wykonywanej przez proces
              void     tsk_flip( fun_id state ) __noreturn;

// ustawienie priorytetu aktualnego procesu z ewentualnym przekazaniem
// sterowania do innego procesu (o wy¿szym priorytecie)
              void     TSK_prio( unsigned prio );
static inline void     tsk_prio( unsigned prio ) { OSCall(TSK_prio, prio); }

// zawieszenie wykonywania aktualnego procesu do czasu 'time'
              unsigned TSK_sleepUntil( unsigned time );
static inline unsigned tsk_sleepUntil( unsigned time ) { return OSCall(TSK_sleepUntil, time); }

// zawieszenie wykonywania aktualnego procesu na czas 'delay'
              unsigned TSK_sleepFor( unsigned delay );
static inline unsigned tsk_sleepFor( unsigned delay ) { return OSCall(TSK_sleepFor, delay); }

// zawieszenie wykonywania aktualnego procesu
static inline unsigned tsk_sleep( void ) { return tsk_sleepFor(INFINITE); }

// zawieszenie wykonywania aktualnego procesu na czas 'delay'
// (delay != 0) => przekazanie sterowania do schedulera
static inline unsigned tsk_delay( unsigned delay ) { return tsk_sleepFor(delay); }

// zawieszenie wykonywania aktualnego procesu na czas nieokreœlony
// i przekazanie sterowania do schedulera
static inline unsigned tsk_suspend( void ) { return tsk_sleep(); }

// wznowienie wykonywania zawieszonego procesu 'tsk' z komunikatem 'event'
              void     TSK_resume   ( tsk_id tsk, unsigned event );
static inline void     tsk_resume   ( tsk_id tsk, unsigned event ) { OSCall(TSK_resume, tsk, event); }
static inline void     tsk_resumeISR( tsk_id tsk, unsigned event ) { tsk_resume(tsk, event); }


/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

#include <string.h>

// definicja klasy procesu
// state: stan procesu (procedura wykonywana przez proces)
// size:  wielkoœæ przydzielonego stosu (w bajtach)
// procedura nie musi byæ typu 'noreturn'; dziêki implementacji wewnêtrznej
// pêtli systemowej, procedura wracaj¹ca (return) bêdzie wywo³ywana ponownie

class TaskBase : public tsk_t
{
public:

	 TaskBase( unsigned _prio, fun_id _state, os_id _top ): tsk_t(_TSK_INIT(_prio, _state, _top)) {}

	~TaskBase( void ) { tsk_kill(this); }

	void     start     ( void )                          {        tsk_start     (this);                }
	void     start     ( fun_id   _state )               {        this->state = _state;
	                                                              tsk_start     (this);                }
	void     resume    ( unsigned _event )               {        tsk_resume    (this, _event);        }
	void     resumeISR ( unsigned _event )               {        tsk_resumeISR (this, _event);        }
};

#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

namespace ThisTask
{
	void     pass      ( void )                          {        tsk_pass      ();                    }
	void     yield     ( void )                          {        tsk_yield     ();                    }
	void     flip      ( fun_id   _state )               {        tsk_flip      (_state);              }
	void     stop      ( void )                          {        tsk_stop      ();                    }
	void     prio      ( unsigned _prio )                {        tsk_prio      (_prio);               }

	unsigned sleepUntil( unsigned _time )                { return tsk_sleepUntil(_time);               }
	unsigned sleepFor  ( unsigned _delay )               { return tsk_sleepFor  (_delay);              }
	unsigned sleep     ( void )                          { return tsk_sleep     ();                    }
	unsigned delay     ( unsigned _delay )               { return tsk_delay     (_delay);              }
	unsigned suspend   ( void )                          { return tsk_suspend   ();                    }
};

#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

template<unsigned _size>
class TaskT : public TaskBase
{
	char _stack[ASIZE(_size)] __osalign;

public:

	TaskT( unsigned _prio, fun_id _state ): TaskBase(_prio, _state, _stack + sizeof(_stack)) {}
};

#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

typedef TaskT<OS_STACK_SIZE> Task;

#endif

/* -------------------------------------------------------------------------- */
