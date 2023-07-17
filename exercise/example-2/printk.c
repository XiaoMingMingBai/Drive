/**
 * This function defines a character variable and a character pointer variable and declares it to be a passable variable.
*/
#include <linux/init.h>
#include <linux/module.h>

int backlight = 127;
module_param(backlight, int, 0664);
MODULE_PARM_DESC(backlight, "this is backlight var,rang[0-255]:defalut:127");

char ch='A';
module_param(ch,byte,0664);
MODULE_PARM_DESC(ch,"this is char var");

char *p = "hello_driver";
module_param(p,charp,0664);
MODULE_PARM_DESC(p,"this is char * var");

static int __init demo_init(void)
{
    printk("%s:backlight = %d\n", __func__, backlight);
    printk("%s:ch = %c\n", __func__, ch);
    printk("%s:p = %s\n", __func__, p);
    return 0;
}
static void __exit demo_exit(void)
{
    printk("%s:backlight = %d\n", __func__, backlight);
    printk("%s:ch = %c\n", __func__, ch);
    printk("%s:p = %s\n", __func__, p);
}
module_init(demo_init);
module_exit(demo_exit);
MODULE_LICENSE("GPL");