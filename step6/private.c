#include <linux/cdev.h>
#include <linux/kfifo.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#define MAX_SIZE 1024

struct class *pclass;
unsigned char *pbuffer;
int ndevices=1;
int ret;
dev_t pdevid;

typedef struct priv_obj {
	struct cdev cdev;
	struct kfifo kfifo;
	struct device* pdev;
}PRIV_OBJ;

PRIV_OBJ* pobj;

int pseudo_open(struct inode * inode , struct file * file)
{
	printk("in open method \n");
	return 0;
}

int pseudo_close(struct inode * inode , struct file * file)
{
	printk("in release method \n");
	return 0;
}

ssize_t pseudo_read(struct file * file, char __user * ubuf, size_t size, loff_t * off)
{
	int rcount;
	printk("in read  method \n");
	if(kfifo_is_empty(&pobj->kfifo)) 
	{
		printk("buffer is empty\n");
		return 0;
	}
	
	rcount = size;
	if(rcount > kfifo_len(&pobj->kfifo))
		rcount = kfifo_len(&pobj->kfifo);

	char* tbuf = kmalloc(rcount, GFP_KERNEL);
	kfifo_out(&pobj->kfifo, tbuf, rcount);
	ret=copy_to_user(tbuf, ubuf,rcount);
	kfree(tbuf);
	
	return rcount;
}

ssize_t pseudo_write(struct file * file, const char __user * ubuf, size_t size, loff_t * off)
{
	int wcount;
	printk("in write  method \n");
	
	if(kfifo_is_full(&pobj->kfifo))
	{
		printk("buffer is full\n");
		return -ENOSPC;
	}
	
	wcount = size;
	if(wcount > kfifo_avail(&pobj->kfifo))wcount = kfifo_avail(&pobj->kfifo);
		char* tbuf=kmalloc(wcount, GFP_KERNEL);
		
	ret=copy_from_user(tbuf, ubuf, wcount);
	kfifo_in(&pobj->kfifo, tbuf, wcount);
	kfree(tbuf);
	
	return wcount;
}

struct file_operations fops = {
	.open = pseudo_open,
	.release = pseudo_close,
	.write = pseudo_write,
	.read = pseudo_read,
};

static int __init pseudo_init(void)
{
	int ret,i=0;
	pclass = class_create(THIS_MODULE, "pseudo_class");
	ret=alloc_chrdev_region(&pdevid, 0, ndevices, "pseudo_sample");
	pbuffer=kmalloc(MAX_SIZE, GFP_KERNEL);
	pobj=kmalloc(sizeof(PRIV_OBJ), GFP_KERNEL);
	kfifo_init(&pobj->kfifo, pbuffer,MAX_SIZE);
	if(ret)
	{
		printk("Pseudo: failed to register driver \n");
		return  -EINVAL;
	}

	cdev_init(&pobj->cdev, &fops);
	kobject_set_name(&pobj->cdev.kobj,"pdevice %d",i);
	ret = cdev_add(&pobj->cdev, pdevid,1);
	pobj->pdev = device_create(pclass,NULL,pdevid,NULL, "psample %d",i);
	printk("successfully registered, major=%d ,minor=%d \n ",MAJOR(pdevid), MINOR(pdevid));
	printk("Pseudo driver sample ...welcome \n");
	return 0;

}

static void __exit pseudo_exit(void)
{
	cdev_del(&pobj->cdev);
	unregister_chrdev_region(pdevid, ndevices);
	device_destroy(pclass, pdevid);
	class_destroy(pclass);
	printk("Pseudo driver sample...bye \n");
	kfifo_free(&pobj->kfifo);
	kfree(pobj);
}
module_init(pseudo_init);
module_exit(pseudo_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sri Naga Anjaneya");
MODULE_DESCRIPTION("A Hello, World Module");
