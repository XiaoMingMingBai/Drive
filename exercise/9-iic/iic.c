/**
 * @brief i2c
 * @details
 */
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/module.h>
int si_probe(struct i2c_client* client, const struct i2c_device_id* id)
{
    printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
    return 0;
}
int si_remove(struct i2c_client* client)
{
    printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
    return 0;
}
struct of_device_id oftable[] = {
    {.compatible = "ming,si7006",},
    {/*NULL*/}
};
struct i2c_driver si7006 = {
    .probe = si_probe,
    .remove = si_remove,
    .driver = {
        .name = "si7006",
        .of_match_table = oftable,
    },
};
module_i2c_driver(si7006);
MODULE_LICENSE("GPL");