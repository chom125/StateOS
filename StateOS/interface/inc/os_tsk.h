/******************************************************************************

    @file    StateOS: os_tsk.h
    @author  Rajmund Szymanski
    @date    11.04.2017
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

#ifndef __STATEOS_TSK_H
#define __STATEOS_TSK_H

#include <oskernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : task (thread)                                                                                  *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifdef __CC_ARM
#pragma push
#pragma anon_unions
#endif

struct __tsk
{
	obj_t    obj;   // object header

	fun_t  * state; // callback procedure
	unsigned start;
	unsigned delay;
	tsk_t  * back;  // previous process in the DELAYED queue

	void   * sp;    // stack pointer
	void   * top;   // top of stack

	unsigned basic; // basic priority
	unsigned prio;  // current priority

	void   * guard; // object that controls the pending process
	mtx_t  * list;  // list of mutexes held
	tsk_t  * join;  // list of joined tasks

	union  {
	unsigned mode;  // used by flag object
	void   * data;  // used by mailbox queue object
	unsigned msg;   // used by message queue object
	};
	union  {
	unsigned flags; // used by flag object: all flags to wait
	unsigned event; // wakeup event
	};
#if defined(__ARMCC_VERSION) && !defined(__MICROLIB)
	char     libspace[96];
#endif
};

#ifdef __CC_ARM
#pragma pop
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : _TSK_INIT                                                                                      *
 *                                                                                                                    *
 * Description       : create and initilize a task object                                                             *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *   top             : top of task's private stack storage                                                            *
 *                                                                                                                    *
 * Return            : task object                                                                                    *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#if defined(__ARMCC_VERSION) && !defined(__MICROLIB)
#define               _TSK_INIT( _prio, _state, _top ) { { 0, 0, 0, 0 }, _state, 0, 0, 0, 0, _top, _prio, _prio, 0, 0, 0, { 0 }, { 0 }, { 0 } }
#else
#define               _TSK_INIT( _prio, _state, _top ) { { 0, 0, 0, 0 }, _state, 0, 0, 0, 0, _top, _prio, _prio, 0, 0, 0, { 0 }, { 0 } }
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : _TSK_CREATE                                                                                    *
 *                                                                                                                    *
 * Description       : create and initilize a task object                                                             *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *   top             : top of task's private stack storage                                                            *
 *                                                                                                                    *
 * Return            : pointer to task object                                                                         *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define               _TSK_CREATE( _prio, _state, _top ) & (tsk_t) _TSK_INIT( _prio, _state, _top )
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : _TSK_STACK                                                                                     *
 *                                                                                                                    *
 * Description       : create task's private stack storage                                                            *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   size            : size of task's private stack storage (in bytes)                                                *
 *                                                                                                                    *
 * Return            : top of task's private stack storage                                                            *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define               _TSK_STACK( _size ) ( stk_t[ASIZE( _size )] ){ 0 } + ASIZE( _size )
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_WRK                                                                                         *
 *                                                                                                                    *
 * Description       : define and initilize complete work area for task object                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : name of a pointer to task object                                                               *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *   size            : size of task private stack (in bytes)                                                          *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_WRK( tsk, prio, state, size )                                          \
                       stk_t tsk##__stk[ASIZE( size )];                                        \
                       tsk_t tsk##__tsk = _TSK_INIT( prio, state, tsk##__stk + ASIZE( size ) ); \
                       tsk_id tsk = & tsk##__tsk

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_TSK                                                                                         *
 *                                                                                                                    *
 * Description       : define and initilize complete work area for task obj. with stack size defined by OS_STACK_SIZE *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : name of a pointer to task object                                                               *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_TSK( tsk, prio, state ) \
                    OS_WRK( tsk, prio, state, OS_STACK_SIZE )

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_WRK_DEF                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize complete work area for task object                                        *
 *                     task state (function body) must be defined immediately below                                   *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : name of a pointer to task object                                                               *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *   size            : size of task private stack (in bytes)                                                          *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_WRK_DEF( tsk, prio, size )        \
                       void tsk##__fun( void );           \
                    OS_WRK( tsk, prio, tsk##__fun, size ); \
                       void tsk##__fun( void )

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_TSK_DEF                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize complete work area for task obj. with stack size defined by OS_STACK_SIZE *
 *                     task state (function body) must be defined immediately below                                   *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : name of a pointer to task object                                                               *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_TSK_DEF( tsk, prio ) \
                    OS_WRK_DEF( tsk, prio, OS_STACK_SIZE )

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_WRK                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize static work area for task object                                          *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : name of a pointer to task object                                                               *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *   size            : size of task private stack (in bytes)                                                          *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_WRK( tsk, prio, state, size )                                          \
                static stk_t tsk##__stk[ASIZE( size )];                                        \
                static tsk_t tsk##__tsk = _TSK_INIT( prio, state, tsk##__stk + ASIZE( size ) ); \
                static tsk_id tsk = & tsk##__tsk

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_TSK                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize static work area for task object with stack size defined by OS_STACK_SIZE *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : name of a pointer to task object                                                               *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_TSK( tsk, prio, state ) \
                static_WRK( tsk, prio, state, OS_STACK_SIZE )

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_WRK_DEF                                                                                 *
 *                                                                                                                    *
 * Description       : define and initilize static work area for task object                                          *
 *                     task state (function body) must be defined immediately below                                   *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : name of a pointer to task object                                                               *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *   size            : size of task private stack (in bytes)                                                          *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_WRK_DEF( tsk, prio, size )        \
                static void tsk##__fun( void );           \
                static_WRK( tsk, prio, tsk##__fun, size ); \
                static void tsk##__fun( void )

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_TSK_DEF                                                                                 *
 *                                                                                                                    *
 * Description       : define and initilize static work area for task object with stack size defined by OS_STACK_SIZE *
 *                     task state (function body) must be defined immediately below                                   *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : name of a pointer to task object                                                               *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_TSK_DEF( tsk, prio ) \
                static_WRK_DEF( tsk, prio, OS_STACK_SIZE )

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : WRK_INIT                                                                                       *
 *                                                                                                                    *
 * Description       : create and initilize complete work area for task object                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *   size            : size of task private stack (in bytes)                                                          *
 *                                                                                                                    *
 * Return            : task object                                                                                    *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                WRK_INIT( prio, state, size ) \
                      _TSK_INIT( prio, state, _TSK_STACK( size ) )
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : WRK_CREATE                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize complete work area for task object                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *   size            : size of task private stack (in bytes)                                                          *
 *                                                                                                                    *
 * Return            : pointer to task object                                                                         *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                WRK_CREATE( prio, state, size ) \
             & (tsk_t) WRK_INIT( prio, state, size )
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : TSK_INIT                                                                                       *
 *                                                                                                                    *
 * Description       : create and initilize complete work area for task obj. with stack size defined by OS_STACK_SIZE *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *                                                                                                                    *
 * Return            : task object                                                                                    *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                TSK_INIT( prio, state ) \
                       WRK_INIT( prio, state, OS_STACK_SIZE )
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : TSK_CREATE                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize complete work area for task obj. with stack size defined by OS_STACK_SIZE *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *                                                                                                                    *
 * Return            : pointer to task object                                                                         *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                TSK_CREATE( prio, state ) \
                       WRK_CREATE( prio, state, OS_STACK_SIZE )
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_init                                                                                       *
 *                                                                                                                    *
 * Description       : initilize complete work area for task object and start the task                                *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : pointer to task object                                                                         *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *   stack           : top of task's private stack storage                                                            *
 *                                                                                                                    *
 * Return            : task object                                                                                    *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

void tsk_init( tsk_t *tsk, unsigned prio, fun_t *state, void *stack );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_create                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize complete work area for task object and start the task                     *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *   size            : size of task private stack (in bytes)                                                          *
 *                                                                                                                    *
 * Return            : pointer to task object (task successfully created)                                             *
 *   0               : task not created (not enough free memory)                                                      *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

tsk_t *tsk_create( unsigned prio, fun_t *state, unsigned size );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_new                                                                                        *
 *                                                                                                                    *
 * Description       : create and initilize complete work area for task obj. with stack size defined by OS_STACK_SIZE *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *                                                                                                                    *
 * Return            : pointer to task object (task successfully created)                                             *
 *   0               : task not created (not enough free memory)                                                      *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
tsk_t *tsk_new( unsigned prio, fun_t *state ) { return tsk_create(prio, state, OS_STACK_SIZE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_start                                                                                      *
 *                                                                                                                    *
 * Description       : start previously defined/created/stopped task object                                           *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : pointer to task object                                                                         *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

void tsk_start( tsk_t *tsk );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_startFrom                                                                                  *
 *                                                                                                                    *
 * Description       : start previously defined/created/stopped task object                                           *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : pointer to task object                                                                         *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

void tsk_startFrom( tsk_t *tsk, fun_t *state );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_stop                                                                                       *
 *                                                                                                                    *
 * Description       : stop current task and remove it from READY queue                                               *
 *                     function doesn't destroy the stack storage                                                     *
 *                     all allocated resources remain intact until restarting the task                                *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

__NO_RETURN
void tsk_stop( void );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_kill                                                                                       *
 *                                                                                                                    *
 * Description       : reset the task object and remove it from READY/DELAYED queue                                   *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : pointer to task object                                                                         *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

void tsk_kill( tsk_t *tsk );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_detach                                                                                     *
 *                                                                                                                    *
 * Description       : detach given task                                                                              *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : pointer to task object                                                                         *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

void tsk_detach( tsk_t *tsk );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_join                                                                                       *
 *                                                                                                                    *
 * Description       : delay execution of current task until termination of given task                                *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : pointer to task object                                                                         *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : joined task was stopped its execution                                                          *
 *   E_STOPPED       : joined task was killed                                                                         *
 *   E_TIMEOUT       : joined task was detached                                                                       *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned tsk_join( tsk_t *tsk );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_yield                                                                                      *
 *                                                                                                                    *
 * Description       : yield system control to the next task with the same priority in READY queue                    *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

void tsk_yield( void );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_pass                                                                                       *
 *                                                                                                                    *
 * Description       : the same as tsk_yield (force context switch)                                                   *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
void tsk_pass( void ) { tsk_yield(); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_flip                                                                                       *
 *                                                                                                                    *
 * Description       : restart current task with the new state (task function)                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   proc            : new task state (task function)                                                                 *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

__NO_RETURN
void tsk_flip( fun_t *state );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_prio                                                                                       *
 *                                                                                                                    *
 * Description       : set current task priority                                                                      *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   prio            : new task priority value                                                                        *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

void tsk_prio( unsigned prio );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_setPrio                                                                                    *
 *                                                                                                                    *
 * Description       : the same as tsk_prio (set current task priority)                                               *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   prio            : new task priority value                                                                        *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
void tsk_setPrio( unsigned prio ) { tsk_prio(prio); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_getPrio                                                                                    *
 *                                                                                                                    *
 * Description       : get current task priority                                                                      *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : current task priority value                                                                    *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
unsigned tsk_getPrio( void ) { return Current->basic; }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_waitUntil                                                                                  *
 *                                                                                                                    *
 * Description       : delay execution of current task until given timepoint and wait for flags or message            *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   flags           : all flags to wait                                                                              *
 *                     0: wait for any flags or message                                                               *
 *   time            : timepoint value                                                                                *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_TIMEOUT       : task object was not released before the specified timeout expired                              *
 *   'another'       : task object resumed by the direct transfer of 'another' flags or message (tsk_give)            *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned tsk_waitUntil( unsigned flags, unsigned time );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_waitFor                                                                                    *
 *                                                                                                                    *
 * Description       : delay execution of current task for given duration of time and wait for flags or message       *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   flags           : all flags to wait                                                                              *
 *                     0: wait for any flag or message                                                                *
 *   delay           : duration of time (maximum number of ticks to delay execution of current task)                  *
 *                     IMMEDIATE: don't delay execution of current task                                               *
 *                     INFINITE:  delay indefinitly execution of current task                                         *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_TIMEOUT       : task object was not released before the specified timeout expired                              *
 *   'another'       : task object resumed by the direct transfer of 'another' flags or message (tsk_give)            *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned tsk_waitFor( unsigned flags, unsigned delay );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_wait                                                                                       *
 *                                                                                                                    *
 * Description       : delay indefinitly execution of current task and wait for flags or message                      *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   flags           : all flags to wait                                                                              *
 *                     0: wait for any flag or message                                                                *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   'another'       : task object resumed by the direct transfer of 'another' flags or message (tsk_give)            *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
unsigned tsk_wait( unsigned flags ) { return tsk_waitFor(flags, INFINITE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_give                                                                                       *
 *                                                                                                                    *
 * Description       : set given flags in waiting task (tsk_wait)                                                     *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : pointer to delayed task object                                                                 *
 *   flags           : flags or message transfered to the task                                                        *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

void tsk_give( tsk_t *tsk, unsigned flags );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_giveISR                                                                                    *
 *                                                                                                                    *
 * Description       : resume execution of given waiting task (tsk_wait)                                              *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : pointer to delayed task object                                                                 *
 *   flags           : flags or message transfered to the task                                                        *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
void tsk_giveISR( tsk_t *tsk, unsigned flags ) { tsk_give(tsk, flags); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_sleepUntil                                                                                 *
 *                                                                                                                    *
 * Description       : delay execution of current task until given timepoint                                          *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   time            : timepoint value                                                                                *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_TIMEOUT       : task object successfully finished countdown                                                    *
 *   E_STOPPED       : task object was resumed (tsk_resume)                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
unsigned tsk_sleepUntil( unsigned time ) { return tmr_waitUntil(&WAIT, time); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_sleepFor                                                                                   *
 *                                                                                                                    *
 * Description       : delay execution of current task for given duration of time                                     *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   delay           : duration of time (maximum number of ticks to delay execution of current task)                  *
 *                     IMMEDIATE: don't delay execution of current task                                               *
 *                     INFINITE:  delay indefinitly execution of current task                                         *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_TIMEOUT       : task object successfully finished countdown                                                    *
 *   E_STOPPED       : task object was resumed (tsk_resume)                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
unsigned tsk_sleepFor( unsigned delay ) { return tmr_waitFor(&WAIT, delay); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_sleep                                                                                      *
 *                                                                                                                    *
 * Description       : delay indefinitly execution of current task                                                    *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_STOPPED       : task object was resumed (tsk_resume)                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
unsigned tsk_sleep( void ) { return tmr_wait(&WAIT); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_delay                                                                                      *
 *                                                                                                                    *
 * Description       : the same as tsk_sleepFor, delay execution of current task for given duration of time           *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   delay           : duration of time (maximum number of ticks to delay execution of current task)                  *
 *                     IMMEDIATE: don't delay execution of current task                                               *
 *                     INFINITE:  delay indefinitly execution of current task                                         *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_TIMEOUT       : task object successfully finished countdown                                                    *
 *   E_STOPPED       : task object was resumed (tsk_resume)                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
unsigned tsk_delay( unsigned delay ) { return tsk_sleepFor(delay); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_suspend                                                                                    *
 *                                                                                                                    *
 * Description       : delay indefinitly execution of given task                                                      *
 *                     tasks from delayed queue can not be suspended                                                  *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : task was successfully suspended                                                                *
 *   E_STOPPED       : task can not be suspended                                                                      *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned tsk_suspend( tsk_t *tsk );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_resume                                                                                     *
 *                                                                                                                    *
 * Description       : resume execution of given delayed task                                                         *
 *                     only suspended and sleeping tasks can be resumed                                               *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : pointer to delayed task object                                                                 *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : task was successfully resumed                                                                  *
 *   E_STOPPED       : task can not be resumed                                                                        *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned tsk_resume( tsk_t *tsk );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tsk_resumeISR                                                                                  *
 *                                                                                                                    *
 * Description       : resume execution of given delayed task                                                         *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tsk             : pointer to delayed task object                                                                 *
 *   event           : the value at which the given task is woken up                                                  *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : task was successfully resumed                                                                  *
 *   E_STOPPED       : task can not be resumed                                                                        *
 *                                                                                                                    *
 * Note              : use only in handler mode                                                                       *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
unsigned tsk_resumeISR( tsk_t *tsk ) { return tsk_resume(tsk); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/**********************************************************************************************************************
 *                                                                                                                    *
 * Namespace         : ThisTask                                                                                       *
 *                                                                                                                    *
 * Description       : provide set of functions for Current Task                                                      *
 *                                                                                                                    *
 **********************************************************************************************************************/

