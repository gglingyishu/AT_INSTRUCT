#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "gg_at.h"
#include "delay.h"
/************************************************
*����
*����������gg_at.c��һ��atָ��ͽ��ܴ����ж��Ŀ��
*�Ķ��ط�ֻ��map��Ӧö�ٺͶ�Ӧģ�鷢������������ж���ֻ�������Ӧatָ���ַ�������
*���� g_GsmWork.send(); g_GsmWork.rec(USART_RX_BUF);�жϹ�������
*ʵ�ֻ����Ļظ�ok����һ�����ظ�������߳�ʱ���ö�Ӧ��timeout�ӿ�
*
*����ƹ��У�������С��Ŀ����ʹ���ڱ�д��ͬ�������»�������Դ������
*����һ��ʹ���л����ƽ�ͨ��atָ��ķ��ͺͻظ��ַ�����
*
*ԭģ��Ӧ����ʹ���˴�����tap����ʽ�ո�����в��ԣ����忴gg.c
************************************************/
extern GG_GsmWork_TypeDef g_GsmWork;
int main(void)
{
    u8 len;	
	
    HAL_Init();                    	 	//��ʼ��HAL��    
    Stm32_Clock_Init(RCC_PLL_MUL9);   	//����ʱ��,72M
    delay_init(72);               		//��ʼ����ʱ����
    uart_init(115200);					//��ʼ������
    LED_Init();							//��ʼ��LED	
    KEY_Init();							//��ʼ������
    while(1)
    {
        delay_ms(20);//���Ե���ʱ����ʽʹ���ö�ʱ�����ؼ���
        g_GsmWork.send();//at���� 
        if(USART_RX_STA&0x8000)
        {					   
            len=USART_RX_STA&0x3fff;
            printf("\r\n�����͵���ϢΪ:\r\n");
            HAL_UART_Transmit(&UART1_Handler,(uint8_t*)USART_RX_BUF,len,1000);	
            while(__HAL_UART_GET_FLAG(&UART1_Handler,UART_FLAG_TC)!=SET);		
            printf("\r\n\r\n");
					
            g_GsmWork.rec(USART_RX_BUF);//at���մ���
					
            USART_RX_STA=0;
            
        }
       
    }
    
}
