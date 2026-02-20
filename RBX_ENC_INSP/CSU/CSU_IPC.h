/**********************************************************************
    Nexcom Co., Ltd.
    Filename         : CSU_IPC.h
    Description      : PC Interface Communication (IPC) Protocol Definition
    Last Updated     : 2026. 02. 11.
**********************************************************************/

#ifndef CSU_IPC_H
#define CSU_IPC_H

/* ************************** [[   include  ]]  *********************************************************** */
#include "main.h"


/* ************************** [[   define   ]]  *********************************************************** */
/* 통신 패킷 관련 상수는 CSU_IPC.c에 정의됨 (SOF: 0x7E, EOT: 0x0D, ID: 0x10) */


/* ************************** [[   enum or struct   ]]  *************************************************** */



/**
 * @brief PC 제어 명령 비트필드 공용체 (ID: 0x10)
 */
typedef union {
    uint16_t all;
    struct {
        bool ReadEn:1;          // Bit 0: Read Enable (SSI Polling)
        bool SetTrig:1;         // Bit 1: Set Sequence Trigger
        bool RstTrig:1;         // Bit 2: Rst Sequence Trigger
        uint16_t Reserved_L:5;  // Bit 3-7: Reserved
        bool SetForce:1;        // Bit 8: Manual Set Force
        bool RstForce:1;        // Bit 9: Manual Rst Force
        bool PwrEn:1;           // Bit 10: Power Manual Control
        bool ManualEn:1;        // Bit 11: Manual Mode Enable
        uint16_t Reserved_H:4;  // Bit 12-15: Reserved
    } bit;
} uIpcCmd;

/**
 * @brief PC로부터 수신되는 메시지 구조체 (ID: 0x10)
 */
typedef struct {
    uint16_t IncNumber;
    uIpcCmd Command;         // 구조체 비트 공용체 사용
    float32_t Reserved1;
    float32_t Reserved2;
} stRcvIpcMsg1;

/**
 * @brief MCU에서 PC로 송신하는 상태 보고 구조체 (ID: 0x10)
 * @details 총 데이터 영역 길이: 11 Bytes (Length Byte 포함)
 * 구조: Length(1) + IncNumber(1) + Status(1) + Angle(4) + RawPD(4)
 */
typedef struct
{
    /* --- 1. Sequence Number (1 byte) --- */
    uint16_t          IncNumber:8u;   // Sequence Number (0~255)
    
    /* --- 2. 상태 및 에러 플래그 (Status - 1 bytes) --- */
    bool          IsValid:1;     // Bit 0: 데이터 유효함
    bool          Err_PV:1;      // Bit 1: 위치 에러
    bool          Err_CRC:1;     // Bit 2: SSI CRC 에러
    bool          ReadStatus:1;  // Bit 3: 폴링 중
    bool          EncBusy:1;     // Bit 4: Zero 시퀀스 중
    bool          EncDone:1;     // Bit 5: 설정 완료
    bool          EncPwrStat:1;  // Bit 6: 전원 상태
    bool          ManualMode:1;  // Bit 7: 매뉴얼 모드 상태

    /* --- 3. 데이터 필드 (4 + 4 = 8 bytes) --- */
    uint32_t          EncoderAngle;   // Angle * 10
    uint32_t          EncoderRawPD;   // RawPD
} stXmtIpcMsg1;



/* ************************** [[   global   ]]  *********************************************************** */
extern stRcvIpcMsg1 xRcvIpcMsg1;
extern stXmtIpcMsg1	xXmtIpcMsg1;



/* ************************** [[  function  ]]  *********************************************************** */
/**
 * @brief PC로부터 수신된 IPC 메시지를 해석하여 구조체에 저장
 */
void recvIpcMessage(uint16_t ID, uint16_t Data[]);

/**
 * @brief 현재 시스템 상태 및 인코더 데이터를 PC로 전송 (10ms 주기)
 */
void sendIpcMessage1(void);

#endif	// #ifndef CSU_IPC_H