namespace ThisTask
{
	void     pass      ( void )                             {        tsk_pass      ();                    }
	void     yield     ( void )                             {        tsk_yield     ();                    }
	void     flip      ( fun_t  * _state )                  {        tsk_flip      (_state);              }
	void     stop      ( void )                             {        tsk_stop      ();                    }
	void     prio      ( unsigned _prio )                   {        tsk_prio      (_prio);               }
	void     setPrio   ( unsigned _prio )                   {        tsk_setPrio   (_prio);               }
	unsigned getPrio   ( void )                             { return tsk_getPrio   ();                    }
	unsigned prio      ( void )                             { return tsk_getPrio   ();                    }

	void     kill      ( void )                             {        tsk_kill      (Current);             }
	void     detach    ( void )                             {        tsk_detach    (Current);             }
	void     suspend   ( void )                             {        tsk_suspend   (Current);             }

	unsigned waitUntil ( unsigned _flags, unsigned _time )  { return tsk_waitUntil (_flags, _time);       }
	unsigned waitFor   ( unsigned _flags, unsigned _delay ) { return tsk_waitFor   (_flags, _delay);      }
	unsigned wait      ( unsigned _flags )                  { return tsk_wait      (_flags);              }
	unsigned sleepUntil( unsigned _time )                   { return tsk_sleepUntil(_time);               }
	unsigned sleepFor  ( unsigned _delay )                  { return tsk_sleepFor  (_delay);              }
	unsigned sleep     ( void )                             { return tsk_sleep     ();                    }
	unsigned delay     ( unsigned _delay )                  { return tsk_delay     (_delay);              }
}

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : TaskT<>                                                                                        *
 *                                                                                                                    *
 * Description       : create and initilize complete work area for task object                                        *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *   size            : size of task private stack (in bytes)                                                          *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *                                                                                                                    *
 **********************************************************************************************************************/

