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

#include "headfile.h"
#include "isr.h"

// interrupt handlers for EXTI0 ... EXTI15
// array of pointers to extern "C" { void f(void); } functions,

void (*extisr_ptr[16])(void) = {NULL};
void (*dmaisr_ptr[5])(void) = {NULL};
void (*timisr_ptr[7])(void) = {NULL};
void (*uart_rx_isr_ptr[2])(void) = {NULL};
void (*uart_tx_isr_ptr[2])(void) = {NULL};

void TIM1_BRK_UP_TRG_COM_IRQHandler (void)
{
	if (timisr_ptr[0]) (*timisr_ptr[0])();
	uint32 state = TIM1->SR;		// 读取中断状态
	TIM1->SR &= ~state;		// 清空中断状态
}

void TIM2_IRQHandler (void)
{
	if (timisr_ptr[1]) (*timisr_ptr[1])();
	uint32 state = TIM2->SR;		// 读取中断状态
	TIM2->SR &= ~state;		// 清空中断状态
}

void TIM3_IRQHandler (void)
{
	if (timisr_ptr[2]) (*timisr_ptr[2])();
	uint32 state = TIM3->SR;		// 读取中断状态
	TIM3->SR &= ~state;		// 清空中断状态
}

void TIM8_BRK_UP_TRG_COM_IRQHandler (void)
{
	if (timisr_ptr[3]) (*timisr_ptr[3])();
	uint32 state = TIM8->SR;		// 读取中断状态
	TIM8->SR &= ~state;		// 清空中断状态
}

void TIM14_IRQHandler (void)
{
	if (timisr_ptr[4]) (*timisr_ptr[4])();
	uint32 state = TIM14->SR;		// 读取中断状态
	TIM14->SR &= ~state;		// 清空中断状态
}

void TIM16_IRQHandler (void)
{
	if (timisr_ptr[5]) (*timisr_ptr[5])();
	uint32 state = TIM16->SR;		// 读取中断状态
	TIM16->SR &= ~state;		// 清空中断状态
}

void TIM17_IRQHandler (void)
{
	if (timisr_ptr[6]) (*timisr_ptr[6])();
	uint32 state = TIM17->SR;		// 读取中断状态
	TIM17->SR &= ~state;		// 清空中断状态
}

void UART1_IRQHandler(void)
{
	if(UART1->ISR & UART_ISR_TX_INTF)		// 串口发送缓冲空中断
	{
		if (uart_tx_isr_ptr[0]) (*uart_tx_isr_ptr[0])();
		UART1->ICR |= UART_ICR_TXICLR;		// 清除中断标志位
	}
	if(UART1->ISR & UART_ISR_RX_INTF)		// 串口接收缓冲中断
	{
		if (uart_rx_isr_ptr[0]) (*uart_rx_isr_ptr[0])();
		UART1->ICR |= UART_ICR_RXICLR;		// 清除中断标志位
	}
}

void UART2_IRQHandler(void)
{
	if(UART2->ISR & UART_ISR_TX_INTF)		// 串口发送缓冲空中断
	{
		if (uart_tx_isr_ptr[1]) (*uart_tx_isr_ptr[1])();
		UART2->ICR |= UART_ICR_TXICLR;		// 清除中断标志位
	}
	if(UART2->ISR & UART_ISR_RX_INTF)		// 串口接收缓冲中断
	{
		if (uart_rx_isr_ptr[1]) (*uart_rx_isr_ptr[1])();
		UART2->ICR |= UART_ICR_RXICLR;		// 清除中断标志位
	}
}

void EXTI0_1_IRQHandler(void)
{
		// 检测与清除中断标志可以根据实际应用进行删改
	if(EXTI_GetITStatus(EXTI_Line0))		// 检测 line0 是否触发
	{
                if (extisr_ptr[0]) (*extisr_ptr[0])();
		EXTI_ClearFlag(EXTI_Line0);		// 清除 line0 触发标志
	}
	if(EXTI_GetITStatus(EXTI_Line1))		// 检测 line1 是否触发
	{
                if (extisr_ptr[1]) (*extisr_ptr[1])();
		EXTI_ClearFlag(EXTI_Line1);		// 清除 line1 触发标志
	}
//	EXTI_ClearFlag(0x0003);		// EXTI_Line0 | EXTI_Line1
}

void EXTI2_3_IRQHandler(void)
{
		// 检测与清除中断标志可以根据实际应用进行删改
	if(EXTI_GetITStatus(EXTI_Line2))		// 检测 line2 是否触发
	{
                if (extisr_ptr[2]) (*extisr_ptr[2])();
		EXTI_ClearFlag(EXTI_Line2);		// 清除 line2 触发标志
	}
	if(EXTI_GetITStatus(EXTI_Line3))		// 检测 line3 是否触发
	{
                if (extisr_ptr[3]) (*extisr_ptr[3])();
		EXTI_ClearFlag(EXTI_Line3);		// 清除 line3 触发标志
	}
//	EXTI_ClearFlag(0x000C);		// EXTI_Line2 | EXTI_Line3
}

