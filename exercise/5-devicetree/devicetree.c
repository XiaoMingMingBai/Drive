#include <linux/init.h>
#include <linux/module.h>
#include <linux/of.h>
// mynode@0x12345678{
//     compatible = "xiaomingmingbai,mynode";
//     string = "hello world";
//     uint   = <0x11223344 0xaabbccdd>;
//     bindata = [00 0c 29 7b f9 be];
//     mixeddata = "devicetree",<0x1234567>,[11 22 33];
// };
struct device_node *node;
#if 1
struct property *p;
#endif
static int __init mynode_init(void)
{
#if 0
    // node = of_find_node_by_path("/mynode@12345678");
    // node = of_find_node_by_name(NULL, "mynode");
    node = of_find_compatible_node(NULL, NULL, "xiaomingmingbai,mynode");
    if (node == NULL)
    {
        printk("find node failed!\n");
        return -ENODATA;
    }
    printk("name = %s,value = %s\n", node->properties->name,
           (char *)node->properties->value);
    printk("name = %s,value = %s\n", node->properties->next->name,
           (char *)node->properties->next->value);
    printk("name = %s,value1 = %#x,value2 = %#x,length = %d\n", node->properties->next->next->name,
           __be32_to_cpup(node->properties->next->next->value),
           __be32_to_cpup(node->properties->next->next->value + 4),
           node->properties->next->next->length);
    //__be32_to_cpup() used to convert big endian to little endian
#else
    int len, i;
    // node = of_find_node_by_path("/mynode@12345678");
    // node = of_find_node_by_name(NULL, "mynode");
    node = of_find_compatible_node(NULL, NULL, "hqyj,mynode");
    if (node == NULL)
    {
        printk("find node failed!\n");
        return -ENODATA;
    }

    p = of_find_property(node, "mixeddata", &len);
    if (p == NULL)
    {
        printk("find property failed!\n");
        return -ENODATA;
    }
    printk("mixeddata = %s\n", (char *)p->value);
    printk("mixeddata = %#x\n", __be32_to_cpup(p->value + strlen(p->value) + 1));
    for (i = 0; i < 3; i++)
    {
        printk("mixeddata = %#x\n", *(unsigned char *)(p->value + strlen(p->value) + 1 + 4 + i));
    }
#endif
    return 0;
}
static void __exit mynode_exit(void)
{
}
module_init(mynode_init);
module_exit(mynode_exit);
MODULE_LICENSE("GPL");