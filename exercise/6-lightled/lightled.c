#include "lightled.h"
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/uaccess.h>
// myleds{
// 	core_leds{
//     	    led1 = <&gpioz 5 0>;
//          led2 = <&gpioz 6 0>;
//          led3 = <&gpioz 7 0>;
//     };
//     extend_leds{
//         led1 = <&gpioe 10 0>;
//         led2 = <&gpiof 10 0>;
//         led3 = <&gpioe 8 0>;
//     };
// };
#define CNAME "myleds"
struct device_node *cnode, *enode;
int cgpiono[3], egpiono[3];
char *name[] = {"led1", "led2", "led3"};
int major = 0;
struct class *cls;
struct device *dev;
int parse_device_tree(void)
{
    cnode = of_find_node_by_path("/myleds/core_leds");
    if (cnode == NULL)
    {
        printk("find core_leds node failed!\n");
        return -ENODATA;
    }
    enode = of_find_node_by_path("/myleds/extend_leds");
    if (enode == NULL)
    {
        printk("find extend_leds node failed!\n");
        return -ENODATA;
    }
    return 0;
}
int leds_init(void)
{
    int i, ret;
    for (i = 0; i < ARRAY_SIZE(name); i++)
    {
        cgpiono[i] = of_get_named_gpio(cnode, name[i], 0);
        if (cgpiono[i] < 0)
        {
            printk("get gpio failed!\n");
            return cgpiono[i];
        }
        egpiono[i] = of_get_named_gpio(enode, name[i], 0);
        if (egpiono[i] < 0)
        {
            printk("get gpio failed!\n");
            return egpiono[i];
        }
        ret = gpio_request(cgpiono[i], NULL);
        if (ret)
        {
            printk("request gpio failed!\n");
            return ret;
        }
        ret = gpio_request(egpiono[i], NULL);
        if (ret)
        {
            printk("request gpio failed!\n");
            return ret;
        }
        ret = gpio_direction_output(cgpiono[i], 0);
        if (ret)
        {
            printk("set gpio output failed!\n");
            return ret;
        }
        ret = gpio_direction_output(egpiono[i], 0);
        if (ret)
        {
            printk("set gpio output failed!\n");
            return ret;
        }
    }
    return 0;
}
void leds_deinit(void)
{
    int i;
    for (i = 0; i < ARRAY_SIZE(name); i++)
    {
        gpio_free(cgpiono[i]);
        gpio_free(egpiono[i]);
    }
}
int myleds_open(struct inode *inode, struct file *filp)
{
    printk("myleds_open\n");
    return 0;
}
void led_set_value(int which, int status)
{
    switch (which)
    {
    case CORE_LED1:
        status == ON ? (gpio_set_value(cgpiono[0], ON)) : (gpio_set_value(cgpiono[0], OFF));
        break;
    case CORE_LED2:
        status == ON ? (gpio_set_value(cgpiono[1], ON)) : (gpio_set_value(cgpiono[1], OFF));
        break;
    case CORE_LED3:
        status == ON ? (gpio_set_value(cgpiono[2], ON)) : (gpio_set_value(cgpiono[2], OFF));
        break;
    case EXTEND_LED1:
        status == ON ? (gpio_set_value(egpiono[0], ON)) : (gpio_set_value(egpiono[0], OFF));
        break;
    case EXTEND_LED2:
        status == ON ? (gpio_set_value(egpiono[1], ON)) : (gpio_set_value(egpiono[1], OFF));
        break;
    case EXTEND_LED3:
        status == ON ? (gpio_set_value(egpiono[2], ON)) : (gpio_set_value(egpiono[2], OFF));
        break;
    }
}
long myleds_ioctl(struct file *file, unsigned int cmd, unsigned long args)
{
    int ret, which;
    ret = copy_from_user(&which, (void *)args, sizeof(which));
    if (ret)
    {
        printk("copy_from_user failed!\n");
        return -EFAULT;
    }
    switch (cmd)
    {
    case LED_ON:
        led_set_value(which, ON);
        break;
    case LED_OFF:
        led_set_value(which, OFF);
        break;
    }
    return 0;
}
int myleds_release(struct inode *inode, struct file *filp)
{
    printk("myleds_release\n");
    return 0;
}

const struct file_operations fops = {
    .open = myleds_open,
    .unlocked_ioctl = myleds_ioctl,
    .release = myleds_release,
};
static int __init myleds_init(void)
{
    int ret;
    if ((ret = parse_device_tree()))
        goto ERR1;
    if ((ret = leds_init()))
        goto ERR1;
    if ((major = register_chrdev(0, CNAME, &fops)) < 0) {
        printk("register_chrdev failed!\n");
        ret = major;
        goto ERR2;
    }
    cls = class_create(THIS_MODULE, CNAME);
    if (IS_ERR(cls)) {
        printk("class_create failed!\n");
        ret = PTR_ERR(cls);
        goto ERR3;
    }
    dev = device_create(cls, NULL, MKDEV(major, 0), NULL, CNAME);
    if (IS_ERR(dev)) {
        printk("device_create failed!\n");
        ret = PTR_ERR(dev);
        goto ERR4;
    }
    return 0;
ERR4:
    class_destroy(cls);
ERR3:
    unregister_chrdev(major, CNAME);
ERR2:
    leds_deinit();
ERR1:
    return ret;
}
static void __exit myleds_exit(void)
{
    device_destroy(cls, MKDEV(major, 0));
    class_destroy(cls);
    unregister_chrdev(major, CNAME);
    leds_deinit();
}
module_init(myleds_init);
module_exit(myleds_exit);
MODULE_LICENSE("GPL");