template<unsigned _size>
struct TaskT : public __tsk
{
	explicit
	 TaskT( const unsigned _prio, fun_t *_state ): __tsk _TSK_INIT(_prio, _state, _stack+ASIZE(_size)) {}
	~TaskT( void ) { assert(obj.id == ID_STOPPED); }

	void     kill      ( void )                             {        tsk_kill      (this);                }
	void     detach    ( void )                             {        tsk_detach    (this);                }
	unsigned join      ( void )                             { return tsk_join      (this);                }
	void     start     ( void )                             {        tsk_start     (this);                }
	void     startFrom ( fun_t  * _state )                  {        tsk_startFrom (this, _state);        }
	void     give      ( unsigned _flags )                  {        tsk_give      (this, _flags);        }
	void     giveISR   ( unsigned _flags )                  {        tsk_giveISR   (this, _flags);        }
	unsigned suspend   ( void )                             { return tsk_suspend   (this);                }
	unsigned resume    ( void )                             { return tsk_resume    (this);                }
	unsigned resumeISR ( void )                             { return tsk_resumeISR (this);                }

	unsigned prio      ( void )                             { return __tsk::basic;                        }
	unsigned getPrio   ( void )                             { return __tsk::basic;                        }
	bool     operator! ( void )                             { return __tsk::obj.id == ID_STOPPED;         }

