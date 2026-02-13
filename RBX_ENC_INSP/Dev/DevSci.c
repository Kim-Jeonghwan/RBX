/**********************************************************************

	Nexcom Co., Ltd.
	Copyright 2021. All Rights Reserved.

	Filename		: DevSci.c
	Version			: 00.00
	Description		: SCI 통신 장치 드라이버
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


/* ************************** [[   include  ]]  *********************************************************** */
#include "DevSci.h"


/* ************************** [[   define   ]]  *********************************************************** */
#define IPC_GPIO_PIN_SCIB_RXD	15u             // SCI RX를 위한 GPIO 번호
#define IPC_GPIO_PIN_SCIB_TXD	14u             // SCI TX를 위한 GPIO 번호
#define IPC_GPIO_CFG_SCIB_RXD	GPIO_15_SCIB_RX	// SCI RX를 위한 핀 설정
#define IPC_GPIO_CFG_SCIB_TXD	GPIO_14_SCIB_TX	// SCI TX를 위한 핀 설정



/* ************************** [[   global   ]]  *********************************************************** */
static stQsci	xQueIPC;


/* ************************** [[  static prototype  ]]  *************************************************** */
static void initScib_IPC(void);


static void enqueueSci(stQsci *pstQ, Uint16 Data);

static Uint16 dequeueSci(stQsci *pstQ, Uint16 *pData);


/* ************************** [[  function  ]]  *********************************************************** */
/*
@funtion	void Initial_SCI(void)
@brief		SCI 초기화
@param		void
@return		void
@remark	
	-	
*/
void Initial_SCI(void)
{
	initScib_IPC();

	memset(&xQueIPC, 0u, sizeof(xQueIPC));
}


/*
@funtion	static void initScib_IPC(void)
@brief		IPC용 SCIB 초기화 (Baudrate 115200)
@param		void
@return		static void
@remark	
	-	
*/
static void initScib_IPC(void)
{
	//
	// 장치 주변장치 초기화:
	//

	//
	// SCI Rx 핀인 GPIO15 설정
	//
	GPIO_setControllerCore(IPC_GPIO_PIN_SCIB_RXD, GPIO_CORE_CPU1);
	GPIO_setPinConfig(IPC_GPIO_CFG_SCIB_RXD);
	GPIO_setDirectionMode(IPC_GPIO_PIN_SCIB_RXD, GPIO_DIR_MODE_IN);
	GPIO_setPadConfig(IPC_GPIO_PIN_SCIB_RXD, GPIO_PIN_TYPE_STD);
	GPIO_setQualificationMode(IPC_GPIO_PIN_SCIB_RXD, GPIO_QUAL_ASYNC);

	//
	// SCI Tx 핀인 GPIO14 설정
	//
	GPIO_setControllerCore(IPC_GPIO_PIN_SCIB_TXD, GPIO_CORE_CPU1);
	GPIO_setPinConfig(IPC_GPIO_CFG_SCIB_TXD);
	GPIO_setDirectionMode(IPC_GPIO_PIN_SCIB_TXD, GPIO_DIR_MODE_OUT);
	GPIO_setPadConfig(IPC_GPIO_PIN_SCIB_TXD, GPIO_PIN_TYPE_STD);
	GPIO_setQualificationMode(IPC_GPIO_PIN_SCIB_TXD, GPIO_QUAL_ASYNC);


	//
	// 이 예제에서 사용되는 인터럽트들을 이 파일 내의
	// ISR 함수들에 매핑합니다.
	//
	Interrupt_register(INT_SCIB_RX, isrScib_IPC);

	Interrupt_enable(INT_SCIB_RX);
	
	Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);


    //
    // 8비트 데이터, 1스탑 비트, 패리티 없음. 보드레이트는 115200.
    //
    SCI_setConfig(SCIB_BASE, DEVICE_LSPCLK_FREQ, 115200u, (SCI_CONFIG_WLEN_8 |
                                                        SCI_CONFIG_STOP_ONE |
                                                        SCI_CONFIG_PAR_NONE));
    SCI_enableModule(SCIB_BASE);
    SCI_resetChannels(SCIB_BASE);
    SCI_enableFIFO(SCIB_BASE);

    //
    // RX 및 TX FIFO 인터럽트 활성화
    //
    //SCI_enableInterrupt(SCIB_BASE, (SCI_INT_RXFF | SCI_INT_TXFF));
    SCI_enableInterrupt(SCIB_BASE, SCI_INT_RXFF);
    SCI_disableInterrupt(SCIB_BASE, SCI_INT_RXERR);

    //
    // 송신 FIFO는 FIFO 상태 비트가 16워드 중 2 이하일 때 인터럽트를 발생시킵니다.
    // 수신 FIFO는 FIFO 상태 비트가 16워드 중 2 이상일 때 인터럽트를 발생시킵니다.
    //
    SCI_setFIFOInterruptLevel(SCIB_BASE, SCI_FIFO_TX1, SCI_FIFO_RX1);
    SCI_performSoftwareReset(SCIB_BASE);

    SCI_resetTxFIFO(SCIB_BASE);
    SCI_resetRxFIFO(SCIB_BASE);

}




