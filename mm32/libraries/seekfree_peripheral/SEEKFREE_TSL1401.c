/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2018,椋绉
* All rights reserved.
* 璁QQ缇わ涓缇わ179029047(宸叉弧)  浜缇わ244861897
*
* 浠ヤ瀹圭灞椋绉锛璁镐寰ㄤ涓ㄩ锛
* 娆㈣浣浣跨ㄥ苟浼搴淇瑰瀹规跺椤讳椋绉澹版
*
* @file				SEEKFREE_TSL1401
* @company			介椋绉
* @author			椋绉(QQ3184284598)
* @version			ョdocversion浠 
* @Software			IAR 8.3 or MDK 5.24
* @Taobao			https://seekfree.taobao.com/
* @date				2020-11-23
* @note
* 					绾垮涔锛
* 					------------------------------------
* 					妯″绠¤			虹
* 					CCD_AO				ョSEEKFREE_TSL1401.h浠朵腑AD_CHANNEL 瀹涔
* 					CCD_CLK				ョSEEKFREE_TSL1401.h浠朵腑CCD_CLK_PIN瀹涔
* 					CCD_SI				ョSEEKFREE_TSL1401.h浠朵腑CCD_SI_PIN 瀹涔
* 					------------------------------------
********************************************************************************************************************/

#include "SEEKFREE_TSL1401.h"

uint8 tsl1401_finish_flag;
uint16 ccd_data[CCD_NUM][128];												// CCD版
ADCCH_enum ccd_adc_ch[CCD_NUM] = AD_CHANNEL;

//-------------------------------------------------------------------------------------------------------------------
// @brief		TSL1401绾块CCD濮
// @param		NULL
// @return		void
// @since		v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void ccd_init(void)     
{
	uint8 temp;
	for(temp = 0; temp<CCD_NUM; temp++)
		adc_init(AD_GROUP, ccd_adc_ch[temp], AD_RESOLUTION);
	gpio_init(CCD_CLK_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);
	gpio_init(CCD_SI_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);
	tim_interrupt_init(PIT_TIMER_CH, 1000/EXPOSURE_TIME, 3);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		TSL1401绾块CCD惧充浣烘ョ惧
// @param		uart_n			涓插
// @param		uart_n			绾挎CCD版
// @return		void
// @since		v1.0
// Sample usage:				璋ㄨ芥板璇峰濮涓插
//-------------------------------------------------------------------------------------------------------------------
void ccd_send_data(UARTN_enum uart_n ,uint16 *dat)
{
	uint8 i = 0;
	uart_putchar(uart_n, 0x00);
	uart_putchar(uart_n, 0xff);
	uart_putchar(uart_n, 0x01);
	uart_putchar(uart_n, 0x00);

	for(i=0; i<128; i++)
	{
		uart_putchar(uart_n, dat[i]>>8);									// 楂8浣
		uart_putchar(uart_n, dat[i]&0XFF);									// 楂浣8浣
	}
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		TSL1401绾块CCD版
// @param		NULL
// @return		void
// @since		v1.0
// Sample usage:				isr.c㈠寤哄搴涓芥帮跺ㄨ芥(涔璁版や腑蹇浣)
//-------------------------------------------------------------------------------------------------------------------
void ccd_collect(void)
{
	uint8 temp;
	uint8 i = 0;

	CCD_CLK(1);
	CCD_SI(0);
	CCD_CLK(0);
	CCD_SI(1);
	CCD_CLK(1);
	CCD_SI(0);

	for(i=0;i<128;i++)
	{
		CCD_CLK(0);
		for(temp = 0; temp<CCD_NUM; temp++)
			ccd_data[temp][i] = adc_convert(AD_GROUP, ccd_adc_ch[temp]); 
		CCD_CLK(1); 
	}
	//瀹蹇浣缃1
	tsl1401_finish_flag = 1;
}
