/******************************************************************************

    @file    State Machine OS: os_sem.h
    @author  Rajmund Szymanski
    @date    18.12.2015
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

#define semBinary   (  1U )
#define semNormal   ( ~0U )
#define semCounting ( ~0U )

/* -------------------------------------------------------------------------- */

// deklaracja semafora 'sem'
// limit: maksymalna ilo�� dost�pnych �eton�w

#define OS_SEM( sem, limit )                          \
               sem_t sem##__sem = _SEM_INIT(0, limit); \
               sem_id sem = & sem##__sem

#define static_SEM( sem, limit )                      \
        static sem_t sem##__sem = _SEM_INIT(0, limit); \
        static sem_id sem = & sem##__sem

/* -------------------------------------------------------------------------- */

// utworzenie obiektu typu semafor (sem)
// z maksymaln� liczb� �eton�w 'limit'
// limit: semNormal, semBinary, other unsigned
// zwraca adres utworzonego obiektu, lub 0
              sem_id   svc_sem_create( unsigned limit );
static inline sem_id       sem_create( unsigned limit ) { return (sem_id) SVCall(svc_sem_create, limit); }

// reset obiektu 'sem'
// wszystkie procesy oczekuj�ce zostaj� wybudzone
// zostaje do nich wys�any komunikat E_STOPPED
              void     svc_sem_kill( sem_id sem );
static inline void         sem_kill( sem_id sem ) { SVCall(svc_sem_kill, sem); }

// zawieszenie wykonywania aktualnego procesu do czasu 'time'
// lub do wybudzenia przez obiekt 'sem'
// je�li liczba �eton�w jest wi�ksza od zera, to nast�puje zaj�cie obiektu 'sem'
// zwraca E_SUCCESS, E_STOPPED lub E_TIMEOUT
              unsigned svc_sem_waitUntil( sem_id sem, unsigned time );
static inline unsigned     sem_waitUntil( sem_id sem, unsigned time ) { return SVCall(svc_sem_waitUntil, sem, time); }

// zawieszenie wykonywania aktualnego procesu na czas 'delay'
// lub do wybudzenia przez obiekt 'sem'
// je�li liczba �eton�w jest wi�ksza od zera, to nast�puje zaj�cie obiektu 'sem'
// zwraca E_SUCCESS, E_STOPPED lub E_TIMEOUT
              unsigned svc_sem_waitFor( sem_id sem, unsigned delay );
static inline unsigned     sem_waitFor( sem_id sem, unsigned delay ) { return SVCall(svc_sem_waitFor, sem, delay); }

// zawieszenie wykonywania aktualnego procesu
// do czasu wybudzenia przez obiekt 'sem'
// je�li liczba �eton�w jest wi�ksza od zera, to nast�puje zaj�cie obiektu 'sem'
// zwraca E_SUCCESS lub E_STOPPED
static inline unsigned     sem_wait( sem_id sem ) { return sem_waitFor(sem, INFINITE); }

// nie zawiesza aktualnie wykonywanego procesu
// je�li liczba �eton�w jest wi�ksza od zera, to nast�puje zaj�cie obiektu 'sem'
// zwraca E_SUCCESS lub E_TIMEOUT
static inline unsigned     sem_take   ( sem_id sem ) { return sem_waitFor(sem, IMMEDIATE); }
static inline unsigned     sem_takeISR( sem_id sem ) { return sem_waitFor(sem, IMMEDIATE); }

// zawieszenie wykonywania aktualnego procesu do czasu 'time'
// lub do wybudzenia przez obiekt 'sem'
// je�li liczba �eton�w jest mniejsza od maksymalnej, to nast�puje zwolnienie obiektu 'sem'
// zwraca E_SUCCESS, E_STOPPED lub E_TIMEOUT
              unsigned svc_sem_sendUntil( sem_id sem, unsigned time );
static inline unsigned     sem_sendUntil( sem_id sem, unsigned time ) { return SVCall(svc_sem_sendUntil, sem, time); }

// zawieszenie wykonywania aktualnego procesu na czas 'delay'
// lub do wybudzenia przez obiekt 'sem'
// je�li liczba �eton�w jest mniejsza od maksymalnej, to nast�puje zwolnienie obiektu 'sem'
// zwraca E_SUCCESS, E_STOPPED lub E_TIMEOUT
              unsigned svc_sem_sendFor( sem_id sem, unsigned delay );
static inline unsigned     sem_sendFor( sem_id sem, unsigned delay ) { return SVCall(svc_sem_sendFor, sem, delay); }

// zawieszenie wykonywania aktualnego procesu
// do wybudzenia przez obiekt 'sem'
// je�li liczba �eton�w jest mniejsza od maksymalnej, to nast�puje zwolnienie obiektu 'sem'
// zwraca E_SUCCESS lub E_STOPPED
static inline unsigned     sem_send( sem_id sem ) { return sem_sendFor(sem, INFINITE); }

// nie zawiesza aktualnie wykonywanego procesu
// je�li liczba �eton�w jest mniejsza od maksymalnej, to nast�puje zwolnienie obiektu 'sem'
// zwraca E_SUCCESS lub E_TIMEOUT
static inline unsigned     sem_give   ( sem_id sem ) { return sem_sendFor(sem, IMMEDIATE); }
static inline unsigned     sem_giveISR( sem_id sem ) { return sem_sendFor(sem, IMMEDIATE); }

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

#include <string.h>

// definicja klasy semafora
// limit: maksymalna ilo�� dost�pnych �eton�w

class Semaphore : public sem_t
{
public:

	 Semaphore( unsigned _limit = semNormal ): sem_t(_SEM_INIT(0, _limit)) {}

	~Semaphore( void ) { sem_kill(this); }

	unsigned waitUntil( unsigned _time  ) { return sem_waitUntil(this, _time);  }
	unsigned waitFor  ( unsigned _delay ) { return sem_waitFor  (this, _delay); }
	unsigned wait     ( void )            { return sem_wait     (this);         }
	unsigned take     ( void )            { return sem_take     (this);         }
	unsigned takeISR  ( void )            { return sem_takeISR  (this);         }
	unsigned sendUntil( unsigned _time  ) { return sem_sendUntil(this, _time);  }
	unsigned sendFor  ( unsigned _delay ) { return sem_sendFor  (this, _delay); }
	unsigned send     ( void )            { return sem_send     (this);         }
	unsigned give     ( void )            { return sem_give     (this);         }
	unsigned giveISR  ( void )            { return sem_giveISR  (this);         }
};

#endif

/* -------------------------------------------------------------------------- */
