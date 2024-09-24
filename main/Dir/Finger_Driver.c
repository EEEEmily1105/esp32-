#include "Finger_Driver.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "Utils.h"
#include "hal/gpio_types.h"
#include "hal/uart_types.h"
#include "portmacro.h"
#include "soc/clk_tree_defs.h"
#include <stdint.h>
#include <stdio.h>
#include <sys/_intsup.h>


void Finger_Driver_Init(void) {

  // ��ʼ������
  uart_config_t uart_conf = {.baud_rate = 57600,
                             .data_bits = UART_DATA_8_BITS,
                             .stop_bits = UART_STOP_BITS_1,
                             // ������żУ��
                             .parity = UART_PARITY_DISABLE,
                             // ������������
                             .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
                             // ����ʱ��ΪĬ��ʼ��
                             .source_clk = UART_SCLK_DEFAULT};
  
  // ��ʼ��һ������
  // ����������Ϊ0 ˵������������ͬ���� ����������ݻ��������� ��һֱ����.
  uart_driver_install(UART_NUM_1, 2048, 0, 0, NULL, 0);

  // ���ô�������
  uart_set_pin(UART_NUM_1, Finger_UART_TX, Finger_UART_RX, UART_PIN_NO_CHANGE,
               UART_PIN_NO_CHANGE);

  uart_param_config(UART_NUM_1, &uart_conf);
  // ����uart����


  //�����ж�����
  gpio_config_t io_conf = {};
  io_conf.intr_type = GPIO_INTR_POSEDGE;
  io_conf.mode = GPIO_MODE_INPUT;
  io_conf.pin_bit_mask = (1ULL << Finger_Int);
  gpio_config(&io_conf);

  DelayMs(200);
}

// ��ȡоƬΨһ���к�
void Finger_Driver_GetSerialNum(void) {
  uint8_t rx_buffer[128] = {0};

  uint8_t cmd[13] = {0xEF, 0X01, 0XFF, 0XFF, 0XFF, 0XFF, 0X01,
                     0X00, 0X04, 0X34, 0X00, 0X00, 0x39};
  // ʹ�ô��ڷ���ָ���?
  uart_write_bytes(UART_NUM_1, (const char *)cmd, 13);

  int len =
      uart_read_bytes(UART_NUM_1, rx_buffer, 128, 500 / portTICK_PERIOD_MS);
  if (len > 0) {
    if (rx_buffer[6] == 0x07 && rx_buffer[9] == 0x00) {
      // ���ַ�����ʽ��ӡ rxbuff[10]�Ժ�����к�.
      printf("ָ��ģ��ϵ�к�:%.32s\r\n", &rx_buffer[10]);
      DelayMs(1);
    } else if (rx_buffer[6] == 0x07 && rx_buffer[9] == 0x01) {
      printf("��ȡָ�����кŴ���\r\n");
    }
  }
}

uint8_t Finger_GetTemperNumber(void) {
  uint8_t cmd[128] = {0xef, 0x01, 0xff, 0xff, 0xff, 0xff,
                      0x01, 0x00, 0x03, 0x1d, 0x00, 0x21};
  uint8_t Rx_buffer[128] = {0};

  // ����������
  uart_write_bytes(UART_NUM_1, (const char *)cmd, 13);
  // ����Ӧ���
  uart_read_bytes(UART_NUM_1, Rx_buffer, 15, 500 / portTICK_PERIOD_MS);
  if (Rx_buffer[9] == 0x00) {
    // ����ܴ洢50��ָ�� ����ֻռ��һ���ֽھ���
    return Rx_buffer[11];
  } else {
    if (Rx_buffer[9] == 0x01) {
      printf("��ȡ��Чģ���������\r\n");
      return 0;
    }
  }
     return 1;

}

uint8_t Finger_EnrollImage(void) {

  uint8_t cmd[12] = {0xef, 0x01, 0xff, 0xff, 0xff, 0xff,
                     0x01, 0x00, 0x03, 0x01, 0x00, 0x05};
  uint8_t Rx_buffer[128] = {0};
  // ����������
  uart_write_bytes(UART_NUM_1, (const char *)cmd, 12);
  // ����Ӧ���
  uart_read_bytes(UART_NUM_1, Rx_buffer, 15, 500 / portTICK_PERIOD_MS);
  if (Rx_buffer[9] == 0x00) {
    printf("��ȡͼ��ɹ�\r\n");
    return 0;

  } else if (Rx_buffer[9] == 0x01) {
    printf("¼��ָ���д���\r\n");
    return 1;
  } else if (Rx_buffer[9] == 0x02) {
    printf("��������û����ָ\r\n");
    return 1;
  } else {
    return 1;
  }
}

