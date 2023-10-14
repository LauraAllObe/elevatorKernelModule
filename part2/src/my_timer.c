#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("cop4610g23");
MODULE_DESCRIPTION("kernel module for pt2/timer");

#define ENTRY_NAME "timer"
#define PERMS 0644
#define PARENT NULL

static struct proc_dir_entry* timer_entry;
static struct timespec64 previous_time;

static ssize_t timer_read(struct file *file, char __user *ubuf, size_t count, loff_t *ppos)
{
	struct timespec64 ts_now;
	char buf[256];
	int len = 0;

	ktime_get_real_ts64(&ts_now);

	if (previous_time.tv_nsec != 0) {
		s64 elapsed = ts_now.tv_sec - previous_time.tv_sec;
		u64 elapsed_nsec = ts_now.tv_nsec - previous_time.tv_nsec;
		
		len = snprintf(buf, sizeof(buf), "current time: %lld.%09ld\nelapsed time: %lld.%09ld\n",
		    (long long)ts_now.tv_sec, ts_now.tv_nsec, elapsed, elapsed_nsec);
	} else {
		len = snprintf(buf, sizeof(buf), "current time: %lld.%09ld\n",
		    (long long)ts_now.tv_sec, ts_now.tv_nsec);
	}
	
	previous_time = ts_now;

	return simple_read_from_buffer(ubuf, count, ppos, buf, len);
}

static const struct proc_ops timer_fops = {
	.proc_read = timer_read,
};

static int __init timer_init(void)
{
	timer_entry = proc_create(ENTRY_NAME, PERMS, PARENT, &timer_fops);
	if(!timer_entry) {
		return -ENOMEM;
	}
	ktime_get_real_ts64(&previous_time);
	return 0;
}

static void __exit timer_exit(void)
{
	proc_remove(timer_entry);
}

module_init(timer_init);
module_exit(timer_exit);
