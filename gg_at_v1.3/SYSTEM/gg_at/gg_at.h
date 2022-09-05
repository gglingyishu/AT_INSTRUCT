#ifndef __GG_AT_H
#define __GG_AT_H

#include "sys.h" 

typedef enum
{
    AT,
    ATEO,
    CCID,
    CGDCONT,
    CSQ,
    CREG,
    QMTCFG,
    QMTOPEN,
    QMTCONN,
    QMTSUB,
    QMTPUBEX,
    ATWAIT
  
} GG_ATStepName;//at指令步骤(GG修改@GSM)根据自己的需求添加，之后对应到map中

typedef enum
{
    IDLE,
    WORK,
    WAIT,
    ERR
    
} GG_GsmSendSta;//当前工作状态

typedef struct
{
    void (*send)();
    void (*rec)();
    
}
GG_GsmWork_TypeDef;

typedef struct
{
    GG_ATStepName Step;
    uint8_t NetWorkSta;
    uint8_t GsmCCIDNum[20];
    uint8_t GsmCsq;
    
    uint8_t WorkSta;
    
}GG_GsmSignSta_TypeDef;

//对应指令发送内容
static void AT_Action(void);
static void ATEO_Action(void);
static void CCID_Action(void);
static void CGDCONT_Action(void);
static void CSQ_Action(void);
static void CREG_Action(void);
static void QMTCFG_Action(void);
static void QMTOPEN_Action(void);
static void QMTCONN_Action(void);
static void QMTSUB_Action(void);
static void QMTPUBEX_Action(void);
//错误处理和多次错误处理
static void OutTimeAction(void);
static void DelAction(void);    
//GG_GsmWork_TypeDef 的 send和rec 调用
static void Gsm_Process_Send(void);
static void Gsm_Process_Rec(unsigned char *buf);
//用户接口 
//信号，ccid，获取写入
void Gsm_GSQSetData(uint8_t csq);
uint8_t Gsm_GetCSQData(void);
void Gsm_CCIDSetData(uint8_t *buf);
uint8_t* Gsm_GetCCIDData(void);
//联网后发送数据
void Gsm_NetSendData(uint8_t *buf);
//网络连接成功标志，自行放在at指令发送处理中
void Gsm_NetWorkStaSetBit(void);

#endif
