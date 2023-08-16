#ifndef __MYLEDS_H__
#define __MYLEDS_H__
enum led_num {
    CORE_LED1 = 0,
    CORE_LED2,
    CORE_LED3,
    EXTEND_LED1,
    EXTEND_LED2,
    EXTEND_LED3,
};
enum led_status {
    OFF,
    ON,
};
#define LED_ON _IOW('o', 1, int)
#define LED_OFF _IOW('o', 2, int)
#endif