#ifndef __MYLED_H__
#define __MYLED_H__

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
// LED1  gpioe10
// LED2  gpiof10
// LED3  gpioe8
#define RCC_MP_AHB4ENSETR 0x50000a28
#define GPIOE_BASE 0x50006000
#define GPIOF_BASE 0x50007000
enum led_status{
    OFF=0,
    ON
};
enum led_num{
    LED1=1,
    LED2,
    LED3
};
typedef struct{
 int width;
    int high;
}img_t;
#define LED_ON _IOW('L',1,int)
#define LED_OFF _IOW('L',0,int)
#define UACCESS_CHAR_ARRAY _IOW('L',2,char [128])
#define UACCESS_STRUCT     _IOWR('L',3,img_t)

#define GET_CMD_SIZE(cmd) ((cmd >> 16)&0x3fff)
#endif