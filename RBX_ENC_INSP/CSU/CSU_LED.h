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
    Uint16 Index:8u;    // GPIO Index (eLed 타입 사용)
    Uint16 Time:8u;     // 토글 주기 설정
    Uint16 Temp:8u;     // 카운트다운용 임시 변수
    Uint16 State:1u;    // 현재 점등 상태 (0: Off, 1: On)
    Uint16 Toggle:1u;   // 토글 모드 활성 (0: None, 1: Toggle)
    Uint16 Reserved:14u;
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
void setLedStatus(stLed *pLed, Uint16 State);

/**
 * @brief LED 토글 모드 활성화 및 주기 설정
 */
void setLedModeToggle(stLed *pLed, Uint16 State, Uint16 Time);

/**
 * @brief 시스템 상태(IsValid)에 따른 오렌지 LED 제어 로직 처리
 * @param IsValid 데이터 유효성 상태 (0: Error, 1: Normal)
 */
void updateOrangeLed(void);


#endif	// #ifndef CSU_LED_H
