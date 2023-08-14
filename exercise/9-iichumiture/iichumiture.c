/**
 * @brief I2C driver for si7006
 * @details This program is the driver of si7006, using it can read
 *          the temperature and humidity data
*/
#include "iichumiture.h"
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#define CNAME "si7006"

struct i2c_client *gclient;
int major;
struct class *cls;
struct device *dev;

int i2c_read_serial_firmware(unsigned short reg)
{
    int ret;
    unsigned char data;
    char buf[2] = { (reg >> 8) , (reg & 0xff) };
    // 1. Encapsulated data
    struct i2c_msg msgs[] = {
        [0] = {
            .addr = gclient->addr,
            .flags = 0,
            .len = 2,
            .buf = buf,
        },
        [1] = {
            .addr = gclient->addr,
            .flags = I2C_M_RD,
            .len = 1,
            .buf = &data,
        },
    };
    // 2. Send data
    ret = i2c_transfer(gclient->adapter, msgs, ARRAY_SIZE(msgs));
    if (ret != ARRAY_SIZE(msgs))
    {
        printk(KERN_INFO "i2c_transfer failed\n");
        return -EIO;
    }
    return data;
}
int i2c_read_humiture(unsigned char reg)
{
    int ret;
    unsigned short data;
    char rbuf[] = { reg };
    // 1. Encapsulated data
    struct i2c_msg msgs[] = {
        [0] = {
            .addr = gclient->addr,
            .flags = 0,
            .len = 1,
            .buf = rbuf,
        },
        [1] = {
            .addr = gclient->addr,
            .flags = I2C_M_RD,
            .len = 2,
            .buf = (u8 *)&data,
        },
    };
    // 2. Send data
    ret = i2c_transfer(gclient->adapter, msgs, ARRAY_SIZE(msgs));
    if (ret != ARRAY_SIZE(msgs))
    {
        printk(KERN_INFO "i2c_transfer failed\n");
        return -EIO;
    }
    return data >> 8 | data << 8;
}

static int si7006_open(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "si7006_open/n");
    return 0;
}
static int si7006_release(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "si7006_release/n");
    return 0;
}
static long si7006_unlockctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int data, ret;
    printk(KERN_INFO "si7006_unlockctl/n");
    switch (cmd)
    {
        case GET_SERIAL:
            data = i2c_read_serial_firmware(SERIAL_ADDR);
            if (data < 0)
            {
                printk(KERN_INFO "i2c_read_serial_firmware failed\n");
                return -EIO;
            }
            ret = copy_to_user((void *)arg, &data, sizeof(data));
            if (ret < 0)
            {
                printk(KERN_INFO "copy_to_user failed\n");
                return -EFAULT;
            }
            break;
        case GET_FIRMWARE:
            data = i2c_read_serial_firmware(FIRMWARE_ADDR);
            if (data < 0)
            {
                printk(KERN_INFO "i2c_read_serial_firmware failed\n");
                return -EIO;
            }
            ret = copy_to_user((void *)arg, &data, sizeof(data));
            if (ret < 0)
            {
                printk(KERN_INFO "copy_to_user failed\n");
                return -EFAULT;
            }
            break;
        case GET_HUM:
            data = i2c_read_humiture(HUM_ADDR);
            if (data < 0)
            {
                printk(KERN_INFO "i2c_read_humiture failed\n");
                return -EIO;
            }
            data = data & 0xffff;
            ret = copy_to_user((void *)arg, &data, sizeof(data));
            if (ret < 0)
            {
                printk(KERN_INFO "copy_to_user failed\n");
                return -EFAULT;
            }
            break;
        case GET_TMP:
            data = i2c_read_humiture(TMP_ADDR);
            if (data < 0)
            {
                printk(KERN_INFO "i2c_read_humiture failed\n");
                return -EIO;
            }
            data = data & 0xffff;
            ret = copy_to_user((void *)arg, &data, sizeof(data));
            if (ret < 0)
            {
                printk(KERN_INFO "copy_to_user failed\n");
                return -EFAULT;
            }
            break;
    }
    return 0;
}
struct file_operations fops = {
    .open = si7006_open,
    .release = si7006_release,
    .unlocked_ioctl = si7006_unlockctl,
};
int si7006_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    gclient = client;
    printk(KERN_INFO "si7006_probe\n");
    // 1. register a char device driver
    major = register_chrdev(0, CNAME, &fops);
    if (major < 0)
    {
        printk(KERN_INFO "register_chrdev failed\n");
        return major;
    }
    // 2. create a device node
    cls = class_create(THIS_MODULE, CNAME);
    if (IS_ERR(cls))
    {
        printk(KERN_INFO "class_create failed\n");
        unregister_chrdev(major, CNAME);
        return PTR_ERR(cls);
    }
    dev = device_create(cls, NULL, MKDEV(major, 0), NULL, CNAME);
    if (IS_ERR(dev))
    {
        printk(KERN_INFO "device_create failed\n");
        class_destroy(cls);
        unregister_chrdev(major, CNAME);
        return PTR_ERR(dev);
    }
    return 0;
}
struct of_device_id oftable[] = {
    {.compatible = "ming,si7006",},
    {},
};
int si7006_remove(struct i2c_client *client)
{
    printk(KERN_INFO "si7006_remove\n");
    device_destroy(cls, MKDEV(major, 0));
    class_destroy(cls);
    unregister_chrdev(major, CNAME);
    return 0;
}
struct i2c_driver si7006 = {
    .probe = si7006_probe,
    .remove = si7006_remove,
    .driver = {
        .name = "si7006",
        .of_match_table = oftable,
    },
};

module_i2c_driver(si7006);
MODULE_LICENSE("GPL");
