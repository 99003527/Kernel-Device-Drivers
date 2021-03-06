#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/kthread.h>

int count;
module_param(count,int,S_IRUGO);

static DEFINE_SPINLOCK(s1);

static struct task_struct *task1;
static struct task_struct *task2;

static int thread_one(void *pargs)
{
    int i;
    for(i=1;i<=count;i++)
    {
        spin_lock(&s1);
        printk("Thread A--%d\n",i);
        spin_unlock(&s1);
    }
    return 0;
}

static int thread_two(void *pargs)
{
    int i;
    for(i=1;i<=count;i++)
    {
        spin_lock(&s1);
        printk("Thread B--%d\n",i);
        spin_unlock(&s1);
    }
    return 0;
}

static int __init semdemo_init(void) {        //init_module
  spin_lock_init(&s1,1);
  task1=kthread_run(thread_one, NULL, "thread_A");
  task2=kthread_run(thread_two, NULL, "thread_B");
  printk("Semapphore Demo..welcome\n");
  return 0;
}
static void __exit semdemo_exit(void) {       //cleanup_module
  if(task1)
      kthread_stop(task1);
  if(task2)
      kthread_stop(task2);
  printk("Semapphore Demo,Leaving the world\n");
}

module_init(semdemo_init);
module_exit(semdemo_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sri Naga Anjaneya");
MODULE_DESCRIPTION("Semaphore Example Module");
