/**********************************************************************
    Nexcom Co., Ltd.
    Filename         : CSU_LED.h
    Description      : System Status LED Control (Green / Orange)
    Last Updated     : 2026. 02. 11.
**********************************************************************/

#ifndef CSU_LED_H
#define CSU_LED_H

/* ************************** [[   include  ]]  *********************************************************** */
#include "main.h"



/* ************************** [[   define   ]]  *********************************************************** */
#define LED_OFF		1u
#define LED_ON		0u

#define LED_NONE	0u
#define LED_TOGGLE	1u



/* ************************** [[   enum or struct   ]]  **************************************************** */

/**
 * @brief LED 인덱스 정의 (GPIO 번호 매핑)
 */
typedef enum
{
	eLED_GREEN					= 0u,
	eLED_ORANGE					= 1u
}eLed;

/**
 * @brief 개별 LED 제어 속성 구조체
 */
typedef struct
{
    uint16_t Index:8u;    // GPIO Index (eLed 타입 사용)
    uint16_t Time:8u;     // 토글 주기 설정
    uint16_t Temp:8u;     // 카운트다운용 임시 변수
    bool     State:1;     // 현재 점등 상태 (false: Off, true: On - Active Low 고려 필요)
    bool     Toggle:1;    // 토글 모드 활성 (false: None, true: Toggle)
    uint16_t Reserved:14u;
} stLed;

/**
 * @brief 시스템 전체 LED 상태 관리 구조체
 */
typedef struct
{
	stLed	ledGreen;
	stLed	ledOrange;
}stLedStatus;



/* ************************** [[   global   ]]  *********************************************************** */
extern stLedStatus xLed;



/* ************************** [[  function  ]]  *********************************************************** */

/**
 * @brief LED 제어를 위한 GPIO 방향 및 Mux 설정
 */
void initGpioDoutLed(void);

/**
 * @brief LED 변수 초기화 및 기본 동작 설정
 */
void Initial_LED(void);

/**
 * @brief LED 동작 상태 업데이트 (Main Loop 호출)
 */
void updateLedStatus(void);

/**
 * @brief LED의 On/Off 상태를 직접 설정 (토글 중단)
 */
void setLedStatus(stLed *pLed, bool State);

/**
 * @brief LED 토글 모드 활성화 및 주기 설정
 */
void setLedModeToggle(stLed *pLed, bool Mode, uint16_t Time);

/**
 * @brief 시스템 상태(IsValid)에 따른 오렌지 LED 제어 로직 처리
 */
void updateOrangeLed(void);


#endif	// #ifndef CSU_LED_H
