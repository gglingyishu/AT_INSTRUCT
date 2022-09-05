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
  
} GG_ATStepName;//atָ���(GG�޸�@GSM)�����Լ���������ӣ�֮���Ӧ��map��

typedef enum
{
    IDLE,
    WORK,
    WAIT,
    ERR
    
} GG_GsmSendSta;//��ǰ����״̬

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

//��Ӧָ�������
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
//������Ͷ�δ�����
static void OutTimeAction(void);
static void DelAction(void);    
//GG_GsmWork_TypeDef �� send��rec ����
static void Gsm_Process_Send(void);
static void Gsm_Process_Rec(unsigned char *buf);
//�û��ӿ� 
//�źţ�ccid����ȡд��
void Gsm_GSQSetData(uint8_t csq);
uint8_t Gsm_GetCSQData(void);
void Gsm_CCIDSetData(uint8_t *buf);
uint8_t* Gsm_GetCCIDData(void);
//������������
void Gsm_NetSendData(uint8_t *buf);
//�������ӳɹ���־�����з���atָ��ʹ�����
void Gsm_NetWorkStaSetBit(void);

#endif