uint8_t Finger_GenerateFeature(uint8_t buffid) {
  uint8_t cmd[16] = {0xef, 0x01, 0xff, 0xff, 0xff,  0xff,
                     0x01, 0x00, 0x04, 0x02, buffid};
  // У��� = ����ʶ + ������ + ָ���� + ��������
  uint16_t sum = cmd[6] + cmd[7] + cmd[8] + cmd[9] + cmd[10];
  cmd[11] = sum >> 8;
  cmd[12] = sum;
  uint8_t Rx_buffer[128] = {0};
  // ����������
  uart_write_bytes(UART_NUM_1, (const char *)cmd, 13);
  // ����Ӧ���
  int len = uart_read_bytes(UART_NUM_1, Rx_buffer, 15, 500 / portTICK_PERIOD_MS);
  if (len > 1) {
    if (Rx_buffer[9] == 0x00) {
      printf("���������ɹ�\r\n");
      return 0;
    } else if (Rx_buffer[9] == 0x01) {
      printf("�հ��д���\r\n");
      return 1;

    } else if (Rx_buffer[9] == 0x06) {
      printf("ָ��̫������������\r\n");
      return 1;

    } else if (Rx_buffer[9] == 0x0a) {
      printf("�ϲ�ʧ��\r\n");
      return 1;

    } else {
      return 1;
    }

  } else {
    return 1;
  }
     return 1;

}


uint8_t Finger_Compositemplate(void)
{
    uint8_t cmd[13] = {0xef, 0x01, 0xff, 0xff, 0xff,  0xff,
                     0x01, 0x00, 0x03, 0x05,0x00,0x09 };
  // У��� = ����ʶ + ������ + ָ���� + ��������
  uint8_t Rx_buffer[128] = {0};
  // ����������
  uart_write_bytes(UART_NUM_1, (const char *)cmd, 12);
  // ����Ӧ���
  int len = uart_read_bytes(UART_NUM_1, Rx_buffer, 15, 500 / portTICK_PERIOD_MS);
  if (len > 1) {
    if (Rx_buffer[9] == 0x00) {
      printf("�ϲ��ɹ�\r\n");
      return 0;
    } else if (Rx_buffer[9] == 0x01) {
      printf("�հ��д���\r\n");
      return 1;
    } else if (Rx_buffer[9] == 0x0a) {
      printf("�ϲ�ʧ��\r\n");
      return 1;
    } else {
      return 1;
    }
  } else {
    return 1;
  }
     return 1;

}


uint8_t Finger_StoreCharater(uint8_t pageid)
{

     uint8_t cmd[15] = {0xef, 0x01, 0xff, 0xff, 0xff,  0xff,
                     0x01, 0x00, 0x06, 0x06,
                     0x01 , //buff ID 
                     pageid};
    
    //����У���

     uint16_t sum =cmd[6] + cmd[7] + cmd[8] + cmd[9] + cmd[10] + cmd[11] + cmd[12];
     cmd[13] = sum >> 8;
     cmd[14] = sum;

     // У��� = ����ʶ + ������ + ָ���� + ��������
     uint8_t Rx_buffer[128] = {0};
     // ����������
     uart_write_bytes(UART_NUM_1, (const char *)cmd, 15);
     // ����Ӧ���
     int len = uart_read_bytes(UART_NUM_1, Rx_buffer, 15, 500 / portTICK_PERIOD_MS);
     if (len > 1) {
       if (Rx_buffer[9] == 0x00) {
         printf("�洢�ɹ�\r\n");
         return 0;
       } else if (Rx_buffer[9] == 0x01) {
         printf("�հ��д���\r\n");
         return 1;
       } else if (Rx_buffer[9] == 0x18) {
         printf("дflash�д���\r\n");
         return 1;
       } else if (Rx_buffer[9] == 0x0b) {
         printf("pageid ����ָ�ƿⷶΧ\r\n");
         return 1;
       }else {
         return 1;
       }
  } else {
    return 1;
  }
     return 1;


}

uint8_t Finger_SetSecureLever(void)
{
    uint8_t cmd[15] = {0xef, 0x01, 0xff, 0xff, 0xff,  0xff,
                     0x01, 0x00, 0x05, 0x0e,
                     0x07 , //buff ID 
                     0x00, 0x00, 0x1B
                     };
     // У��� = ����ʶ + ������ + ָ���� + ��������
     uint8_t Rx_buffer[128] = {0};
     // ����������
     uart_write_bytes(UART_NUM_1, (const char *)cmd, 14);
     // ����Ӧ���
     int len = uart_read_bytes(UART_NUM_1, Rx_buffer, 15, 100 / portTICK_PERIOD_MS);
     if(len > 1) 
     {
        if(Rx_buffer[6] == 0x07 && Rx_buffer[9] == 0x00)
        {
            printf("ָ��ģ�鰲ȫ�ȼ�����Ϊ0 �ɹ�\r\n");
        }
     }else {
      printf("ʧ��\r\n");
     return 1;
     }
     return 1;

}

