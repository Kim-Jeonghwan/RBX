/**********************************************************************

	Nexcom Co., Ltd.
	Copyright 2021. All Rights Reserved.

	Filename		: DevTimer.c
	Version			: 00.10
	Description		: 
	Tracebility		: 
	Programmer	    :
	Last Updated	: 2025. 08. 04.

**********************************************************************/

/*
 * Modification History
 * --------------------
 * 
 * 
*/


/* ************************** [[   include  ]]  *********************************************************** */
#include "DevTimer.h"


/* ************************** [[   define   ]]  *********************************************************** */


/* ************************** [[   global   ]]  *********************************************************** */
stTimer xTimer;


/* ************************** [[  static prototype  ]]  *************************************************** */
static void initCPUTimers(void);

static void configCPUTimer(Uint32 cpuTimer, float freq, float period);


/* ************************** [[  function  ]]  *********************************************************** */
/*
@funtion	void Initial_TIMER(void)
@brief		타이머 모듈 초기화
@param		void
@return		void
@remark	
	-	
*/
void Initial_TIMER(void)
{
	//	  
	// 각 CPU 타이머 인터럽트를 위한 ISR 등록
	//    
	Interrupt_register(INT_TIMER0, &isr_CpuTimer0);
    Interrupt_register(INT_TIMER1, &isr_CpuTimer1);
    Interrupt_register(INT_TIMER2, &isr_CpuTimer2);

	//
	// 장치 주변장치 초기화. 이 예제에서는 CPU 타이머만 초기화함.
	//
	initCPUTimers();

	//
	// CPU-Timer 0, 1, 2를 설정함:
	// 각각의 주기 설정 (마이크로초 단위)
	//
	configCPUTimer(CPUTIMER0_BASE, DEVICE_SYSCLK_FREQ, 100);	 	// 100 us
	configCPUTimer(CPUTIMER1_BASE, DEVICE_SYSCLK_FREQ, 1000);		// 1 ms
	configCPUTimer(CPUTIMER2_BASE, DEVICE_SYSCLK_FREQ, 1000000);	// 1000 ms


	//
	// CPU-Timer 0, 1, 2에 연결된 CPU 인터럽트 int1, int13, int14를 각각 활성화함.
	// PIE에서 TINT0 활성화: Group 1 인터럽트 7
	//
	Interrupt_enable(INT_TIMER0);
	Interrupt_enable(INT_TIMER1);
	Interrupt_enable(INT_TIMER2);

	//
	// CPU-Timer 0, 1, 2 시작
	//
	CPUTimer_startTimer(CPUTIMER0_BASE);
	CPUTimer_startTimer(CPUTIMER1_BASE);
	CPUTimer_startTimer(CPUTIMER2_BASE);


}

//
// initCPUTimers - 모든 3개의 CPU 타이머를 알려진 상태로 초기화함.
//
static void initCPUTimers(void)
{
    //
    // 타이머 주기를 최대로 초기화
    //
    CPUTimer_setPeriod(CPUTIMER0_BASE, 0xFFFFFFFF);
    CPUTimer_setPeriod(CPUTIMER1_BASE, 0xFFFFFFFF);
    CPUTimer_setPeriod(CPUTIMER2_BASE, 0xFFFFFFFF);

    //
    // 프리스케일 카운터를 1로 나누도록 초기화 (SYSCLKOUT)
    //
    CPUTimer_setPreScaler(CPUTIMER0_BASE, 0);
    CPUTimer_setPreScaler(CPUTIMER1_BASE, 0);
    CPUTimer_setPreScaler(CPUTIMER2_BASE, 0);

    //
    // 타이머가 중지되었는지 확인
    //
    CPUTimer_stopTimer(CPUTIMER0_BASE);
    CPUTimer_stopTimer(CPUTIMER1_BASE);
    CPUTimer_stopTimer(CPUTIMER2_BASE);

    //
    // 모든 카운터 레지스터에 주기 값을 다시 로드함
    //
    CPUTimer_reloadTimerCounter(CPUTIMER0_BASE);
    CPUTimer_reloadTimerCounter(CPUTIMER1_BASE);
    CPUTimer_reloadTimerCounter(CPUTIMER2_BASE);

    //
    // 인터럽트 카운터 초기화
    //
	(void)memset(&xTimer, 	0, sizeof(xTimer));		// 타이머 구조체 초기화
}



//
// configCPUTimer - 선택된 타이머를 지정된 "freq"와 "period" 매개변수에 따른 주기로 초기화함.
// "freq"는 Hz 단위로 입력되고 "period"는 마이크로초 단위임. 
// 설정 후 타이머는 중지 상태로 유지됨.
//
static void configCPUTimer(Uint32 cpuTimer, float freq, float period)
{
    Uint32 temp;

    //
    // 타이머 주기 초기화:
    //
    temp = (Uint32)((freq / 1000000) * period);
    CPUTimer_setPeriod(cpuTimer, temp - 1);

    //
    // 프리스케일 카운터를 1로 나누도록 설정 (SYSCLKOUT):
    //
    CPUTimer_setPreScaler(cpuTimer, 0);

    //
    // 타이머 제어 레지스터 초기화. 타이머 중지, 재로드, 
    // Free Run 비활성화, 인터럽트 활성화 상태임.
    // 추가적으로 Free 및 Soft 비트가 설정됨.
    //
    CPUTimer_stopTimer(cpuTimer);
    CPUTimer_reloadTimerCounter(cpuTimer);
    CPUTimer_setEmulationMode(cpuTimer,
                              CPUTIMER_EMULATIONMODE_STOPAFTERNEXTDECREMENT);
    CPUTimer_enableInterrupt(cpuTimer);
}





/*
@funtion	__interrupt void isr_CpuTimer0(void)
@brief		100 us 주기 인터럽트
@param		void
@return		__interrupt void
@remark	
	-	
*/
__interrupt void isr_CpuTimer0(void)
{
    //
    // 이 인터럽트에 대한 응답을 확인하여 Group 1에서 더 많은 인터럽트를 수신할 수 있도록 함
    //

    sendScib_IPC();
    
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}


/*
@funtion	__interrupt void isr_CpuTimer1(void)
@brief		1 ms 주기 인터럽트
@param		void
@return		__interrupt void
@remark	
	-	
*/
__interrupt void isr_CpuTimer1(void)
{
	xTimer.Cycle_1ms ++;
	xTimer.Cycle_10ms ++;
	xTimer.Cycle_100ms ++;
	xTimer.Cycle_1000ms ++;

    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}



/*
@funtion	__interrupt void isr_CpuTimer2(void)
@brief		1000 ms 주기 인터럽트
@param		void
@return		__interrupt void
@remark	
	-	
*/
__interrupt void isr_CpuTimer2(void)
{
    //
    // CPU가 인터럽트를 인지함
    //

    xTimer.Hz = xTimer.Hzcnt;
    xTimer.Hzcnt = 0u;

	Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}



