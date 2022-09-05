#include "sys.h"
#include "usart.h"	
#include "gg_at.h"	
#include "string.h"

/*
*   ����: GG
*   ����: guotaoyuan1998@163.com
*   VX: qingya_1998
*   ��������: 22/08/17
*   ��������: 22/08/18
*   �汾: V1.3
*           ����˵��:
*           (BETA):������ָ������κ�Ĵ�����            
*           (BETA):ͳһ��ʧ���ж���Ĭ��Ϊ�ط������������Ը�
*           (V1.3):�����˲��ֵط�ע��,���󲿷�Ҫ��ֲ΢���Ĳ��ּ����ʶ(GG�޸�@GSM)����
*                  ��֮ǰд�Ŀ⡣
*           (V1.3):������csq��ccid���ⲿ�ӿ�
*   ps: �ƻ��¸��汾���ݣ��ⲿ�ӿڶೡ�����ԣ�����AT��ATE��action�ж��޸�
*                         
*       ʹ�÷�ʽ:   ����	   g_GsmWork.send();-> g_GsmWork.rec(USART_RX_BUF); (�Ƽ������ڴ�����ɱ�־�жϺ���ã�
*                              -> ��Ӧ�����߼������޸ģ������Ǳ�ʶ����
*/



#define AT_SENDDATA             printf

static GG_GsmSignSta_TypeDef s_AT_SignSta;
static GG_GsmSendSta Gsm_SignRecHandleCallback(unsigned char *buf);//���մ���(GG�޸�@GSM)

typedef struct
{
    GG_ATStepName Step;//��ǰ����
    GG_ATStepName NextStep;//��һ����
    void (*action)();//��ǰ����ָ���
    void (*outTimeAction)(); //��ʱ����
    void (*delAction)(); //��γ�ʱ������
    uint16_t outtime;//������λʱ��
    uint16_t outtimeloop;//��ʱ�������     
    
}GG_ATWORK_TypeDef;

static GG_ATWORK_TypeDef s_atBobyHandleMap[] = 
{
    {AT,ATEO,AT_Action,OutTimeAction,DelAction,100,5},
    {ATEO,CCID,ATEO_Action,OutTimeAction,DelAction,100,5},
    {CCID,CGDCONT,CCID_Action,OutTimeAction,DelAction,100,5},
    {CGDCONT,CSQ,CGDCONT_Action,OutTimeAction,DelAction,100,5},
    {CSQ,CREG,CSQ_Action,OutTimeAction,DelAction,100,5},
    {CREG,QMTCFG,CREG_Action,OutTimeAction,DelAction,100,5},
    {QMTCFG,QMTOPEN,QMTCFG_Action,OutTimeAction,DelAction,100,5},
    {QMTOPEN,QMTCONN,QMTOPEN_Action,OutTimeAction,DelAction,100,5},
    {QMTCONN,QMTSUB,QMTCONN_Action,OutTimeAction,DelAction,100,5},
    {QMTSUB,QMTPUBEX,QMTSUB_Action,OutTimeAction,DelAction,100,5},
    {QMTPUBEX,ATWAIT,QMTPUBEX_Action,OutTimeAction,DelAction,100,5},
    {QMTPUBEX,ATWAIT,QMTPUBEX_Action,OutTimeAction,DelAction,100,5},     

};//(GG�޸�@GSM)

GG_GsmWork_TypeDef g_GsmWork =
{
    Gsm_Process_Send,
    Gsm_Process_Rec,

};

static void Gsm_Process_Send(void)
{
    static uint16_t outtime = 0;
    static uint8_t outtimeloop = 0;
    
    if(IDLE == s_AT_SignSta.WorkSta)
    {
        s_atBobyHandleMap[s_AT_SignSta.Step].action();
        s_AT_SignSta.WorkSta = WORK;
        outtime = 0;
        
    }
    else if(WAIT == s_AT_SignSta.WorkSta)
    {
        //printf("1111");
        outtimeloop = 0;
        if(s_AT_SignSta.Step != ATWAIT)
        {
            s_AT_SignSta.Step = s_atBobyHandleMap[s_AT_SignSta.Step].NextStep;
            s_AT_SignSta.WorkSta = IDLE;
        }            
    
    }
    else if((WORK == s_AT_SignSta.WorkSta)||(ERR == s_AT_SignSta.WorkSta))
    {
        if((outtime > s_atBobyHandleMap[s_AT_SignSta.Step].outtime)||(ERR == s_AT_SignSta.WorkSta))
        {
            outtimeloop++;
            if(outtimeloop < s_atBobyHandleMap[s_AT_SignSta.Step].outtimeloop)
            {
                s_atBobyHandleMap[s_AT_SignSta.Step].outTimeAction();
                
            }
            else
            {
                s_atBobyHandleMap[s_AT_SignSta.Step].delAction();
                
            }
                           
        }
        
        outtime++;
    }
    
}

static void Gsm_Process_Rec(unsigned char *buf)
{
    //if((WORK == s_AT_SignSta.WorkSta)||(s_atBobyHandleMap[s_AT_SignSta.Step].time>))
    if(WORK == s_AT_SignSta.WorkSta)
    {
        s_AT_SignSta.WorkSta = Gsm_SignRecHandleCallback(buf);  
        
    }

}

static void AT_Action(void)
{
    AT_SENDDATA("AT\r\n");
    
}

