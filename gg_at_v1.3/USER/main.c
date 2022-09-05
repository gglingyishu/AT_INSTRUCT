#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "gg_at.h"
#include "delay.h"
/************************************************
*测试
*测试主体是gg_at.c，一个at指令发送接受错误判定的框架
*改动地方只有map对应枚举和对应模块发送内容与接受判定（只需填入对应at指令字符串），
*调用 g_GsmWork.send(); g_GsmWork.rec(USART_RX_BUF);判断功能流程
*实现基本的回复ok走下一步，回复错误或者超时调用对应的timeout接口
*
*框架推广中，后续有小项目被人使用在编写者同意的情况下会慢慢开源上来。
*在下一次使用中会完善较通用at指令的发送和回复字符处理
*
*原模版应该是使用了大量的tap，格式空格可能有不对，主体看gg.c
************************************************/
extern GG_GsmWork_TypeDef g_GsmWork;
int main(void)
{
    u8 len;	
	
    HAL_Init();                    	 	//初始化HAL库    
    Stm32_Clock_Init(RCC_PLL_MUL9);   	//设置时钟,72M
    delay_init(72);               		//初始化延时函数
    uart_init(115200);					//初始化串口
    LED_Init();							//初始化LED	
    KEY_Init();							//初始化按键
    while(1)
    {
        delay_ms(20);//测试的延时，正式使用用定时器返回计数
        g_GsmWork.send();//at发送 
        if(USART_RX_STA&0x8000)
        {					   
            len=USART_RX_STA&0x3fff;
            printf("\r\n您发送的消息为:\r\n");
            HAL_UART_Transmit(&UART1_Handler,(uint8_t*)USART_RX_BUF,len,1000);	
            while(__HAL_UART_GET_FLAG(&UART1_Handler,UART_FLAG_TC)!=SET);		
            printf("\r\n\r\n");
					
            g_GsmWork.rec(USART_RX_BUF);//at接收处理
					
            USART_RX_STA=0;
            
        }
       
    }
    
}
