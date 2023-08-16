#include <linux/time.h>
#include <linux/module.h>
// 1. allocation timer
struct timer_list my_timer;
int i;
void my_timer_callback(struct timer_list *timer)
{
    printk(KERN_INFO "my_timer_callback called (%ld).\n", jiffies);
    i++;
    if (i < 3)
    {
        mod_timer(&my_timer, jiffies + HZ);
    }
}
static int __init timer_init(void)
{
    printk(KERN_INFO "timer module loaded\n");
    // 2. init timer
    my_timer.expires = jiffies + 3 * HZ;
    timer_setup(&my_timer, my_timer_callback, 0);
    // 3. start timer
    add_timer(&my_timer);
    return 0;
}
static void __exit timer_exit(void)
{
    printk(KERN_INFO "Goodbye\n");
    del_timer(&my_timer);
}

module_init(timer_init);
module_exit(timer_exit);
MODULE_LICENSE("GPL");