static void ATEO_Action(void)
{
    AT_SENDDATA("ATEO\r\n");
    
}

static void CCID_Action(void)
{
    AT_SENDDATA("AT+CCID\r\n");
    
}

static void CGDCONT_Action(void)
{
    AT_SENDDATA("AT+CGDCONT?\r\n");
    
}

static void CSQ_Action(void)
{
    AT_SENDDATA("AT+CSQ\r\n");    

}

static void CREG_Action(void)
{
    AT_SENDDATA("AT+CREG?\r\n");    

}

static void QMTCFG_Action(void)
{
    AT_SENDDATA("AT+QMTCFG=\"recv/mode\",0,0,1\r\n");    

}

static void QMTOPEN_Action(void)
{
    AT_SENDDATA("AT+QMTOPEN=\"recv/mode\",0,0,1\r\n");    

}
 
static void QMTCONN_Action(void)
{
    AT_SENDDATA("AT+QMTCONN=0 \r\n");    

}
   
static void QMTSUB_Action(void)
{
    AT_SENDDATA("AT+QMTSUB=0,1\r\n");    

} 

static void QMTPUBEX_Action(void)
{
    AT_SENDDATA("AT+QMTPUBEX=0\r\n");    

}
 

//static void GSM_NextAction(void)
//{
//    //GG_ATStepName name;
//    //GG_ATSignChargeCallback(ATEO);
//    s_AT_SignSta.Step = s_atBobyHandleMap[s_AT_SignSta.Step].NextStep;
//    
//}

static GG_GsmSendSta Gsm_SignRecHandleCallback(unsigned char *buf)
{
    //uint8_t sta;
	char *p;
    
    switch(s_AT_SignSta.Step)
    {
        case AT:
        {
            if(NULL != (p = strstr( (char*)buf,"OK")))
            {	
                //printf("1111");                
                return WAIT;    
            }
            else
            {
                return ERR;
            }
            break;
        }
        case ATEO:
        {
            if(NULL != (p = strstr( (char*)buf,"OK")))
            {	
                //printf("1111");                
                return WAIT;    
            }
            else
            {
                return ERR;
            }
            break;
        }
        case CCID:
        {
            if(NULL != (p = strstr( (char*)buf,"OK")))
            {	
                //printf("1111");                
                return WAIT;    
            }
            else
            {
                return ERR;
            }
            break;
        }
        case CGDCONT:
        {
            if(NULL != (p = strstr( (char*)buf,"OK")))
            {	
                //printf("1111");                
                return WAIT;    
            }
            else
            {
                return ERR;
            }            
            break;
        }
        case CSQ:
        {
            if(NULL != (p = strstr( (char*)buf,"OK")))
            {	
                //printf("1111");                
                return WAIT;    
            }
            else
            {
                return ERR;
            }            
            break;
        }           
        case CREG:
        {
            if(NULL != (p = strstr( (char*)buf,"OK")))
            {	
                //printf("1111");                
                return WAIT;    
            }
            else
            {
                return ERR;
            }            
            break;
        }          
        case QMTCFG:
        {
            if(NULL != (p = strstr( (char*)buf,"OK")))
            {	
                //printf("1111");                
                return WAIT;    
            }
            else
            {
                return ERR;
            }            
            break;
        }        
        case QMTOPEN:
        {
            if(NULL != (p = strstr( (char*)buf,"OK")))
            {	
                //printf("1111");                
                return WAIT;    
            }
            else
            {
                return ERR;
            }            
            break;
        }         
        case QMTCONN:
        {
            if(NULL != (p = strstr( (char*)buf,"OK")))
            {	
                //printf("1111");                
                return WAIT;    
            }
            else
            {
                return ERR;
            }            
            break;
        }        
        case QMTSUB:
        {
            if(NULL != (p = strstr( (char*)buf,"OK")))
            {	
                //printf("1111");                
                return WAIT;    
            }
            else
            {
                return ERR;
            }            
            break;
        }         
        case QMTPUBEX:
        {
            if(NULL != (p = strstr( (char*)buf,"OK")))
            {	
                //printf("1111");                
                return WAIT;    
            }
            else
            {
                return ERR;
            }            
            break;
        }        
        
        default:
            break;
    }
    
    return WAIT;
       
}

static void OutTimeAction(void)
{
    s_AT_SignSta.WorkSta = IDLE;
    
}

static void DelAction(void)
{
    s_AT_SignSta.WorkSta = IDLE;
    s_AT_SignSta.Step = AT;
    
}

void Gsm_GSQSetData(uint8_t csq)
{
    s_AT_SignSta.GsmCsq = csq;
    
}

uint8_t Gsm_GetCSQData(void)
{
    return s_AT_SignSta.GsmCsq;
    
}

void Gsm_CCIDSetData(uint8_t *buf)
{
    int i;
    for(i=0; i<sizeof(s_AT_SignSta.GsmCCIDNum); i++)
    {
        buf[i] = s_AT_SignSta.GsmCCIDNum[i];
    
    }
    
}    

uint8_t* Gsm_GetCCIDData(void)
{
    return s_AT_SignSta.GsmCCIDNum;
    
}

void Gsm_NetSendData(uint8_t *buf)
{
    if(1 == s_AT_SignSta.NetWorkSta)
    {
    
    }

}
void Gsm_NetWorkStaSetBit(void)
{
    s_AT_SignSta.NetWorkSta = 1;

}

    