void EXTI4_15_IRQHandler(void)
{
		// 检测与清除中断标志可以根据实际应用进行删改
	if(EXTI_GetITStatus(EXTI_Line4))		// 检测 line4 是否触发
	{
                if (extisr_ptr[4]) (*extisr_ptr[4])();
		EXTI_ClearFlag(EXTI_Line4);		// 清除 line4 触发标志
	}
	if(EXTI_GetITStatus(EXTI_Line5))		// 检测 line5 是否触发
	{
                if (extisr_ptr[5]) (*extisr_ptr[5])();
		EXTI_ClearFlag(EXTI_Line5);		// 清除 line5 触发标志
	}
	if(EXTI_GetITStatus(EXTI_Line6))		// 检测 line6 是否触发
	{
                if (extisr_ptr[6]) (*extisr_ptr[6])();
		EXTI_ClearFlag(EXTI_Line6);		// 清除 line6 触发标志
	}
	if(EXTI_GetITStatus(EXTI_Line7))		// 检测 line7 是否触发
	{
                if (extisr_ptr[7]) (*extisr_ptr[7])();
		EXTI_ClearFlag(EXTI_Line7);		// 清除 line8 触发标志
	}
	if(EXTI_GetITStatus(EXTI_Line8))		// 检测 line8 是否触发
	{
                if (extisr_ptr[8]) (*extisr_ptr[8])();
		EXTI_ClearFlag(EXTI_Line8);		// 清除 line8 触发标志
	}
	if(EXTI_GetITStatus(EXTI_Line9))		// 检测 line9 是否触发
	{
                if (extisr_ptr[9]) (*extisr_ptr[9])();
		EXTI_ClearFlag(EXTI_Line9);		// 清除 line9 触发标志
	}
	if(EXTI_GetITStatus(EXTI_Line10))		// 检测 line10 是否触发
	{
                if (extisr_ptr[10]) (*extisr_ptr[10])();
		EXTI_ClearFlag(EXTI_Line10);		// 清除 line10 触发标志
	}
	if(EXTI_GetITStatus(EXTI_Line11))		// 检测 line11 是否触发
	{
                if (extisr_ptr[11]) (*extisr_ptr[11])();
		EXTI_ClearFlag(EXTI_Line11);		// 清除 line11 触发标志
	}
	if(EXTI_GetITStatus(EXTI_Line12))		// 检测 line12 是否触发
	{
                if (extisr_ptr[12]) (*extisr_ptr[12])();
		EXTI_ClearFlag(EXTI_Line12);		// 清除 line12 触发标志
	}
	if(EXTI_GetITStatus(EXTI_Line13))		// 检测 line13 是否触发
	{
                if (extisr_ptr[13]) (*extisr_ptr[13])();
		EXTI_ClearFlag(EXTI_Line13);		// 清除 line13 触发标志
	}
	if(EXTI_GetITStatus(EXTI_Line14))		// 检测 line14 是否触发
	{
                if (extisr_ptr[14]) (*extisr_ptr[13])();
		EXTI_ClearFlag(EXTI_Line14);		// 清除 line14 触发标志
	}
	if(EXTI_GetITStatus(EXTI_Line15))		// 检测 line15 是否触发
	{
                if (extisr_ptr[15]) (*extisr_ptr[15])();
		EXTI_ClearFlag(EXTI_Line15);		// 清除 line15 触发标志
	}
//	EXTI_ClearFlag(0xFFF0);		// EXTI_Line4 | ... | EXTI_Line15
}

void DMA1_Channel1_IRQHandler(void)
{
	if(SET == DMA_GetFlagStatus(DMA1_FLAG_TC1))		// 判断触发通道
	{
                if (dmaisr_ptr[0]) (*dmaisr_ptr[0])();
		DMA_ClearFlag(DMA1_FLAG_TC1);		// 清空该通道中断标志
	}
}

void DMA1_Channel2_3_IRQHandler(void)
{
	if(SET == DMA_GetFlagStatus(DMA1_FLAG_TC2))		// 判断触发通道
	{
                if (dmaisr_ptr[1]) (*dmaisr_ptr[1])();
		DMA_ClearFlag(DMA1_FLAG_TC2);		// 清空该通道中断标志
	}
	if(SET == DMA_GetFlagStatus(DMA1_FLAG_TC3))		// 判断触发通道
	{
                if (dmaisr_ptr[2]) (*dmaisr_ptr[2])();
		DMA_ClearFlag(DMA1_FLAG_TC3);		// 清空该通道中断标志
	}
}

void DMA1_Channel4_5_IRQHandler(void)
{
	if(SET == DMA_GetFlagStatus(DMA1_FLAG_TC4))		// 判断触发通道
	{
                if (dmaisr_ptr[3]) (*dmaisr_ptr[3])();
		DMA_ClearFlag(DMA1_FLAG_TC4);		// 清空该通道中断标志
	}
	if(SET == DMA_GetFlagStatus(DMA1_FLAG_TC5))		// 判断触发通道
	{
                if (dmaisr_ptr[4]) (*dmaisr_ptr[4])();
		DMA_ClearFlag(DMA1_FLAG_TC5);		// 清空该通道中断标志
	}
}

#ifdef Will_never_be_defined
WWDG_IRQHandler
PVD_IRQHandler
PWM_IRQHandler
FLASH_IRQHandler
RCC_IRQHandler
EXTI0_1_IRQHandler
EXTI2_3_IRQHandler
EXTI4_15_IRQHandler
HWDIV_IRQHandler
DMA1_Channel1_IRQHandler
DMA1_Channel2_3_IRQHandler
DMA1_Channel4_5_IRQHandler
ADC1_IRQHandler
TIM1_BRK_UP_TRG_COM_IRQHandler
TIM1_CC_IRQHandler
TIM2_IRQHandler
TIM3_IRQHandler
TIM8_BRK_UP_TRG_COM_IRQHandler
TIM8_CC_IRQHandler
TIM14_IRQHandler
ADC2_IRQHandler
TIM16_IRQHandler
TIM17_IRQHandler
I2C1_IRQHandler
COMP1_2_3_4_5_IRQHandler
SPI1_IRQHandler
SPI2_IRQHandler
UART1_IRQHandler
UART2_IRQHandler
#endif
