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

  // 锟斤拷始锟斤拷锟斤拷锟斤拷
  uart_config_t uart_conf = {.baud_rate = 57600,
                             .data_bits = UART_DATA_8_BITS,
                             .stop_bits = UART_STOP_BITS_1,
                             // 设置奇偶校验
                             .parity = UART_PARITY_DISABLE,
                             // 设置流量控制
                             .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
                             // 设置时钟为默认始终
                             .source_clk = UART_SCLK_DEFAULT};
  
  // 锟斤拷始锟斤拷一锟斤拷锟斤拷锟斤拷
  // 当发送数组为0 说明发送数据是同步的 如果发送数据缓冲区满了 会一直阻塞.
  uart_driver_install(UART_NUM_1, 2048, 0, 0, NULL, 0);

  // 锟斤拷锟矫达拷锟斤拷锟斤拷锟斤拷
  uart_set_pin(UART_NUM_1, Finger_UART_TX, Finger_UART_RX, UART_PIN_NO_CHANGE,
               UART_PIN_NO_CHANGE);

  uart_param_config(UART_NUM_1, &uart_conf);
  // 锟斤拷锟斤拷uart锟斤拷锟斤拷


  //配置中断引脚
  gpio_config_t io_conf = {};
  io_conf.intr_type = GPIO_INTR_POSEDGE;
  io_conf.mode = GPIO_MODE_INPUT;
  io_conf.pin_bit_mask = (1ULL << Finger_Int);
  gpio_config(&io_conf);

  DelayMs(200);
}

// 获取芯片唯一序列号
void Finger_Driver_GetSerialNum(void) {
  uint8_t rx_buffer[128] = {0};

  uint8_t cmd[13] = {0xEF, 0X01, 0XFF, 0XFF, 0XFF, 0XFF, 0X01,
                     0X00, 0X04, 0X34, 0X00, 0X00, 0x39};
  // 使锟矫达拷锟节凤拷锟斤拷指锟斤拷锟?
  uart_write_bytes(UART_NUM_1, (const char *)cmd, 13);

  int len =
      uart_read_bytes(UART_NUM_1, rx_buffer, 128, 500 / portTICK_PERIOD_MS);
  if (len > 0) {
    if (rx_buffer[6] == 0x07 && rx_buffer[9] == 0x00) {
      // 以字符串形式打印 rxbuff[10]以后的序列号.
      printf("指纹模块系列号:%.32s\r\n", &rx_buffer[10]);
      DelayMs(1);
    } else if (rx_buffer[6] == 0x07 && rx_buffer[9] == 0x01) {
      printf("获取指纹序列号错误\r\n");
    }
  }
}

uint8_t Finger_GetTemperNumber(void) {
  uint8_t cmd[128] = {0xef, 0x01, 0xff, 0xff, 0xff, 0xff,
                      0x01, 0x00, 0x03, 0x1d, 0x00, 0x21};
  uint8_t Rx_buffer[128] = {0};

  // 发送命令码
  uart_write_bytes(UART_NUM_1, (const char *)cmd, 13);
  // 接受应答包
  uart_read_bytes(UART_NUM_1, Rx_buffer, 15, 500 / portTICK_PERIOD_MS);
  if (Rx_buffer[9] == 0x00) {
    // 最多能存储50个指纹 所以只占据一个字节就行
    return Rx_buffer[11];
  } else {
    if (Rx_buffer[9] == 0x01) {
      printf("读取有效模版个数有误\r\n");
      return 0;
    }
  }
     return 1;

}

uint8_t Finger_EnrollImage(void) {

  uint8_t cmd[12] = {0xef, 0x01, 0xff, 0xff, 0xff, 0xff,
                     0x01, 0x00, 0x03, 0x01, 0x00, 0x05};
  uint8_t Rx_buffer[128] = {0};
  // 发送命令码
  uart_write_bytes(UART_NUM_1, (const char *)cmd, 12);
  // 接受应答包
  uart_read_bytes(UART_NUM_1, Rx_buffer, 15, 500 / portTICK_PERIOD_MS);
  if (Rx_buffer[9] == 0x00) {
    printf("获取图像成功\r\n");
    return 0;

  } else if (Rx_buffer[9] == 0x01) {
    printf("录入指纹有错误\r\n");
    return 1;
  } else if (Rx_buffer[9] == 0x02) {
    printf("传感器上没有手指\r\n");
    return 1;
  } else {
    return 1;
  }
}

