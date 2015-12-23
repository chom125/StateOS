/******************************************************************************

    @file    State Machine OS: oscall.c
    @author  Rajmund Szymanski
    @date    22.12.2015
    @brief   This file defines SVC_Handler for STM32 uC.

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

#ifdef __GNUC__

__attribute__((naked))
void SVC_Handler( void )
{
	__asm volatile
	(
"	push { r4, lr } \n"
"	mrs    r4, PSP  \n"
"	blx    ip       \n"
"	str    r0,[r4]  \n"
"	pop  { r4, pc } \n"
	);
}

#endif
