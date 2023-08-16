#ifndef __SI7006_H__
#define __SI7006_H__
#define SERIAL_ADDR 0xfcc9
#define FIRMWARE_ADDR 0x84b8

#define GET_SERIAL _IOR('y', 0, int)
#define GET_FIRMWARE _IOR('y', 1, int)

#endif