uint8_t Finger_GenerateFeature(uint8_t buffid) {
  uint8_t cmd[16] = {0xef, 0x01, 0xff, 0xff, 0xff,  0xff,
                     0x01, 0x00, 0x04, 0x02, buffid};
  // 校验和 = 包标识 + 包长度 + 指令吗 + 缓冲区号
  uint16_t sum = cmd[6] + cmd[7] + cmd[8] + cmd[9] + cmd[10];
  cmd[11] = sum >> 8;
  cmd[12] = sum;
  uint8_t Rx_buffer[128] = {0};
  // 发送命令码
  uart_write_bytes(UART_NUM_1, (const char *)cmd, 13);
  // 接受应答包
  int len = uart_read_bytes(UART_NUM_1, Rx_buffer, 15, 500 / portTICK_PERIOD_MS);
  if (len > 1) {
    if (Rx_buffer[9] == 0x00) {
      printf("生成特征成功\r\n");
      return 0;
    } else if (Rx_buffer[9] == 0x01) {
      printf("收包有错误\r\n");
      return 1;

    } else if (Rx_buffer[9] == 0x06) {
      printf("指纹太乱生不成特征\r\n");
      return 1;

    } else if (Rx_buffer[9] == 0x0a) {
      printf("合并失败\r\n");
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
  // 校验和 = 包标识 + 包长度 + 指令吗 + 缓冲区号
  uint8_t Rx_buffer[128] = {0};
  // 发送命令码
  uart_write_bytes(UART_NUM_1, (const char *)cmd, 12);
  // 接受应答包
  int len = uart_read_bytes(UART_NUM_1, Rx_buffer, 15, 500 / portTICK_PERIOD_MS);
  if (len > 1) {
    if (Rx_buffer[9] == 0x00) {
      printf("合并成功\r\n");
      return 0;
    } else if (Rx_buffer[9] == 0x01) {
      printf("收包有错误\r\n");
      return 1;
    } else if (Rx_buffer[9] == 0x0a) {
      printf("合并失败\r\n");
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
    
    //计算校验和

     uint16_t sum =cmd[6] + cmd[7] + cmd[8] + cmd[9] + cmd[10] + cmd[11] + cmd[12];
     cmd[13] = sum >> 8;
     cmd[14] = sum;

     // 校验和 = 包标识 + 包长度 + 指令吗 + 缓冲区号
     uint8_t Rx_buffer[128] = {0};
     // 发送命令码
     uart_write_bytes(UART_NUM_1, (const char *)cmd, 15);
     // 接受应答包
     int len = uart_read_bytes(UART_NUM_1, Rx_buffer, 15, 500 / portTICK_PERIOD_MS);
     if (len > 1) {
       if (Rx_buffer[9] == 0x00) {
         printf("存储成功\r\n");
         return 0;
       } else if (Rx_buffer[9] == 0x01) {
         printf("收包有错误\r\n");
         return 1;
       } else if (Rx_buffer[9] == 0x18) {
         printf("写flash有错误\r\n");
         return 1;
       } else if (Rx_buffer[9] == 0x0b) {
         printf("pageid 超出指纹库范围\r\n");
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
     // 校验和 = 包标识 + 包长度 + 指令吗 + 缓冲区号
     uint8_t Rx_buffer[128] = {0};
     // 发送命令码
     uart_write_bytes(UART_NUM_1, (const char *)cmd, 14);
     // 接受应答包
     int len = uart_read_bytes(UART_NUM_1, Rx_buffer, 15, 100 / portTICK_PERIOD_MS);
     if(len > 1) 
     {
        if(Rx_buffer[6] == 0x07 && Rx_buffer[9] == 0x00)
        {
            printf("指纹模块安全等级设置为0 成功\r\n");
        }
     }else {
      printf("失败\r\n");
     return 1;
     }
     return 1;

}

uint8_t Finger_Sign_In(uint8_t pageid)
{
    uint8_t n = 1 ; 
    //使用goto指令
    SetFingertag:
    //首先发送获取图像指令
    if(Finger_EnrollImage()) goto SetFingertag;
    //成功 发送生成特征指令
    if(Finger_GenerateFeature(n)) goto SetFingertag;
    printf("这是第%d次获取指纹成功\r\n",n);
    if( n <= 4 )
    {
        //获取特征指令不够 接着获取指令 
        n += 1 ;
        goto SetFingertag;
    }
        //发送合并模版指令 
        if(Finger_Compositemplate()) goto END;
        //发送存储模版指令
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
                     0x00, 0xff //从起始页搜索到结束页
                     };
    //计算校验和
    uint16_t sum =cmd[6] + cmd[7] + cmd[8] + cmd[9] + cmd[10] + cmd[11] + cmd[12];
     cmd[13] = sum >> 8;
     cmd[14] = sum;
     // 校验和 = 包标识 + 包长度 + 指令吗 + 缓冲区号
     uint8_t Rx_buffer[128] = {0};
     // 发送命令码
     uart_write_bytes(UART_NUM_1, (const char *)cmd, 15);
     // 接受应答包
     int len = uart_read_bytes(UART_NUM_1, Rx_buffer, 15, 100 / portTICK_PERIOD_MS);
     if(len > 1) 
     {
        if(Rx_buffer[6] == 0x07 && Rx_buffer[9] == 0x00)
        {
            printf("指纹搜索成功\r\n");
            return 0;
        }else if(Rx_buffer[6] == 0x07 && Rx_buffer[9] == 0x01){
            printf("没有搜索到指纹\r\n");
            return 1;
        }
        else if(Rx_buffer[6] == 0x07 && Rx_buffer[9] == 0x09){
            printf("没有搜索到指纹  页码与得分为 0 \r\n");
            return 1;
        }
     }else {
     return 1;
     }
     return 1;

}

uint8_t Finger_Verify(void)
{
    //获取图像指令
    if(Finger_EnrollImage())  return 1;

    //生成特征指令
    if(Finger_GenerateFeature(1)) return 1;
    //发送搜索指纹指令
    if(Finfer_Search()) return 1;
    //成功匹配指纹 
    return  0 ;
}

uint8_t Finger_Cancel(void)
{
    
    uint8_t cmd[15] = {0xef, 0x01, 0xff, 0xff, 0xff,  0xff,
                     0x01, 0x00, 0x03, 0x30
                     };
    //计算校验和
    uint16_t sum =cmd[6] + cmd[7] + cmd[8] + cmd[9];
     cmd[10] = sum >> 8;
     cmd[11] = sum;
     // 校验和 = 包标识 + 包长度 + 指令吗 + 缓冲区号
     uint8_t Rx_buffer[128] = {0};
     // 发送命令码
     uart_write_bytes(UART_NUM_1, (const char *)cmd, 12);
     // 接受应答包
     int len = uart_read_bytes(UART_NUM_1, Rx_buffer, 15, 100 / portTICK_PERIOD_MS);
     if(len > 1) 
     {
        if(Rx_buffer[9] == 0x00)
        {
            printf("取消设置成功\r\n");
            return 0;
        }else if(Rx_buffer[9] == 0x01){
            printf("取消设置失败\r\n");
            return 1;
        }else if(Rx_buffer[9] == 0x31){
            printf("功能与加密等级不匹配\r\n");
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
     // 校验和 = 包标识 + 包长度 + 指令吗 + 缓冲区号
     uint8_t Rx_buffer[128] = {0};
     // 发送命令码
     uart_write_bytes(UART_NUM_1, (const char *)cmd, 12);
     // 接受应答包
     int len = uart_read_bytes(UART_NUM_1, Rx_buffer, 15, 100 / portTICK_PERIOD_MS);
     if(len > 1) 
     {
        if(Rx_buffer[6] == 0x07 && Rx_buffer[9] == 0x00)
        {
            printf("休眠设置成功\r\n");
        }else if(Rx_buffer[6] == 0x07 && Rx_buffer[9] == 0x01){
            printf("休眠设置失败\r\n");
        }
     }
}
