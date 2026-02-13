/**********************************************************************

	Nexcom Co., Ltd.
	Copyright 2021. All Rights Reserved.

	Filename		: DevSpi.c
	Version			: 00.00
	Description		: 
	Tracebility		: 
	Programmer		: 
	Last Updated	: 2026. 02. 11.

	Function List	:	
						

**********************************************************************/

/*
 * Modification History
 * --------------------
 * 
 * 
*/


/* DESCRIPTION
 * 
 * 
*/


/* ************************** [[   include  ]]  *********************************************************** */
#include "DevSpi.h"

/* ************************** [[   define   ]]  *********************************************************** */
// #define SSI_SIMO_SPIB	63u // SPI SIMOB
#define ENCODER_SOMI_GPIOA	17u // SPI SOMIA (엔코더 데이터 수신)
#define ENCODER_CLK_GPIOA	18u // SPI CLKA (엔코더 클럭 출력)
// #define SSI_CS			66u // 칩 선택핀 (사용 시 활성화)


/* ************************** [[   global   ]]  *********************************************************** */


/* ************************** [[   static prototype  ]]  ************************************************** */
static void InitSpia(void);



/* ************************** [[  function  ]]  *********************************************************** */
/*
@funtion	void Initial_SPI(void)
@brief		SPI 초기화 
@param		void
@return		void
@remark	
	-	ProtoType 은 DSP2833x_GlobalPrototypes.h 정의
	-	DevDspInit.c의 DevInitPeripherals() 호출 
*/
void Initial_SPI(void)
{
	// SSI
    InitSpia();

}



/*
@funtion	static void InitSpib(void)
@brief		
@param		void
@return		static void
@remark	
	-	
*/


static void InitSpia(void)
{
    EALLOW; // 보호 레지스터 쓰기 허용

    // 핀 설정
    // GPIO_setPinConfig(GPIO_63_SPIB_SIMO);
    // GPIO_setPadConfig(SSI_SIMO_SPIB, GPIO_PIN_TYPE_STD);
    // GPIO_setQualificationMode(SSI_SIMO_SPIB, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(GPIO_17_SPIA_SOMI);
    GPIO_setPadConfig(ENCODER_SOMI_GPIOA, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(ENCODER_SOMI_GPIOA, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(GPIO_18_SPIA_CLK);
    GPIO_setPadConfig(ENCODER_CLK_GPIOA, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(ENCODER_CLK_GPIOA, GPIO_QUAL_ASYNC);

    // GPIO_setPinConfig(GPIO_66_GPIO66);
    // GPIO_setPadConfig(SSI_CS, GPIO_PIN_TYPE_STD);
    // GPIO_setQualificationMode(SSI_CS, GPIO_QUAL_SYNC);
    // GPIO_setDirectionMode(SSI_CS, GPIO_DIR_MODE_OUT);
    // GPIO_setMasterCore(SSI_CS, GPIO_CORE_CPU1);



	// Spi 초기화. 1MHz SPICLK, Mode-2(CPOL1,CPHA0), 16비트 워드 사이즈 사용.
    SPI_disableModule(SPIA_BASE);
    SPI_setConfig(SPIA_BASE, 
					DEVICE_LSPCLK_FREQ, 
					SPI_PROT_POL1PHA0,                                          // SSI엔코더는 보통 클럭이 High로 대기하다가 첫 번째 하강 엣지에서 데이터를 내보내는 CPOL=1, CPHA=0 (IncOder SPI1 표준 규격) 또는 CPOL1PHA1 많이 씀
					SPI_MODE_MASTER, 
					1000000u,                                                   // 일단 1MHz(260126) - 필요 시 10MHz로 변경
					16);                                                        // 8 비트 : 8, 16 비트 : 16
    SPI_disableFIFO(SPIA_BASE);
    SPI_setEmulationMode(SPIA_BASE, SPI_EMULATION_STOP_AFTER_TRANSMIT);
    SPI_enableModule(SPIA_BASE);

    EDIS;   // 보호 레지스터 쓰기 금지
}

