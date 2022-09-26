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

#ifndef _TSL1401_c
#define _TSL1401_c

#include "common.h"
#include "zf_adc.h"
#include "zf_gpio.h"
#include "zf_uart.h"
#include "zf_pit.h"

#define CCD_NUM				2
#define CCD_CLK_PIN			C2												//	CCDCLK寮瀹涔
#define CCD_SI_PIN			C3												//	CCDSI 寮瀹涔
#define AD_GROUP			ADC_2											//	瀹涔AD
#define AD_CHANNEL			{ADC2_CH02_B14,ADC2_CH01_B15}					//	瀹涔AD
#define EXPOSURE_TIME		10												//	瀹涔CCD堕 浣MS
#define PIT_TIMER_CH		TIM_14											//	浣跨ㄥㄦ涓
#define AD_RESOLUTION		ADC_10BIT										//	AD杞10浣
	
#define CCD_CLK(x)			gpio_set(CCD_CLK_PIN, x)
#define CCD_SI(x)			gpio_set(CCD_SI_PIN, x)

extern uint16 ccd_data[CCD_NUM][128];										// CCD版
extern uint8 tsl1401_finish_flag;

void ccd_init(void);
void ccd_collect(void);
void ccd_send_data(UARTN_enum uart_n ,uint16 *dat);

#endif