	private:
	stk_t _stack[ASIZE(_size)];
};

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : Task                                                                                           *
 *                                                                                                                    *
 * Description       : create and initilize complete work area for task object with default stack size                *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *                                                                                                                    *
 **********************************************************************************************************************/

struct Task: public TaskT<OS_STACK_SIZE>
{
	explicit
	Task( const unsigned _prio, fun_t *_state ): TaskT<OS_STACK_SIZE>(_prio, _state) {}
};

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : startTaskT<>                                                                                   *
 *                                                                                                                    *
 * Description       : create and initilize complete work area for task object                                        *
 *                     and start task object                                                                          *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *   size            : size of task private stack (in bytes)                                                          *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *                                                                                                                    *
 **********************************************************************************************************************/

template<unsigned _size>
struct startTaskT : public TaskT<_size>
{
	explicit
	startTaskT( const unsigned _prio, fun_t *_state ): TaskT<_size>(_prio, _state) { tsk_start(this); }
};

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : startTask                                                                                      *
 *                                                                                                                    *
 * Description       : create and initilize complete work area for task object with default stack size                *
 *                     and start task object                                                                          *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *   prio            : initial task priority (any unsigned int value)                                                 *
 *   state           : task state (initial task function) doesn't have to be noreturn-type                            *
 *                     it will be executed into an infinite system-implemented loop                                   *
 *                                                                                                                    *
 **********************************************************************************************************************/

struct startTask : public startTaskT<OS_STACK_SIZE>
{
	explicit
	startTask( const unsigned _prio, fun_t *_state ): startTaskT<OS_STACK_SIZE>(_prio, _state) {}
};

#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_TSK_H
