/**********************************************************************
    Nexcom Co., Ltd.
    Filename         : main.h
    Description      : 
    Last Updated     : 2026. 01. 30.
**********************************************************************/

#ifndef MAIN_H
#define MAIN_H

/* ************************** [[   include  ]]  *********************************************************** */
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

#include "f28x_project.h"			// TI 제공 칩-지원 헤더 통합 Include 용 헤더파일 (bit-field)
#include "driverlib.h"				// TI 제공 Driver API Library 헤더파일 (driverlib)
#include "device.h"

#include "DevCommon.h"
#include "DevDspInit.h"
#include "DevSci.h"
#include "DevSpi.h"
#include "DevTimer.h"
#include "easy28x_bitfield_v11.h"

#include "CSU_IPC.h"
#include "CSU_LED.h"
#include "CSU_SSI.h"
#include "CSU_Zero.h"


/* ************************** [[   define   ]]  *********************************************************** */
typedef unsigned char  Uint8;



/* ************************** [[   enum or struct   ]]  *************************************************** */



/* ************************** [[   global   ]]  *********************************************************** */



/* ************************** [[  function  ]]  *********************************************************** */
// DSP program entry point
void main(void);


#endif	// #ifndef MAIN_H

