/**********************************************************************
    Nexcom Co., Ltd.
    Filename         : CSU_LED.c
    Description      : System Status LED Control (Green / Orange)
    Last Updated     : 2026. 02. 11.
**********************************************************************/

/* ************************** [[   include  ]]  *********************************************************** */
#include "CSU_LED.h"


/* ************************** [[   define   ]]  *********************************************************** */



/* ************************** [[   global   ]]  *********************************************************** */
stLedStatus xLed;


/* ************************** [[  static prototype  ]]  *************************************************** */
static void HW_writeLedPin(uint16_t Index, uint16_t State); 
static void HW_toggleLedPin(uint16_t Index);


/* ************************** [[  function  ]]  *********************************************************** */

/**
 * @brief LED 관련 GPIO 초기화
 */
void initGpioDoutLed(void)
{
	// LED GREEN (GPIO0)
	GPIO_SetupPinMux(eLED_GREEN, GPIO_MUX_CPU1, 0u);
	GPIO_SetupPinOptions(eLED_GREEN, GPIO_OUTPUT, GPIO_PUSHPULL);

	// LED ORANGE (GPIO1)
	GPIO_SetupPinMux(eLED_ORANGE, GPIO_MUX_CPU1, 0u);
	GPIO_SetupPinOptions(eLED_ORANGE, GPIO_OUTPUT, GPIO_PUSHPULL);
}

/**
 * @brief LED 초기 설정
 * @details Green LED는 시스템 정상 동작 확인용으로 1초 주기로 설정하는 것을 기본으로 함
 */
void Initial_LED(void)
{
    // Green LED (GPIO 0) 설정
    xLed.ledGreen.Index  = eLED_GREEN;
    setLedModeToggle(&xLed.ledGreen, LED_TOGGLE, 10u); // 1초 주기 점멸

    // Orange LED (GPIO 1) 설정
    xLed.ledOrange.Index = eLED_ORANGE;
    setLedModeToggle(&xLed.ledOrange, LED_NONE, 0u);   // 기본 꺼짐
    setLedStatus(&xLed.ledOrange, LED_OFF);
}

/**
 * @brief LED 상태 머신 업데이트
 * @details 100ms 주기로 호출되어 토글 카운트를 관리하거나 On/Off 상태를 유지함
 */
void updateLedStatus(void)
{
    uint16_t i = 0u;
    stLed *pLed[2];
    
    pLed[0] = &xLed.ledGreen;
    pLed[1] = &xLed.ledOrange;

    for(i = 0u; i < 2u; i++)
    {
        if(pLed[i]->Toggle == LED_TOGGLE)
		{
            if(pLed[i]->Temp == 0u)
            {
                HW_toggleLedPin(pLed[i]->Index); // 레지스터 직접 제어
                pLed[i]->Temp = pLed[i]->Time;
            }
            else
            {
                pLed[i]->Temp--;
            }
        }
        else
        {
            HW_writeLedPin(pLed[i]->Index, pLed[i]->State); // 레지스터 직접 제어
        }
    }
}


/**
 * @brief LED 강제 점등/소등 설정
 * @param pLed 대상 LED 구조체 포인터
 * @param State LED_ON(0) 또는 LED_OFF(1)
 */
void setLedStatus(stLed *pLed, bool State)
{
    if(pLed->State != State)
    {
        pLed->State = State;
    }
}

/**
 * @brief LED 토글 모드 및 주기 설정
 * @param pLed 대상 LED 구조체 포인터
 * @param Mode true: Toggle 활성, false: None
 * @param Time 토글 주기 (100ms 단위)
 */
void setLedModeToggle(stLed *pLed, bool Mode, uint16_t Time)
{
    pLed->Toggle = Mode;
    pLed->Time   = Time;
    pLed->Temp   = 0u;
}

/**
 * @brief 시스템 상태(IsValid)에 따른 오렌지 LED 제어 로직
 * @details IsValid가 false(에러)일 때 오렌지 LED 상시 점등하여 에러 알림
 */
void updateOrangeLed(void)
{
    if (xXmtIpcMsg1.IsValid == false)
    {
        setLedStatus(&xLed.ledOrange, LED_ON);
    }
    else
    {
        setLedStatus(&xLed.ledOrange, LED_OFF);
    }
}


/**
 * @brief 하드웨어 레지스터를 통한 LED 출력 제어 (Internal)
 */
static void HW_writeLedPin(uint16_t Index, uint16_t State)
{
    GPIO_writePin(Index, (uint32_t)State);
}

/**
 * @brief 하드웨어 레지스터를 통한 LED 출력 반전 (Internal)
 */
static void HW_toggleLedPin(uint16_t Index)
{
    GPIO_togglePin(Index);
}