/*
@funtion	static interrupt void isrScib_IPC(void)
@brief		SCIB IPC 수신 인터럽트 서비스 루틴
@param		void
@return		[ static interrupt void ]	
@remark	
	-	
*/
__interrupt void isrScib_IPC(void)
{
	static stSciB	xRcvIPC;

    Uint16 Data[1u];

	// FIFO에 데이터가 있을 때만 루프를 돌며 읽는 것이 안전하지만, 
    // 현재 레벨이 1이므로 1바이트씩 처리하는 로직을 유지
    SCI_readCharArray(SCIB_BASE, Data, 1u);

  	switch(xRcvIPC.Frame)
  	{
  	case eSciB_SOF:
  		if(Data[0u] == 0x7Eu)
  		{
  			xRcvIPC.Frame	= eSciB_MSGID;
  			xRcvIPC.POS	= 0u;
  			xRcvIPC.CRC	= 0u;
  		}
  	break;

  	case eSciB_MSGID:
		xRcvIPC.MSGID	= Data[0u];
		xRcvIPC.Frame	= eSciB_LEN;
  	break;

  	case eSciB_LEN:
		xRcvIPC.LEN	= Data[0u];
		xRcvIPC.CRC	= Data[0u];			// LEN 포함 합산
		xRcvIPC.POS = 0u;				// POS 초기화 보장
        
        if(xRcvIPC.LEN > 0u)
        {
            xRcvIPC.LEN--;              // 보정: LEN 필드가 자기 자신을 포함하므로 실제 데이터는 LEN-1개
            xRcvIPC.Frame = eSciB_DATA;
        }
        else
        {
            xRcvIPC.Frame = eSciB_CRC;   // 데이터가 없는 경우
        }
  	break;

  	case eSciB_DATA:
		xRcvIPC.DATA[xRcvIPC.POS++] = (Data[0u] & 0x00FFu); // 하위 8비트만 명시
		xRcvIPC.CRC += (Data[0u] & 0x00FFu);
		xRcvIPC.LEN--;                          // 남은 개수 하나 감소
        if(xRcvIPC.LEN == 0u)                   // 다 받았으면 CRC 단계로
        {
            xRcvIPC.Frame = eSciB_CRC;
        }	
  	break;

  	case eSciB_CRC:
  		if((xRcvIPC.CRC & 0xFFu) == Data[0u])
  		{
			xRcvIPC.Frame	= eSciB_EOT;	// 맞으면 EOT 대기
		}
		else
		{
			xRcvIPC.Frame	= eSciB_SOF;	// 틀리면 SOF 로
		}
  	break;  	

  	case eSciB_EOT:
		if(Data[0u] == 0x0Du)	// 마지막 바이트 0x0D 인지 체크
		{
			recvIpcMessage(xRcvIPC.MSGID, xRcvIPC.DATA);
		}
		xRcvIPC.Frame	= eSciB_SOF;	// 성공하든 실패하든 초기화
  	break;  	
  	
  	default:
  		xRcvIPC.Frame = eSciB_SOF;
  	break;
  	}

    SCI_clearOverflowStatus(SCIB_BASE);

    SCI_clearInterruptStatus(SCIB_BASE, SCI_INT_RXFF);

    //
    // PIE ack 발행
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);

}




/*
@funtion	void xmtScib_IPC(Uint16 data[], Uint16 len)
@brief		IPC 데이터를 송신 큐에 저장
@param		[ Uint16 data[] ] 송신할 데이터 배열
@param		[ Uint16 len ] 데이터 길이
@return		void
@remark	
	-	
*/
void xmtScib_IPC(Uint16 data[], Uint16 len)
{
#if 1 // 2025-08-05 9:13:57
	Uint16 i = 0u;

	for(i = 0u; i < len; i++)
	{
		enqueueSci(&xQueIPC, data[i]);
	}
#else
	SCI_writeCharArray(SCIB_BASE, data, len);
#endif // #if 0 // 2025-08-05 9:13:57
}



/*
@funtion	void sendScib_IPC(void)
@brief		100 us 주기로 데이터 실제 전송 
@param		void
@return		void
@remark	
	-	isr_CpuTimer0()에서 호출됨
*/
void sendScib_IPC(void)
{
    Uint16 i = 0u;
    Uint16 len = 0u;
    Uint16 popData = 0u;
    Uint16 sendData[20u] = {0u};		// 10 에서 20으로 변경

    for(i = 0u; i < 20u; i++)
    {
        if(dequeueSci(&xQueIPC, &popData) == 1u)
        {
            sendData[len ++] = popData;
        }
    }

    if(len > 0u)
    {
        SCI_writeCharArray(SCIB_BASE, sendData, len);
    }
}




/*
@funtion	static void enqueueSci(stQsci *pstQ, Uint16 Data)
@brief		SCI 송신 큐에 데이터 삽입 (Enqueue)
@param		[ stQsci *pstQ ] 큐 구조체 포인터
@param		[ Uint16 Data ] 삽입할 데이터
@return		static void
@remark	
	-	
*/
static void enqueueSci(stQsci *pstQ, Uint16 Data)
{
    Uint16 nRear = 0u;

    if(pstQ->rear <= QUEUE_MAX_SCI)
    {
        nRear = ((pstQ->rear + 1u) % QUEUE_MAX_SCI);
    }

    if(nRear != pstQ->front)
    {
        pstQ->Data[pstQ->rear]  = Data;
        pstQ->rear           	= nRear;
    }
}




/*
@funtion	static Uint16 dequeueSci(stQsci *pstQ, Uint16 *pData)
@brief		SCI 송신 큐에서 데이터 추출 (Dequeue)
@param		[ stQsci *pstQ ] 큐 구조체 포인터
@param		[ Uint16 *pData ] 추출된 데이터를 저장할 포인터
@return		[ static Uint16 ] 1: 성공, 0: 큐가 비어있음
@remark	
	-	
*/
static Uint16 dequeueSci(stQsci *pstQ, Uint16 *pData)
{
    Uint16 result = 0u;

    if(pstQ->front != pstQ->rear)
    {
        *pData = pstQ->Data[pstQ->front];

        if(pstQ->front <= QUEUE_MAX_SCI)
        {
            pstQ->front = (pstQ->front + 1u) % QUEUE_MAX_SCI;
        }

        result = 1u;
    }

    return result;
}
