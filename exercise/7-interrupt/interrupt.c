#include <linux/init.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/interrupt.h>
// myirqs{
//     interrupt-parent = <&gpiof>;
//     interrupts = <9 0>,<7 0>,<8 0>;
// };
struct device_node *node;
unsigned int irqno;
irqreturn_t key_irq_handle(int irq,void *data)
{
    printk("key1 down...\n");
    return IRQ_HANDLED;
}
static int __init myirqs_init(void)
{
    int ret;
    // 1.解析得到节点
    node = of_find_node_by_path("/myirqs");
    if (node == NULL)
    {
        printk("of_find_node_by_path err\n");
        return -EINVAL;
    }
    // 2.获取软中断号
    irqno = irq_of_parse_and_map(node, 0);
    if(!irqno){
        printk("irq_of_parse_and_map err\n");
        return -EINVAL;
    }
    // 3.注册中断
    ret  = request_irq(irqno,key_irq_handle,IRQF_TRIGGER_FALLING,"key1_irq",NULL);
    if(ret){
        printk("request_irq err\n");
        return ret;
    }
    return 0;
}
static void __exit myirqs_exit(void)
{
    free_irq(irqno,NULL);
}
module_init(myirqs_init);
module_exit(myirqs_exit);
MODULE_LICENSE("GPL");