#include <linux/init.h>
#include <linux/module.h>

/**
 * static：
 *      由于 module——init 支持自定义命名，为了避免与内核函数名重复，所以加 static 修饰
 * __init/__exit:
 *      由于 linux 对内存的极致利用，安装和卸载函数只需要执行一次，所以需要在执行后取消空间，因此加 __init/__exit
*/ 
static int __init demo_init(void)
{
    return 0;
}
static void __exit demo_exit(void)
{

}

module_init(demo_init); // 指定加载函数
module_exit(demo_exit); // 指定卸载函数

MODULE_LICENSE("GPL"); // 指定许可证
MODULE_AUTHOR("ming <xiaomingmingbai@outlook.com>"); // 指定作者信息
MODULE_DESCRIPTION("A demo"); // 指定描述信息
MODULE_ALIAS("test"); // 指定别名