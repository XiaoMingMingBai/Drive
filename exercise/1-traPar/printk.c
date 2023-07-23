/**
 * This function defines a parameter and declares that the parameter can be passed.
*/
#include <linux/init.h>
#include <linux/module.h>

int backlight = 127;
module_param(backlight, int, 0664); // Declare the parameter as a passable parameter.
MODULE_PARM_DESC(backlight, "this is backlight var,rang[0-255]:defalut:127"); // Describe the variables.

static int __init demo_init(void)
{
    printk("%s:backlight = %d\n", __func__, backlight);
    return 0;
}
static void __exit demo_exit(void)
{
    printk("%s:backlight = %d\n", __func__, backlight);
}
module_init(demo_init);
module_exit(demo_exit);
MODULE_LICENSE("GPL");