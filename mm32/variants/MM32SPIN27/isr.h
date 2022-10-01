/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
* 技术讨论QQ群：一群：179029047(已满)  二群：244861897
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file				isr
* @company			成都逐飞科技有限公司
* @author			逐飞科技(QQ3184284598)
* @version			查看doc内version文件 版本说明
* @Software			IAR 8.3 or MDK 5.28
* @Target core			MM32SPIN2XPs
* @Taobao			https://seekfree.taobao.com/
* @date				2020-11-23
********************************************************************************************************************/

#ifndef _isr_h
#define _isr_h

#include "board.h"
#include "headfile.h"

// array of interrupt service routines for TIM1 ... TIM17
extern void (*timisr_ptr[7])(void);
// array of interrupt service routines for UART0 ... UART1
extern void (*uart_rx_isr_ptr[2])(void);
extern void (*uart_tx_isr_ptr[2])(void);
// array of interrupt service routines for EXTI0 ... EXTI15
extern void (*extisr_ptr[16])(void);
// array of interrupt service routines for DMA1 channel 1 ... channel 5
extern void (*dmaisr_ptr[5])(void);


#endif
