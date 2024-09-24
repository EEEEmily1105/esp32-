#pragma once


//ָ��ģ����ʹ�ô��� 
//��ʼ������
#include <stdint.h>



#define Finger_UART_RX GPIO_NUM_20
#define Finger_UART_TX GPIO_NUM_21
#define Finger_Int GPIO_NUM_10
#define BUFF_SIZE 1024
void Finger_Driver_Init(void);
//��ȡΨһ���к�
void Finger_Driver_GetSerialNum(void);

//��ȡģ������ BufferIDָ�ƻ�����  ������Чģ�����
uint8_t  Finger_GetTemperNumber(void);

//���ͻ�ȡͼ��ָ��   ��¼��ָ�ƷŽ���������
//���� 0 ��ȡ�ɹ�  1 :��ȡʧ��
uint8_t Finger_EnrollImage(void);

//��������  ������������  
//����0 �ɹ�  1 :ʧ�� 
uint8_t Finger_GenerateFeature(uint8_t buffid);

//���ͺϳ�ģ��ָ�� �������ļ��ں�����ģ���,���������ģ�滺������
//�ɹ�����0 ʧ�ܷ���1 
uint8_t Finger_Compositemplate(void);

//���ʹ洢ģ��ָ��  �����洢��page ID�� flash ���ݿ�λ��
//ֻ�м��ܵȼ�Ϊ0 ����1 ֧�ִ˹��� 
uint8_t Finger_StoreCharater(uint8_t pageid);

//���ü��ܵȼ� 3.3.1.12 дϵͳ�Ĵ���PS_WriteReg
uint8_t Finger_SetSecureLever(void);


//��ȡָ��->�������� -->  ���ͺϳ�ģ��  -->�洢ģ�� �߼�����
//�ɹ�����0  ʧ�ܷ���1 
uint8_t Finger_Sign_In(uint8_t pageid);

//����ָ�ƿ����������ָ��  ������������ҳ��
uint8_t Finfer_Search(void);

//��ָ֤���߼�v �ɹ�����0 
uint8_t Finger_Verify(void);

//ָ������ָ��
void Finger_Sleep(void);

//ָ��ȡ��ָ��  ȡ����ָ֤�� ȡ��ע��ָ��
uint8_t Finger_Cancel(void);
