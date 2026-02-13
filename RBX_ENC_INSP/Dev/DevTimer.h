/**********************************************************************

	Nexcom Co., Ltd.
	Copyright 2021. All Rights Reserved.

	Filename		: DevTimer.h
	Version			: 00.10
	Description		: CPU 타이머 설정 및 인터럽트 서비스 루틴 헤더
	Tracebility		: 
	Programmer	    :
	Last Updated	: 2026. 02. 11.

**********************************************************************/

/*
 * Modification History
 * --------------------
 * 
 * 
*/


#ifndef DEVTIMER_H
#define DEVTIMER_H

/* ************************** [[   include  ]]  *********************************************************** */
#include "main.h"


/* ************************** [[   define   ]]  *********************************************************** */


/* ************************** [[   enum or struct   ]]  *************************************************** */
typedef struct
{
	Uint16 Cycle_1ms;
	Uint16 Cycle_10ms;
	Uint16 Cycle_100ms;
	Uint16 Cycle_1000ms;

	Uint16 Hzcnt;
	Uint16 Hz;
} stTimer;


/* ************************** [[   global   ]]  *********************************************************** */
extern stTimer xTimer;


/* ************************** [[  function  ]]  *********************************************************** */
// DSP 타이머 초기화 
void Initial_TIMER(void);

__interrupt void isr_CpuTimer0(void);

__interrupt void isr_CpuTimer1(void);

__interrupt void isr_CpuTimer2(void);

#endif	// #ifndef DEVTIMER_H
