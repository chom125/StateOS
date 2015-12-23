/******************************************************************************

    @file    State Machine OS: os_box.h
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

// deklaracja skrzynki fifo 'box'
// limit: rozmiar skrzynki fifo (maksymalna ilo�� komunikat�w)
// size: wielko�� komunikatu

#define OS_BOX( box, limit, size )                                   \
               char box##__data[limit*size];                          \
               box_t box##__box = _BOX_INIT(limit, size, box##__data); \
               box_id box = & box##__box


#define static_BOX( box, limit, size )                               \
        static char box##__data[limit*size];                          \
        static box_t box##__box = _BOX_INIT(limit, size, box##__data); \
        static box_id box = & box##__box

/* -------------------------------------------------------------------------- */

// utworzenie skrzynki fifo o rozmiarze 'limit' i wielko�ci komunikatu 'size'
// zwraca adres utworzonego obiektu, lub 0
              box_id   BOX_create( unsigned limit, unsigned size );
static inline box_id   box_create( unsigned limit, unsigned size ) { return (box_id) OSCall(BOX_create, limit, size); }

// reset obiektu 'box'
// wszystkie procesy oczekuj�ce zostaj� wybudzone
// zostaje do nich wys�any komunikat E_STOPPED
              void     BOX_kill( box_id box );
static inline void     box_kill( box_id box ) { OSCall(BOX_kill, box); }

// zawieszenie wykonywania aktualnego procesu do czasu 'time'
// lub do odebrania porcji danych 'data' z obiektu 'box'
// zwraca E_SUCCESS, E_STOPPED lub E_TIMEOUT
              unsigned BOX_waitUntil( box_id box, void *data, unsigned time );
static inline unsigned box_waitUntil( box_id box, void *data, unsigned time ) { return OSCall(BOX_waitUntil, box, data, time); }

// zawieszenie wykonywania aktualnego procesu na czas 'delay'
// lub do odebrania porcji danych 'data' z obiektu 'box'
// zwraca E_SUCCESS, E_STOPPED lub E_TIMEOUT
              unsigned BOX_waitFor( box_id box, void *data, unsigned delay );
static inline unsigned box_waitFor( box_id box, void *data, unsigned delay ) { return OSCall(BOX_waitFor, box, data, delay); }

// zawieszenie wykonywania aktualnego procesu
// do czasu odebrania porcji danych 'data' z obiektu 'box'
// zwraca E_SUCCESS lub E_STOPPED
static inline unsigned box_wait( box_id box, void *data ) { return box_waitFor(box, data, INFINITE); }

// nie zawiesza wykonywania aktualnego procesu
// odebiera porcj� danych 'data' z obiektu 'box' (je�li dane s� dost�pne)
// zwraca E_SUCCESS lub E_TIMEOUT
static inline unsigned box_take   ( box_id box, void *data ) { return box_waitFor(box, data, IMMEDIATE); }
static inline unsigned box_takeISR( box_id box, void *data ) { return box_waitFor(box, data, IMMEDIATE); }

// zawieszenie wykonywania aktualnego procesu do czasu 'time'
// lub do wys�ania porcji danych 'data' do obiektu 'box'
// zwraca E_SUCCESS, E_STOPPED lub E_TIMEOUT
              unsigned BOX_sendUntil( box_id box, void *data, unsigned time );
static inline unsigned box_sendUntil( box_id box, void *data, unsigned time ) { return OSCall(BOX_sendUntil, box, data, time); }

// zawieszenie wykonywania aktualnego procesu na czas 'delay'
// lub do wys�ania porcji danych 'data' do obiektu 'box'
// zwraca E_SUCCESS, E_STOPPED lub E_TIMEOUT
              unsigned BOX_sendFor( box_id box, void *data, unsigned delay );
static inline unsigned box_sendFor( box_id box, void *data, unsigned delay ) { return OSCall(BOX_sendFor, box, data, delay); }

// zawieszenie wykonywania aktualnego procesu
// do czasu wys�ania porcji danych 'data' do obiektu 'box'
// zwraca E_SUCCESS lub E_STOPPED
static inline unsigned box_send( box_id box, void *data ) { return box_sendFor(box, data, INFINITE); }

// nie zawiesza wykonywania aktualnego procesu
// wysy�a porcj� danych 'data' do obiektu 'box' (je�li jest mo�liwe wys�anie)
// zwraca E_SUCCESS lub E_TIMEOUT
static inline unsigned box_give   ( box_id box, void *data ) { return box_sendFor(box, data, IMMEDIATE); }
static inline unsigned box_giveISR( box_id box, void *data ) { return box_sendFor(box, data, IMMEDIATE); }

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

#include <string.h>

// definicja klasy kolejki fifo
// limit: rozmiar kolejki fifo (ilo�� porcji danych)

template<class T, unsigned _limit>
class MailBoxQueueT : public box_t
{
	T _data[_limit];

public:

	 MailBoxQueueT( void ): box_t(_BOX_INIT(_limit, sizeof(T), _data)) {}

	~MailBoxQueueT( void ) { box_kill(this); }

	unsigned waitUntil( T *_data, unsigned _time  ) { return box_waitUntil(this, _data, _time);  }
	unsigned waitFor  ( T *_data, unsigned _delay ) { return box_waitFor  (this, _data, _delay); }
	unsigned wait     ( T *_data )                  { return box_wait     (this, _data);         }
	unsigned take     ( T *_data )                  { return box_take     (this, _data);         }
	unsigned takeISR  ( T *_data )                  { return box_takeISR  (this, _data);         }
	unsigned sendUntil( T *_data, unsigned _time  ) { return box_sendUntil(this, _data, _time);  }
	unsigned sendFor  ( T *_data, unsigned _delay ) { return box_sendFor  (this, _data, _delay); }
	unsigned send     ( T *_data )                  { return box_send     (this, _data);         }
	unsigned give     ( T *_data )                  { return box_give     (this, _data);         }
	unsigned giveISR  ( T *_data )                  { return box_giveISR  (this, _data);         }
};

#endif

/* -------------------------------------------------------------------------- */
