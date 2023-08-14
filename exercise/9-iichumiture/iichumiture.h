#if !defined(__SI7006_H__)
#define __SI7006_H__

#define SERIAL_ADDR 0xfcc9
#define FIRMWARE_ADDR 0x84b8
#define HUM_ADDR 0xe5
#define TMP_ADDR 0xe3

#define GET_SERIAL _IOR('y', 0, int)
#define GET_FIRMWARE _IOR('y', 1, int)
#define GET_HUM _IOR('y', 2, int)
#define GET_TMP _IOR('y', 3, int)

#endif // __SI7006_H__
