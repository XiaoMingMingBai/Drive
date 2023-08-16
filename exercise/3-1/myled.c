/**
 * @brief Implement blocking IO
 * @details Initinalize
*/
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#define COUNT 3
#define CNAME "mycdev"
int major = 0;
int minor = 0;
char kbuf[128] = { 0 };
struct cdev* cdev;
struct class* cls;
struct device* dev;

wait_queue_head_t wq_head; // 定义等待队列头
unsigned int condition = 0; // 定义数据是否准备好的条件变量
int mycdev_open(struct inode* inode, struct file* file)
{
    printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
    return 0;
}
ssize_t mycdev_read(struct file* file,
    char __user* ubuf, size_t size, loff_t* offs)
{
    int ret;

    // 1.判断是否是阻塞打开的
    if (file->f_flags & O_NONBLOCK) {
        // 非阻塞
        return -EINVAL;
    } else {
        // 2.阻塞
        ret = wait_event_interruptible(wq_head, condition);
        if(ret == -ERESTARTSYS){
            printk("receive signal interrupt sleep\n");
            return ret;
        }
    }

    // 3.将数据拷贝到用户空间
    printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
    if (size > sizeof(kbuf))
        size = sizeof(kbuf);
    ret = copy_to_user(ubuf, kbuf, size);
    if (ret) {
        printk("copy_from_user error\n");
        return -EIO;
    }

    // 4.将condition置为0
    condition = 0;
    return size;
}
ssize_t mycdev_write(struct file* file,
    const char __user* ubuf, size_t size, loff_t* offs)
{
    int ret;
    printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
    if (size > sizeof(kbuf))
        size = sizeof(kbuf);
    ret = copy_from_user(kbuf, ubuf, size);
    if (ret) {
        printk("copy_from_user error\n");
        return -EIO;
    }
    // 唤醒
    condition = 1;
    wake_up_interruptible(&wq_head);
    return size;
}
int mycdev_close(struct inode* inode, struct file* file)
{
    printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
    return 0;
}
const struct file_operations fops = {
    .open = mycdev_open,
    .read = mycdev_read,
    .write = mycdev_write,
    .release = mycdev_close,

};
static int __init mycdev_init(void)
{
    int ret, i;
    dev_t devno;
    // 1.分配对象cdev
    cdev = cdev_alloc();
    if (cdev == NULL) {
        printk("cdev_alloc error\n");
        ret = -ENOMEM;
        goto ERR1;
    }
    // 2.初始化对象
    cdev_init(cdev, &fops);
    // 3.申请设备号
    if (major > 0) {
        // 3.1静态申请
        ret = register_chrdev_region(MKDEV(major, minor), COUNT, CNAME);
        if (ret) {
            printk("register_chrdev_region error\n");
            goto ERR2;
        }
    } else {
        // 3.2动态申请
        ret = alloc_chrdev_region(&devno, minor, COUNT, CNAME);
        if (ret) {
            printk("alloc_chrdev_region error\n");
            goto ERR2;
        }
        major = MAJOR(devno);
        minor = MINOR(devno);
    }
    // 4.注册字符设备驱动
    ret = cdev_add(cdev, MKDEV(major, minor), COUNT);
    if (ret) {
        printk("cdev_add error\n");
        goto ERR3;
    }
    // 5.自动创建设备节点
    cls = class_create(THIS_MODULE, CNAME);
    if (IS_ERR(cls)) {
        printk("class_create error\n");
        ret = PTR_ERR(cls);
        goto ERR4;
    }
    for (i = 0; i < COUNT; i++) {
        dev = device_create(cls, NULL, MKDEV(major, minor + i), NULL, "%s%d", CNAME, i);
        if (IS_ERR(dev)) {
            printk("device_create error\n");
            ret = PTR_ERR(dev);
            goto ERR5;
        }
    }

    // 初始化等待队列头
    init_waitqueue_head(&wq_head);

    return 0;
ERR5:
    for (--i; i >= 0; i--) {
        device_destroy(cls, MKDEV(major, minor + i));
    }
    class_destroy(cls);
ERR4:
    cdev_del(cdev);
ERR3:
    unregister_chrdev_region(MKDEV(major, minor), COUNT);
ERR2:
    kfree(cdev);
ERR1:
    return ret;
}
static void __exit mycdev_exit(void)
{
    int i;
    for (i = 0; i < COUNT; i++) {
        device_destroy(cls, MKDEV(major, minor + i));
    }
    class_destroy(cls);
    cdev_del(cdev);
    unregister_chrdev_region(MKDEV(major, minor), COUNT);
    kfree(cdev);
}
module_init(mycdev_init);
module_exit(mycdev_exit);
MODULE_LICENSE("GPL");
