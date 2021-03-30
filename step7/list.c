#include <linux/init.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/kfifo.h>
//#include <linux/wait.h

typedef struct priv_obj {
	struct cdev cdev;
	struct kfifo kfifo;
	struct device* pdev;
	struct list_head *entry;
}PRIV_OBJ;

LIST_HEAD(mydevlist);

struct class *pclass;
unsigned char *pbuffer;
dev_t pdevid;
#define MAX_SIZE 1024
int ndevices=5,ret = 0;
module_param(ndevices,int, S_IRUGO);

//struct PRIV_OBJ *pobj;



int pseudo_open(struct inode* inode , struct file* file)
{
    struct cdev *cptr=inode->i_cdev;
    PRIV_OBJ *pobj = container_of(cptr, PRIV_OBJ, cdev);
    file->private_data=pobj;
    printk("Pseudo--open method\n");
    return 0;
}

int pseudo_close(struct inode* inode, struct file* file)
{
	printk("Pseudo--release method\n");
	return 0;
}

ssize_t pseudo_read(struct file* file, char __user * ubuf, size_t size, loff_t * off)
{
	//printk("Pseudo--read method\n");
	//return 0;
	int rcount;
	char* tbuf;
	printk("in read  method \n");
	PRIV_OBJ *pobj=file->private_data;
	if(kfifo_is_empty(&pobj->kfifo)) 
	{
		printk("buffer is empty\n");
		return 0;
	}
//min
	rcount = size;
	if(rcount > kfifo_len(&pobj->kfifo))
	rcount = kfifo_len(&pobj->kfifo);
	tbuf = kmalloc(rcount, GFP_KERNEL);
	ret=kfifo_out(&pobj->kfifo, tbuf, rcount);
	ret=copy_to_user(ubuf, tbuf,rcount);
//error handling
	return rcount;
	kfree(tbuf);
	printk("Pseudo--read method\n");
   	return 0;
	
}

ssize_t pseudo_write(struct file * file, const char __user * ubuf , size_t size, loff_t * off)
{
	//printk("Pseudo--write method\n");
	//return -ENOSPC;
	int wcount;
	char* tbuf;
	printk("in write  method \n");
	PRIV_OBJ *pobj=file->private_data;
	if(kfifo_is_full(&pobj->kfifo))
	{
		printk("buffer is full\n");
		return -ENOSPC;
	}
	wcount = size;
	if(wcount > kfifo_avail(&pobj->kfifo))
	wcount = kfifo_avail(&pobj->kfifo);
	tbuf=kmalloc(wcount, GFP_KERNEL);
	ret=copy_from_user(tbuf, ubuf, wcount);
//error handling if copy_form_user
	kfifo_in(&pobj->kfifo, tbuf, wcount);
	return wcount;
	kfree(tbuf);
	printk("Pseudo--write method\n");
	return 0;
}

/*static long pseudo_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	printk("Pseudo--ioctl method\n");
	switch(cmd) {
		case MY_IOCTL_LEN :
			printk("ioctl--kfifo length is %d\n", kfifo_len(&pobj->kfifo));
			return(kfifo_len(&pobj->kfifo));
			break;
		case MY_IOCTL_AVAIL :
			printk("ioctl--kfifo avail is %d\n", kfifo_avail(&pobj->kfifo));
			return(kfifo_avail(&pobj->kfifo));
			break;
		case MY_IOCTL_RESET :
			printk("ioctl--kfifo got reset\n");
			kfifo_reset(&pobj->kfifo);
			return 0;
			break;
	}
	return 0;
}*/


struct file_operations fops = {
	.open = pseudo_open,
	.release = pseudo_close,
	.write = pseudo_write,
	.read = pseudo_read,
	//.unlocked_ioctl = pseudo_ioctl
};

int __init listdemo_init(void) {        //init_module
	int i,ret = 0;
  	struct priv_obj *pobj;
  	for(i=0;i<ndevices;i++) 
  	{	
    		//pclass = class_create(THIS_MODULE, "pseudo_class");
		ret=alloc_chrdev_region(&pdevid, 0, ndevices, "pseudo_sample");
		pobj = kmalloc(sizeof(PRIV_OBJ), GFP_KERNEL);
		pbuffer=kmalloc(MAX_SIZE, GFP_KERNEL);
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
		list_add_tail(&pobj->entry, mydevlist);
		return 0;  
	}
    //kfifo_alloc(&pobj->kfifo,....
   	 printk("Hello World..welcome\n");
    
}

void __exit listdemo_exit(void) {       //cleanup_module
  //	struct list_head *pcur,*pbak;
  	struct PRIV_OBJ *ptr, *qtr;
   	int i=0;
 	for(i;i<ndevices;i++) { 
   		list_for_each_safe(ptr, qtr, &mydevlist, entry) {
     		pobj = list_entry(pcur, PRIV_OBJ, entry);
     		kfifo_free(&pobj->kfifo);
     		cdev_del(&pobj->cdev);
     		unregister_chrdev_region(pdevid, ndevices);
     		device_destroy(pclass, pdevid+i);
     		//class_destroy(pclass);
     	}
     	kfree(ptr);
     	printk("Bye,Leaving the world\n");
}
module_init(listdemo_init);
module_exit(listdemo_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sri Naga Anjaneya");
MODULE_DESCRIPTION("A simple Module not ");

