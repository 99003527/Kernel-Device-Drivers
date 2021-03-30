#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
int xvar=100;
void sayHello(void)
{
}
static int __init simple_init(void)
{
 printk("Module Dependency:\n");
 return 0;
}
static void __exit simple_exit(void)
{
 printk("It works!\n");
}
module_init(simple_init);
module_exit(simple_exit);
EXPORT_SYMBOL_GPL(xvar);
EXPORT_SYMBOL_GPL(sayHello);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sri Naga Anjaneya");
MODULE_DESCRIPTION("A Module Dependency Module");
