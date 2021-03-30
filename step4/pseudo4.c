#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#define MAX_SIZE 1024
dev_t pdevid;
int ndevices=1;
unsigned char *pbuffer;
int rd_offset=0;
int wr_offset=0;
int buflen=0;
int ret;
struct cdev cdev;
struct file_operations fops;
struct device *pdev; //global
struct class *pclass; //global

 
int pseudo_open(struct inode* inode , struct file* file)
{
	printk("Pseudo--open method\n");
	return 0;
}
int pseudo_close(struct inode* inode , struct file* file)
{
	printk("Pseudo--release method\n");
	return 0;
}
ssize_t pseudo_read(struct file * file, char __user * ubuf , size_t size, loff_t * off)
{
	int rcount;
	printk("Pseudo--read method\n");
	if(buflen==0)
	//wr_offset-rd_offset==0
	{
		printk("buffer is empty\n");
		return 0;
	}
	rcount = size;
	if(rcount > buflen)
	rcount = buflen;
	//min of buflen, size
	ret=copy_to_user(ubuf, pbuffer + rd_offset,rcount);
	if(ret)
	{
		printk("copy to user failed\n");
		return -EFAULT;
	}
	rd_offset+=rcount;
	buflen -= rcount;
	
	return rcount;
}
ssize_t pseudo_write(struct file * file, const char __user * ubuf , size_t size, loff_t * off)
{
	int wcount;
	printk("Pseudo--write method\n");
	if(wr_offset >= MAX_SIZE)
	{
		printk("buffer is full\n");
		return -ENOSPC;
	}
	wcount = size;
	if(wcount > MAX_SIZE - wr_offset)
		wcount = MAX_SIZE - wr_offset;
	//min
	ret=copy_from_user(ubuf, pbuffer + wr_offset, wcount);
	if(ret)
	{
		printk("copy from user failed\n");
		return -EFAULT;
	}
	wr_offset+=wcount;
	buflen += wcount;
    
	return wcount;
}
 
struct file_operations fops ={
	.open= pseudo_open,
	.release = pseudo_close,
	.write= pseudo_write,
	.read= pseudo_read
};

static int __init pseudo_init(void)
{
	int ret,i=0;
	cdev_init(&cdev, &fops);
	ret=alloc_chrdev_region(&pdevid, 0, ndevices, "pseudo_sample");
	ret = cdev_add(&cdev, pdevid, 1);
	pclass = class_create(THIS_MODULE, "pseudo_class");
	//alloc_chrdev_region, cdev_init, cdev_add
	pbuffer = kmalloc(MAX_SIZE, GFP_KERNEL);
	pdev = device_create(pclass, NULL, pdevid, NULL, "psample%d",i);
	if(ret) 
	{
		printk("Pseudo: Failed to register driver\n");
		return -EINVAL;
	}
 	printk("Successfully registered,major=%d,minor=%d\n",
 	MAJOR(pdevid), MINOR(pdevid));
 	printk("Pseudo Driver Sample..welcome\n");
 	
	return 0;
}


static void __exit pseudo_exit(void) 
{
	//unregister_chrdev_region(pdevid, ndevices);
	cdev_del(&cdev);
	printk("Pseudo Driver Sample..Version 2..Bye\n");
	kfree(pbuffer);
	device_destroy(pclass, pdevid);
	class_destroy(pclass);
}


module_init(pseudo_init);
module_exit(pseudo_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sri Naga Anjaneya");
MODULE_DESCRIPTION("A Hello, World Module");
