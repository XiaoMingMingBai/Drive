#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>

struct device_node *node;
int key_gpio;
struct timer_list timer;
unsigned int irqno;

void timer_func(struct timer_list *t)
{
    printk(KERN_INFO "timer_func()\n");
    if (gpio_get_value(key_gpio) == 0)
    {
        printk(KERN_DEBUG "key pressed\n");
    }
}
irqreturn_t key_irq_func(int irq, void *dev)
{
    printk(KERN_INFO "key_irq_func()\n");
    mod_timer(&timer, jiffies + 1);
    return IRQ_HANDLED;
}
static int __init led_init(void)
{
    int ret;
    printk(KERN_INFO "led_init()\n");
    node = of_find_node_by_path("/myirqs");
    if (node == NULL)
    {
        printk(KERN_ERR "of_find_node_by_path() fail\n");
        ret = -EINVAL;
        goto ERR0;
    }
    key_gpio = of_get_named_gpio(node, "keys", 0);
    if (key_gpio < 0)
    {
        printk(KERN_ERR "of_get_named_gpio() fail\n");
        ret = -EINVAL;
        goto ERR0;
    }
    timer.expires = jiffies + 1;
    timer_setup(&timer, timer_func, 0);
    add_timer(&timer);
    irqno = irq_of_parse_and_map(node, 0);
    if (!irqno)
    {
        printk(KERN_ERR "irq_of_parse_and_map() fail\n");
        ret = -EINVAL;
        goto ERR0;
    }
    ret = request_irq(irqno, key_irq_func, IRQF_TRIGGER_FALLING, "key1_irq", NULL);
    if (ret)
    {
        printk(KERN_ERR "request_irq() fail\n");
        goto ERR0;
    }
    return 0;
ERR0:
    return ret;
}
static void __exit led_exit(void)
{
    printk(KERN_INFO "led_exit()\n");
    free_irq(irqno, NULL);
    del_timer(&timer);
}
module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");