uint8_t Finger_Sign_In(uint8_t pageid)
{
    uint8_t n = 1 ; 
    //ʹ��gotoָ��
    SetFingertag:
    //���ȷ��ͻ�ȡͼ��ָ��
    if(Finger_EnrollImage()) goto SetFingertag;
    //�ɹ� ������������ָ��
    if(Finger_GenerateFeature(n)) goto SetFingertag;
    printf("���ǵ�%d�λ�ȡָ�Ƴɹ�\r\n",n);
    if( n <= 4 )
    {
        //��ȡ����ָ��� ���Ż�ȡָ�� 
        n += 1 ;
        goto SetFingertag;
    }
        //���ͺϲ�ģ��ָ�� 
        if(Finger_Compositemplate()) goto END;
        //���ʹ洢ģ��ָ��
        if(Finger_StoreCharater(pageid)) 
        {
            goto END;
        }else {
        return 0;
        }
    END :
    return 1;
}

uint8_t Finfer_Search(void)
{
    uint8_t cmd[15] = {0xef, 0x01, 0xff, 0xff, 0xff,  0xff,
                     0x01, 0x00, 0x08, 0x04,
                     0x01 , //buff ID 
                     0x00, 0xff //����ʼҳ����������ҳ
                     };
    //����У���
    uint16_t sum =cmd[6] + cmd[7] + cmd[8] + cmd[9] + cmd[10] + cmd[11] + cmd[12];
     cmd[13] = sum >> 8;
     cmd[14] = sum;
     // У��� = ����ʶ + ������ + ָ���� + ��������
     uint8_t Rx_buffer[128] = {0};
     // ����������
     uart_write_bytes(UART_NUM_1, (const char *)cmd, 15);
     // ����Ӧ���
     int len = uart_read_bytes(UART_NUM_1, Rx_buffer, 15, 100 / portTICK_PERIOD_MS);
     if(len > 1) 
     {
        if(Rx_buffer[6] == 0x07 && Rx_buffer[9] == 0x00)
        {
            printf("ָ�������ɹ�\r\n");
            return 0;
        }else if(Rx_buffer[6] == 0x07 && Rx_buffer[9] == 0x01){
            printf("û��������ָ��\r\n");
            return 1;
        }
        else if(Rx_buffer[6] == 0x07 && Rx_buffer[9] == 0x09){
            printf("û��������ָ��  ҳ����÷�Ϊ 0 \r\n");
            return 1;
        }
     }else {
     return 1;
     }
     return 1;

}

uint8_t Finger_Verify(void)
{
    //��ȡͼ��ָ��
    if(Finger_EnrollImage())  return 1;

    //��������ָ��
    if(Finger_GenerateFeature(1)) return 1;
    //��������ָ��ָ��
    if(Finfer_Search()) return 1;
    //�ɹ�ƥ��ָ�� 
    return  0 ;
}

uint8_t Finger_Cancel(void)
{
    
    uint8_t cmd[15] = {0xef, 0x01, 0xff, 0xff, 0xff,  0xff,
                     0x01, 0x00, 0x03, 0x30
                     };
    //����У���
    uint16_t sum =cmd[6] + cmd[7] + cmd[8] + cmd[9];
     cmd[10] = sum >> 8;
     cmd[11] = sum;
     // У��� = ����ʶ + ������ + ָ���� + ��������
     uint8_t Rx_buffer[128] = {0};
     // ����������
     uart_write_bytes(UART_NUM_1, (const char *)cmd, 12);
     // ����Ӧ���
     int len = uart_read_bytes(UART_NUM_1, Rx_buffer, 15, 100 / portTICK_PERIOD_MS);
     if(len > 1) 
     {
        if(Rx_buffer[9] == 0x00)
        {
            printf("ȡ�����óɹ�\r\n");
            return 0;
        }else if(Rx_buffer[9] == 0x01){
            printf("ȡ������ʧ��\r\n");
            return 1;
        }else if(Rx_buffer[9] == 0x31){
            printf("��������ܵȼ���ƥ��\r\n");
            return 1;
        }
     }
     return 1;
}


void Finger_Sleep(void)
{
        uint8_t cmd[15] = {0xef, 0x01, 0xff, 0xff, 0xff,  0xff,
                     0x01, 0x00, 0x03, 0x33,
                     0x00 , 0x37
                     };
     // У��� = ����ʶ + ������ + ָ���� + ��������
     uint8_t Rx_buffer[128] = {0};
     // ����������
     uart_write_bytes(UART_NUM_1, (const char *)cmd, 12);
     // ����Ӧ���
     int len = uart_read_bytes(UART_NUM_1, Rx_buffer, 15, 100 / portTICK_PERIOD_MS);
     if(len > 1) 
     {
        if(Rx_buffer[6] == 0x07 && Rx_buffer[9] == 0x00)
        {
            printf("�������óɹ�\r\n");
        }else if(Rx_buffer[6] == 0x07 && Rx_buffer[9] == 0x01){
            printf("��������ʧ��\r\n");
        }
     }
}
