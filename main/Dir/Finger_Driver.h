#pragma once


//指纹模块是使用串口 
//初始化串口
#include <stdint.h>



#define Finger_UART_RX GPIO_NUM_20
#define Finger_UART_TX GPIO_NUM_21
#define Finger_Int GPIO_NUM_10
#define BUFF_SIZE 1024
void Finger_Driver_Init(void);
//获取唯一序列号
void Finger_Driver_GetSerialNum(void);

//获取模板数量 BufferID指纹缓冲区  返回有效模版个数
uint8_t  Finger_GetTemperNumber(void);

//发送获取图像指令   将录入指纹放进缓冲区中
//返回 0 获取成功  1 :获取失败
uint8_t Finger_EnrollImage(void);

//生成特征  参数缓冲区号  
//返回0 成功  1 :失败 
uint8_t Finger_GenerateFeature(uint8_t buffid);

//发送合成模版指令 将特征文件融合生成模版后,结果存在于模版缓冲区中
//成功返回0 失败返回1 
uint8_t Finger_Compositemplate(void);

//发送存储模版指令  参数存储到page ID号 flash 数据库位置
//只有加密等级为0 或者1 支持此功能 
uint8_t Finger_StoreCharater(uint8_t pageid);

//设置加密等级 3.3.1.12 写系统寄存器PS_WriteReg
uint8_t Finger_SetSecureLever(void);


//获取指纹->生成特征 -->  发送合成模版  -->存储模版 逻辑函数
//成功返回0  失败返回1 
uint8_t Finger_Sign_In(uint8_t pageid);

//搜索指纹库中有无相关指纹  返回搜索到的页码
uint8_t Finfer_Search(void);

//验证指纹逻辑v 成功返回0 
uint8_t Finger_Verify(void);

//指纹休眠指令
void Finger_Sleep(void);

//指纹取消指令  取消验证指纹 取消注册指纹
uint8_t Finger_Cancel(void);
