#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/sched.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("cop4610g23");
MODULE_DESCRIPTION("kernel module for pt3/syscalls");

#define ENTRY_NAME "syscalls"
#define PERMS 0644
#define PARENT NULL
#define BUFFER_SIZE 5

static struct proc_dir_entry *syscalls_entry;

int start_elevator(void);
int issue_request(int start_floor, int destination_floor, int type);
int stop_elevator(void);

extern int (*STUB_start_elevator)(void);
extern int (*STUB_issue_request)(int,int,int);
extern int (*STUB_stop_elevator)(void);

int start_elevator(void) {
	return 0;
}

int issue_request(int start_floor, int destination_floor, int type) {
	return 0;
}

int stop_elevator(void) {
	return 0;
}

static ssize_t syscalls_read(struct file *file, char __user *ubuf, size_t count, loff_t *ppos) {
	char buf[4096];
	char *ptr = buf;
	int len = 0;
	len += snprintf(ptr + len, 4096 - len, "Syscall working! ");
	
	// Copy data to user space
	if (*ppos > 0 || count < len)
	{
		return 0;
	}
	if (copy_to_user(ubuf, buf, len)) 
	{
		return -EFAULT;
	}
	*ppos = len;
    return len;
}

static const struct proc_ops syscalls_fops = 
{
	.proc_read = syscalls_read,
};
static int __init elevator_init(void)
{
	STUB_start_elevator = start_elevator;
	STUB_issue_request = issue_request;
	STUB_stop_elevator = stop_elevator;

	syscalls_entry = proc_create(ENTRY_NAME, PERMS, PARENT, &syscalls_fops);
	if(!syscalls_entry) {
		return -ENOMEM;
	}

	return 0;
}
static void __exit elevator_exit(void)
{
	STUB_start_elevator = NULL;
	STUB_issue_request = NULL;
	STUB_stop_elevator = NULL;

	remove_proc_entry("syscalls", NULL);
}

module_init(elevator_init);
module_exit(elevator_exit);
