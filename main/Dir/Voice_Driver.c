#include "Voice_Driver.h"
#include "Utils.h"
#include "driver/gpio.h"
#include "hal/gpio_types.h"
#include <stdint.h>

void voice_Init(void) {
  // ��������ģʽ  busy����������

  gpio_config_t info = {};
  info.intr_type = GPIO_INTR_DISABLE;
  info.mode = GPIO_MODE_OUTPUT;
  info.pin_bit_mask = (1ULL << VOICE_DATA);
  gpio_config(&info);


  gpio_config_t info1 = {};
  info1.intr_type = GPIO_INTR_DISABLE;
  info1.mode = GPIO_MODE_INPUT;
  info1.pin_bit_mask = (1ULL << VOICE_BUSY);
  gpio_config(&info1);

  // ��ʼ�������������ŵ�ƽ״̬
  gpio_set_level(VOICE_BUSY, 1);
  gpio_set_level(VOICE_DATA, 1);
  DelayMs(100);

}

void voice_Transmit_Data(uint8_t Data) {
  // ʹ��һ�ߴ��ڷ������ �ȷ��͵��ֽ��ֽ�����
  gpio_set_level(VOICE_DATA, 0);
  DelayMs(10);
  // ��ʼ��������
  uint8_t tdata = 0;
  tdata = Data & 0x01;
  for (uint8_t i = 0; i < 8; i++) {
    if (tdata == 1) {
      // ���͵���1
      gpio_set_level(VOICE_DATA, 1);
      DelayUs(600);
      gpio_set_level(VOICE_DATA, 0);
      DelayUs(200);

    } else {
      gpio_set_level(VOICE_DATA, 1);
      DelayUs(200);
      gpio_set_level(VOICE_DATA, 0);
      DelayUs(600);
    }
    Data >>= 1;
    tdata = Data & 0x01;
  }
      gpio_set_level(VOICE_DATA, 1);
      DelayMs(2);


}
