#if !defined(__MYLED_H__)
#define __MYLED_H__
// register address
typedef struct {
 volatile unsigned int MODER;   // 0x00
 volatile unsigned int OTYPER;  // 0x04
 volatile unsigned int OSPEEDR; // 0x08
 volatile unsigned int PUPDR;   // 0x0C
 volatile unsigned int IDR;     // 0x10
 volatile unsigned int ODR;     // 0x14
 volatile unsigned int BSRR;    // 0x18
 volatile unsigned int LCKR;    // 0x1C 
 volatile unsigned int AFRL;    // 0x20 
 volatile unsigned int AFRH;    // 0x24
 volatile unsigned int BRR;     // 0x28
 volatile unsigned int res;
 volatile unsigned int SECCFGR; // 0x30
}gpio_t;

// LED1 is connected to PE10
// LED2 is connected to PF10
// LED3 is connected to PE8
#define RCC_MP_AHB4ENSETR 0x50000A28
#define GPIOE_BASE 0x50006000
#define GPIOF_BASE 0x50007000
// led selection
enum {
  LED1 = 0,
  LED2,
  LED3,
  LED_MAX
};
// led switch
enum {
  LED_OFF = 0,
  LED_ON,
  LED_TOGGLE,
};

#if 0

#define LED_ON 0b01 00000000000100 01001100 00000001
#define LED_OFF 0b01  00000000000100  01001100  00000000

#else
#define LED_ON _IOW('L',1,int)
#define LED_OFF _IOW('L',0,int)
#define GET_CMD_SIZE(cmd) ((cmd >> 16)&0x3fff)
#endif


#endif // __MYLED_H__
