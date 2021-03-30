#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
static int __init hello_init(void)
{
 printk("Hello World..welcome\n");
 return 0;
}
static void __exit hello_exit(void)
{
 printk("Bye,Leaving the world\n");
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sri Naga Anjaneya");
MODULE_DESCRIPTION("A Simple Module");
