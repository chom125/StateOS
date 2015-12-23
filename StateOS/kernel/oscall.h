/******************************************************************************

    @file    State Machine OS: oscall.h
    @author  Rajmund Szymanski
    @date    23.12.2015
    @brief   This file contains macro definitions for the Cortex-M devices.

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

#ifdef __cplusplus
extern "C" {
#endif

/* Some useful macros ------------------------------------------------------- */

#define __SVC_0(f)         SVC_0((unsigned)(f))
#define __SVC_1(f,a)       SVC_1((unsigned)(f),(unsigned)(a))
#define __SVC_2(f,a,b)     SVC_2((unsigned)(f),(unsigned)(a),(unsigned)(b))
#define __SVC_3(f,a,b,c)   SVC_3((unsigned)(f),(unsigned)(a),(unsigned)(b),(unsigned)(c))
#define __SVC_4(f,a,b,c,d) SVC_4((unsigned)(f),(unsigned)(a),(unsigned)(b),(unsigned)(c),(unsigned)(d))

/* -------------------------------------------------------------------------- */

#define VA_GLUE(a,...)                 a##__VA_ARGS__
#define VA_SVCN(...)       VA_GLUE(__SVC_,__VA_ARGS__)
#define VA_ARGN(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,n,...)  n
#define VA_ARGC(...)       VA_ARGN(0,##__VA_ARGS__,9,8,7,6,5,4,3,2,1,0)

/* -------------------------------------------------------------------------- */

#define  OSCall(f,...)     VA_SVCN(VA_ARGC(__VA_ARGS__))(f,##__VA_ARGS__)

/* -------------------------------------------------------------------------- */

#ifdef  __GNUC__

static inline __attribute__((always_inline))
unsigned SVC_0( unsigned f )
{
	register unsigned R0 __asm("r0");
	register unsigned IP __asm("ip") = f;
	__asm volatile ( "svc %[n]" : "=r"(R0) : "r"(IP),[n]"n"(0) : "memory" );
	return R0;
}

static inline __attribute__((always_inline))
unsigned SVC_1( unsigned f, unsigned a )
{
	register unsigned R0 __asm("r0") = a;
	register unsigned IP __asm("ip") = f;
	__asm volatile ( "svc %[n]" : "+r"(R0) : "r"(IP),[n]"n"(0) : "memory" );
	return R0;
}

static inline __attribute__((always_inline))
unsigned SVC_2( unsigned f, unsigned a, unsigned b )
{
	register unsigned R0 __asm("r0") = a;
	register unsigned R1 __asm("r1") = b;
	register unsigned IP __asm("ip") = f;
	__asm volatile ( "svc %[n]" : "+r"(R0) : "r"(R1),"r"(IP),[n]"n"(0) : "memory" );
	return R0;
}

static inline __attribute__((always_inline))
unsigned SVC_3( unsigned f, unsigned a, unsigned b, unsigned c )
{
	register unsigned R0 __asm("r0") = a;
	register unsigned R1 __asm("r1") = b;
	register unsigned R2 __asm("r2") = c;
	register unsigned IP __asm("ip") = f;
	__asm volatile ( "svc %[n]" : "+r"(R0) : "r"(R1),"r"(R2),"r"(IP),[n]"n"(0) : "memory" );
	return R0;
}

static inline __attribute__((always_inline))
unsigned SVC_4( unsigned f, unsigned a, unsigned b, unsigned c, unsigned d )
{
	register unsigned R0 __asm("r0") = a;
	register unsigned R1 __asm("r1") = b;
	register unsigned R2 __asm("r2") = c;
	register unsigned R3 __asm("r3") = d;
	register unsigned IP __asm("ip") = f;
	__asm volatile ( "svc %[n]" : "+r"(R0) : "r"(R1),"r"(R2),"r"(R3),"r"(IP),[n]"n"(0) : "memory" );
	return R0;
}

#endif//__GNUC__

/* -------------------------------------------------------------------------- */

#ifdef  __CC_ARM

__svc_indirect(0) unsigned SVC_0(unsigned f);
__svc_indirect(0) unsigned SVC_1(unsigned f, unsigned a);
__svc_indirect(0) unsigned SVC_2(unsigned f, unsigned a, unsigned b);
__svc_indirect(0) unsigned SVC_3(unsigned f, unsigned a, unsigned b, unsigned c);
__svc_indirect(0) unsigned SVC_4(unsigned f, unsigned a, unsigned b, unsigned c, unsigned d);

#endif//__CC_ARM

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif
