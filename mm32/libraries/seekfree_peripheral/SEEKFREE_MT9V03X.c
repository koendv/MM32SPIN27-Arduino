/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2018,逐飞科技
* All rights reserved.
* 技术讨论QQ群：一群：179029047(已满)  二群：244861897
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file				SEEKFREE_MT9V03X
* @company			成都逐飞科技有限公司
* @author			逐飞科技(QQ3184284598)
* @version			查看doc内version文件 版本说明
* @Software			IAR 8.3 or MDK 5.24
* @Taobao			https://seekfree.taobao.com/
* @date				2020-11-23
* @note
* 					接线定义：
* 					------------------------------------
* 					模块管脚			单片机管脚
* 					SDA(51的RX)			查看SEEKFREE_MT9V03X.h文件中的MT9V03X_COF_UART_TX宏定义
* 					SCL(51的TX)			查看SEEKFREE_MT9V03X.h文件中的MT9V03X_COF_UART_RX宏定义
* 					场中断(VSY)			查看SEEKFREE_MT9V03X.h文件中的MT9V03X_VSYNC_PIN宏定义
* 					行中断(HREF)		查看SEEKFREE_MT9V03X.h文件中的MT9V03X_HREF_PIN宏定义
* 					像素中断(PCLK)		查看SEEKFREE_MT9V03X.h文件中的MT9V03X_PCLK_PIN宏定义
* 					数据口(D0-D7)		查看SEEKFREE_MT9V03X.h文件中的MT9V03X_DATA_PIN宏定义
* 					------------------------------------
********************************************************************************************************************/

#include "zf_systick.h"
#include "zf_uart.h"
#include "zf_gpio.h"
#include "zf_exti.h"
#include "zf_camera.h"

#include "SEEKFREE_MT9V03X.h"

uint8 mt9v03x_finish_flag = 0;												// 一场图像采集完成标志位
uint8 mt9v03x_image[MT9V03X_H][MT9V03X_W];

static uint8	receive[3];
static uint8	receive_num = 0;
static vuint8	uart_receive_flag;

//需要配置到摄像头的数据
int16 MT9V03X_CFG[CONFIG_FINISH][2]=
{
	{AUTO_EXP,			0},													// 自动曝光设置		范围1-63 0为关闭 如果自动曝光开启  EXP_TIME命令设置的数据将会变为最大曝光时间，也就是自动曝光时间的上限
																			// 一般情况是不需要开启这个功能，因为比赛场地光线一般都比较均匀，如果遇到光线非常不均匀的情况可以尝试设置该值，增加图像稳定性
	{EXP_TIME,			450},												// 曝光时间			摄像头收到后会自动计算出最大曝光时间，如果设置过大则设置为计算出来的最大曝光值
	{FPS,				50},												// 图像帧率			摄像头收到后会自动计算出最大FPS，如果过大则设置为计算出来的最大FPS
	{SET_COL,			MT9V03X_W},											// 图像列数量		范围1-752     K60采集不允许超过188
	{SET_ROW,			MT9V03X_H},											// 图像行数量		范围1-480
	{LR_OFFSET,			0},													// 图像左右偏移量	正值 右偏移   负值 左偏移  列为188 376 752时无法设置偏移    摄像头收偏移数据后会自动计算最大偏移，如果超出则设置计算出来的最大偏移
	{UD_OFFSET,			0},													// 图像上下偏移量	正值 上偏移   负值 下偏移  行为120 240 480时无法设置偏移    摄像头收偏移数据后会自动计算最大偏移，如果超出则设置计算出来的最大偏移
	{GAIN,				32},												// 图像增益			范围16-64     增益可以在曝光时间固定的情况下改变图像亮暗程度


	{INIT,				0}													// 摄像头开始初始化
};

//从摄像头内部获取到的配置数据
int16 GET_CFG[CONFIG_FINISH-1][2]=
{
	{AUTO_EXP,			0},													// 自动曝光设置
	{EXP_TIME,			0},													// 曝光时间
	{FPS,				0},													// 图像帧率
	{SET_COL,			0},													// 图像列数量
	{SET_ROW,			0},													// 图像行数量
	{LR_OFFSET,			0},													// 图像左右偏移量
	{UD_OFFSET,			0},													// 图像上下偏移量
	{GAIN,				0},													// 图像增益
};

