#include "sys.h"
#include "usart.h"	
#include "gg_at.h"	
#include "string.h"

/*
*   作者: GG
*   邮箱: guotaoyuan1998@163.com
*   VX: qingya_1998
*   创建日期: 22/08/17
*   更新日期: 22/08/18
*   版本: V1.3
*           更新说明:
*           (BETA):增加了指令错误多次后的处理函数            
*           (BETA):统一的失败判定，默认为重发，特殊需求自改
*           (V1.3):新增了部分地方注释,将大部分要移植微调的部分加入标识(GG修改@GSM)区别
*                  与之前写的库。
*           (V1.3):新增了csq，ccid等外部接口
*   ps: 计划下个版本内容：外部接口多场景测试，除了AT和ATE的action判定修改
*                         
*       使用方式:   调用	   g_GsmWork.send();-> g_GsmWork.rec(USART_RX_BUF); (推荐可以在串口完成标志判断后调用）
*                              -> 对应界面逻辑方面修改，尤其是标识部分
*/



#define AT_SENDDATA             printf

static GG_GsmSignSta_TypeDef s_AT_SignSta;
static GG_GsmSendSta Gsm_SignRecHandleCallback(unsigned char *buf);//接收处理(GG修改@GSM)

typedef struct
{
    GG_ATStepName Step;//当前步骤
    GG_ATStepName NextStep;//下一步骤
    void (*action)();//当前步骤指令发送
    void (*outTimeAction)(); //超时处理
    void (*delAction)(); //多次超时错误处理
    uint16_t outtime;//超出单位时间
    uint16_t outtimeloop;//超时错误次数     
    
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

};//(GG修改@GSM)

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

    