//-------------------------------------------------------------------------------------------------------------------
// @brief		MT9V03X摄像头串口中断函数
// @param		NULL
// @return		void
// @since		v1.0
// Sample usage:
// @note
//-------------------------------------------------------------------------------------------------------------------
void mt9v03x_uart_callback(void)
{
	receive[receive_num] = (uart_index[MT9V03X_COF_UART])->RDR & 0x01FF;;
	receive_num++;

	if(1==receive_num && 0XA5!=receive[0])  receive_num = 0;
	if(3 == receive_num)
	{
		receive_num = 0;
		uart_receive_flag = 1;
	}
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		配置摄像头内部配置信息
// @param		uartn			选择使用的串口
// @param		buff			发送配置信息的地址
// @return		void
// @since		v1.0
// Sample usage:				调用该函数前请先初始化串口
//-------------------------------------------------------------------------------------------------------------------
void set_config(UARTN_enum uartn, int16 buff[CONFIG_FINISH-1][2])
{
	uint16 temp, i;
	uint8  send_buffer[4];

	uart_receive_flag = 0;

	//设置参数  具体请参看问题锦集手册
	//开始配置摄像头并重新初始化
	for(i=0; i<CONFIG_FINISH; i++)
	{
		send_buffer[0] = 0xA5;
		send_buffer[1] = buff[i][0];
		temp = buff[i][1];
		send_buffer[2] = temp>>8;
		send_buffer[3] = (uint8)temp;

		uart_putbuff(uartn,send_buffer,4);
		systick_delay_ms(2);
	}
	//等待摄像头初始化成功
	while(!uart_receive_flag);
	uart_receive_flag = 0;
	while((0xff != receive[1]) || (0xff != receive[2]));
	//以上部分对摄像头配置的数据全部都会保存在摄像头上51单片机的eeprom中
	//利用set_exposure_time函数单独配置的曝光数据不存储在eeprom中
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		获取摄像头内部配置信息
// @param		uartn			选择使用的串口
// @param		buff			接收配置信息的地址
// @return		void
// @since		v1.0
// Sample usage:				调用该函数前请先初始化串口
//-------------------------------------------------------------------------------------------------------------------
void get_config(UARTN_enum uartn, int16 buff[CONFIG_FINISH-1][2])
{
	uint16 temp, i;
	uint8  send_buffer[4];

	for(i=0; i<CONFIG_FINISH-1; i++)
	{
		send_buffer[0] = 0xA5;
		send_buffer[1] = GET_STATUS;
		temp = buff[i][0];
		send_buffer[2] = temp>>8;
		send_buffer[3] = (uint8)temp;

		uart_putbuff(uartn,send_buffer,4);

		//等待接受回传数据
		while(!uart_receive_flag);
		uart_receive_flag = 0;

		buff[i][1] = receive[1]<<8 | receive[2];
	}
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		获取摄像头固件版本
// @param		uartn			选择使用的串口
// @return		void
// @since		v1.0
// Sample usage:				调用该函数前请先初始化串口
//-------------------------------------------------------------------------------------------------------------------
uint16 get_version(UARTN_enum uartn)
{
	uint16 temp;
	uint8  send_buffer[4];
	send_buffer[0] = 0xA5;
	send_buffer[1] = GET_STATUS;
	temp = GET_VERSION;
	send_buffer[2] = temp>>8;
	send_buffer[3] = (uint8)temp;

	uart_putbuff(uartn,send_buffer,4);

	//等待接受回传数据
	while(!uart_receive_flag);
	uart_receive_flag = 0;

	return ((uint16)(receive[1]<<8) | receive[2]);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		单独设置摄像头曝光时间
// @param		uartn			选择使用的串口
// @param		light			设置曝光时间越大图像越亮，摄像头收到后会根据分辨率及FPS计算最大曝光时间如果设置的数据过大，那么摄像头将会设置这个最大值
// @return		uint16			当前曝光值，用于确认是否正确写入
// @since		v1.0
// Sample usage:				调用该函数前请先初始化串口
//-------------------------------------------------------------------------------------------------------------------
uint16 set_exposure_time(UARTN_enum uartn, uint16 light)
{
	uint16 temp;
	uint8  send_buffer[4];

	send_buffer[0] = 0xA5;
	send_buffer[1] = SET_EXP_TIME;
	temp = light;
	send_buffer[2] = temp>>8;
	send_buffer[3] = (uint8)temp;

	uart_putbuff(uartn,send_buffer,4);

	//等待接受回传数据
	while(!uart_receive_flag);
	uart_receive_flag = 0;

	temp = receive[1]<<8 | receive[2];
	return temp;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		对摄像头内部寄存器进行写操作
// @param		uartn			选择使用的串口
// @param		addr			摄像头内部寄存器地址
// @param		data			需要写入的数据
// @return		uint16			寄存器当前数据，用于确认是否写入成功
// @since		v1.0
// Sample usage:				调用该函数前请先初始化串口
//-------------------------------------------------------------------------------------------------------------------
uint16 set_mt9v03x_reg(UARTN_enum uartn, uint8 addr, uint16 data)
{
	uint16 temp;
	uint8  send_buffer[4];

	send_buffer[0] = 0xA5;
	send_buffer[1] = SET_ADDR;
	temp = addr;
	send_buffer[2] = temp>>8;
	send_buffer[3] = (uint8)temp;

	uart_putbuff(uartn,send_buffer,4);
	systick_delay_ms(10);

	send_buffer[0] = 0xA5;
	send_buffer[1] = SET_DATA;
	temp = data;
	send_buffer[2] = temp>>8;
	send_buffer[3] = (uint8)temp;

	uart_putbuff(uartn,send_buffer,4);

	//等待接受回传数据
	while(!uart_receive_flag);
	uart_receive_flag = 0;

	temp = receive[1]<<8 | receive[2];
	return temp;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		初始化摄像头场中断
// @param		NULL
// @return		void
// @since		v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void mt9v03x_exti_init(void)
{
	gpio_init(MT9V03X_VSYNC_PIN, GPI, GPIO_LOW, GPI_PULL_UP);
	exti_interrupt_init(MT9V03X_VSYNC_PIN, EXTI_Trigger_Falling, 0x00);
	nvic_init(MT9V03X_VSYNC_IRQN, 0, ENABLE);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		初始化摄像头场PCLK输入
// @param		NULL
// @return		void
// @since		v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void mt9v03x_tim1_etr_init(void)
{
	camera_tim_etr_init(MT9V03X_TIMETR_PCLK);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		初始化摄像头场DMA
// @param		NULL
// @return		void
// @since		v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void mt9v03x_dma_init(void)
{
	uint8 num;
	for(num=0; num<8; num++)
	{
		gpio_init((PIN_enum)(MT9V03X_DATA_PIN + num), GPI, GPIO_LOW, GPI_PULL_UP);
	}

	//DMA1总线初始化
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	//DMA摄像头初始化
	camera_dma_init(MT9V03X_DMA_CH, (uint32)MT9V03X_DATA_ADD, (uint32)camera_buffer_addr, MT9V03X_H*MT9V03X_W);
	//中断配置
	nvic_init(MT9V03X_DMA_IRQN, 1, ENABLE);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		MT9V03X摄像头初始化
// @param		NULL
// @return		void
// @since		v1.0
// Sample usage:				使用FLEXIO接口采集摄像头
//-------------------------------------------------------------------------------------------------------------------
void mt9v03x_init(void)
{
	camera_type = CAMERA_GRAYSCALE;//设置连接摄像头类型
	camera_buffer_addr = mt9v03x_image[0];

	// 初始换串口 配置摄像头
	uart_init (MT9V03X_COF_UART, 9600,MT9V03X_COF_UART_TX, MT9V03X_COF_UART_RX);
	uart_rx_irq(MT9V03X_COF_UART, ENABLE);
	systick_delay_ms(200);

	uart_receive_flag = 0;
	//等待摄像头上电初始化成功 方式有两种：延时或者通过获取配置的方式 二选一
	//systick_delay_ms(1000);//延时方式
	get_config(MT9V03X_COF_UART, GET_CFG);//获取配置的方式

	uart_receive_flag = 0;
	set_config(MT9V03X_COF_UART, MT9V03X_CFG);

	uart_receive_flag = 0;
	//获取配置便于查看配置是否正确
	get_config(MT9V03X_COF_UART, GET_CFG);

	//DMA初始化
	mt9v03x_dma_init();
	//GPIO触发定时器初始化,PLCK引脚初始化
	mt9v03x_tim1_etr_init();
	//VSYNC初始化
	mt9v03x_exti_init();
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		MT9V03X摄像头场中断
// @param		NULL
// @return		void
// @since		v1.0
// Sample usage:				在isr.c里面先创建对应的中断函数，然后调用该函数(之后别忘记清除中断标志位)
//-------------------------------------------------------------------------------------------------------------------
void mt9v03x_vsync(void)
{
	MT9V03X_DMA_CH->CNDTR = MT9V03X_H*MT9V03X_W;							// 设置当前DMA传输的剩余数量 向下递减 该寄存器只能在DMA不工作时更改。
	MT9V03X_DMA_CH->CCR |= DMA_CCR_EN;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		MT9V03X摄像头DMA完成中断
// @param		NULL
// @return		void
// @since		v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void mt9v03x_dma(void)
{
	MT9V03X_DMA_CH->CCR &= (uint16)(~DMA_CCR_EN);							// 关闭DMA1
	mt9v03x_finish_flag = 1;												// 一副图像从采集开始到采集结束耗时3.8MS左右(50FPS、188*120分辨率)
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		总钻风摄像头图像发送至上位机查看图像
// @param		uartn			使用的串口号
// @param		image			需要发送的图像地址
// @param		width			图像的列
// @param		height			图像的行
// @return		void
// @since		v1.0
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void seekfree_sendimg_03x(UARTN_enum uartn, uint8 *image, uint16 width, uint16 height)
{
	// 发送命令
	uart_putchar(uartn,0x00);
	uart_putchar(uartn,0xff);
	uart_putchar(uartn,0x01);
	uart_putchar(uartn,0x01);
	// 发送图像
	uart_putbuff(uartn, image, width*height);
}
