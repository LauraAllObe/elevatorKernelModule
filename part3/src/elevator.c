#include <string.h>
/*#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>*/
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
MODULE_DESCRIPTION("kernel module for pt3/elevator");

#define ENTRY_NAME "elevator"
#define PERMS 0644
#define PARENT NULL
#define BUFFER_SIZE 5


int waiting = 0;
int serviced = 0;
//struct task_struct *onboarding; //boarding elevator
//struct task_struct *offboarding; //departing elevator
//static DEFINE_MUTEX(elev_mutex);

enum state {OFFLINE, IDLE, LOADING, UP, DOWN};
enum weight{FRESHMAN = 100, SOPHMORE = 150, JUNIOR = 200, SENIOR = 250};

struct mutex mutex;
mutex_init(&mutex);

struct passenger
{
	//char id;
	int year;
	int current_floor;
	int destination_floor;
	struct list_head list;	//passengers
};

struct floor
{
	//int num_passengers;
	struct list_head list;	//people waiting on a floor
};

struct elev
{
	int current_floor;
	int current_weight;
	int current_passengers;
	enum state status;
	struct list_head list;	//people on the elevator
	struct floor floor[6];	
	//struct task_struct *kthread;
};
//for pt3(5)
static struct proc_dir_entry *elevator_entry;
char passenger_type_to_char(int year) {
    switch (year) {
        case 100:
            return 'F';
        case 150:
            return 'O';
        case 200:
            return 'J';
        case 250:
            return 'S';
        default:
            return '?';
    }
}
static struct elev elev;
//manage passenger arrivals

static int passarr(void *data)
{
	int i = 0;
	while(!kthread_should_stop())
	{
		
		if ((mutex_lock_interruptible(&arr->mutex) == 0) &&(elev->current_passengers < 5))
		{
			
			elev->current_passengers++;
			elev->floor[elev->current_floor]->num_passengers--;
			list_add_tail(&list_first_entry(&elev->floor[elev->current_floor]->list, elev->list);
			list_del(list_first_entry(&elev->floor[elev->current_floor]->list, struct passenger, list);
		}
		mutex_unlock(&arr->mutex);
	}
	
	return 0;
}
//manage passenger departures
static int passdep(void *data)
{
	int i = 0;
	while(!kthread_should_stop())
	{
		mutex_lock(&buffer_mutex);
		if ((mutex_lock_interruptible(&dep->mutex) == 0) &&(elev->current_passengers > 0))
		{
			
			elev->current_passengers--;
			list_del(list_first_entry(&elev->list, struct passenger, list);
		}
		mutex_unlock(&dep->mutex);
		serviced++;
	}
	
	return 0;
}
//thread init

//next elevator move
int travel(int curfl, int destfl)
{
	if(curfl < destfl)
	{
		ssleep(2);
		elev->status == UP;
		return(curfl++);
	} else if(curfl > destfl)
	{
		ssleep(2);
		elev->status == DOWN;
		return(curfl--);
	} else
	{
		elev->status == IDLE;
		
		return(destfl);
	}
	
}
void elev_state(struct elev * w_thread)
{
	switch(w_thread->status)
	{
		case: LOADING
		{
			ssleep(1);
			passdep();
			passarr();
			current_floor = travel(elev->current_floor, list_first_entry(&elev.list, struct passenger, destination_floor);
			
		} case: UP
		case: DOWN
		{
			if(!list_empty(&elev.list))
			{
				if((list_first_entry(&floor.list, struct passenger, year)+elev->current_weight) <= 750)
				{
					elev->status = LOADING;
				}
			} else
			{
				current_floor = travel(elev->current_floor, list_first_entry(&elev.list, struct passenger, destination_floor);
			}
			
		} case: IDLE
		{
			if(!list_empty(&elev.list))
			{
				if((list_first_entry(&floor.list, struct passenger, year)+elev->current_weight) <= 750)
				{
					elev->status = LOADING;
				} else
				{
					current_floor = travel(elev->current_floor, list_first_entry(&elev.list, struct passenger, destination_floor);
				}
			} else
			{
				elev->status = OFFLINE;
			}
		}
	}
}

//filing in passengers
static ssize_t line_up(struct file *file, char __user *ubuf, size_t count, loff t *ppos
{
	 int ye, cur, des;

    if (copy_from_user(&ye, ubuf, sizeof(int)) != 0 || copy_from_user(&cur, ubuf + sizeof(int), sizeof(int)) != 0 || copy_from_user(&des, ubuf + 2 * sizeof(int), sizeof(int)) != 0) 
	{
        return -EFAULT;
    }
    
    if(ye==0)
    {
    	ye = 100;
	} else if(ye==1)
    {
    	ye = 150;
	}else if(ye==2)
    {
    	ye = 200;
	}else if(ye==3)
    {
    	ye = 250;
	} else
	{
		print(KERN_INFO "Error: Invalid Year");
		return -ENOMEM;
	}
	
	if((cur>5)||(curr<0))
	{
		print(KERN_INFO "Error: Invalid Starting Floor");
		return -ENOMEM;
	}
	if((des>5)||(des<0))
	{
		print(KERN_INFO "Error: Invalid Destination Floor");
		return -ENOMEM;
	}
	
	struct passenger *new_passenger = kmalloc(sizeof(struct passenger), GFP_KERNEL);
	if(!new_passenger)
	{
		printk(KERN_INFO "Error: Could not allocate memory for new passenger.\n");
		return -ENOMEM;
	}
	
	/*new_passenger->id = next_passenger_id++;
	if(next_passenger_id > 'Z')
	{
		next_passenger_id = 'A';
	}*/
	list_add_tail(&new_passenger, &passenger->list);
	list_add_tail(&new_passenger, &elev->floor[new_passenger->currentfloor]->list);
	waiting++;
	
}
//part 3(5) additions (not done)
static ssize_t elevator_read(struct file *file, char __user *ubuf, size_t count, loff_t *ppos) {
    char buf[4096];
    char *ptr = buf;
    int len = 0;

    // Elevator state
    len += snprintf(ptr + len, 4096 - len, "Elevator state: ");
    switch (elev.status) {
        case OFFLINE:
            len += snprintf(ptr + len, 4096 - len, "OFFLINE");
            break;
        case IDLE:
            len += snprintf(ptr + len, 4096 - len, "IDLE");
            break;
        case LOADING:
            len += snprintf(ptr + len, 4096 - len, "LOADING");
            break;
        case UP:
            len += snprintf(ptr + len, 4096 - len, "UP");
            break;
        case DOWN:
            len += snprintf(ptr + len, 4096 - len, "DOWN");
            break;
    }
    len += snprintf(ptr + len, 4096 - len, "\n");

    // Current floor
    len += snprintf(ptr + len, 4096 - len, "Current floor: %d\n", elev.current_floor);

    // Current load
    len += snprintf(ptr + len, 4096 - len, "Current load: %d lbs\n", elev.current_weight);

    // List of passengers in the elevator
    len += snprintf(ptr + len, 4096 - len, "Elevator status: ");
    struct passenger *pass;
    list_for_each_entry(pass, &elev.list, list) {
        len += snprintf(ptr + len, 4096 - len, "%c%d ", passenger_type_to_char(pass->year), pass->destination_floor);
    }
    len += snprintf(ptr + len, 4096 - len, "\n");

    // Number of passengers
    int total_passengers = 0;
    list_for_each_entry(pass, &elev.list, list) {
        total_passengers++;
    }

    for (int i = 0; i < 6; i++) {
        len += snprintf(ptr + len, 4096 - len, "[%c] Floor %d: %d ", 
            (i == elev.current_floor ? '*' : ' '), i + 1, elev.floors[i].num_passengers);
        struct passenger *floor_pass;
        list_for_each_entry(floor_pass, &elev.floors[i].list, list) {
            len += snprintf(ptr + len, 4096 - len, "%c%d ", passenger_type_to_char(floor_pass->year), floor_pass->destination_floor);
        }
        len += snprintf(ptr + len, 4096 - len, "\n");
    }
	
    len += snprintf(ptr + len, 4096 - len, "Number of passengers: %d\n", total_passengers);

    // Number of passengers waiting on each floor
    len += snprintf(ptr + len, 4096 - len, "Number of passengers waiting: %d\n", waiting);

    // Copy data to user space
    if (*ppos > 0 || count < len) {
        return 0;
    }
    if (copy_to_user(ubuf, buf, len)) {
        return -EFAULT;
    }
    *ppos = len;

    return len;
}

static const struct proc_ops elevator_fops = 
{
	.proc_read = elevator_read,
};
static int __init elevator_init(void)
{
	
	arr_thread = kthread_run(passarr, NULL, "passarr");
	dep_thread = kthread_run(passdep, NULL, "passdep");
	elev.current_floor = 1;
	elev.current_weight = 0;
	elev.num_passengers = 0;
	elev.status = IDLE;
	INIT_LIST_HEAD(&elev.passengers);
	elevator_thread = kthread_run(elevator_function, NULL, "elevator_thread");
	if(IS_ERR(elevator_thread)) {
		printk(KERN_ERR "Failed to create the elevator thread\n");
		return PTR_ERR(elevator_thread);
	}
	elevator_entry = proc_create(ENTRY_NAME, PERMS, PARENT, &elevator_fops);
	if(!elevator_entry) {
		return -ENOMEM;
	}
	return 0;
}
static int __init elevator_exit(void)
{
	if(arr_thread)
	{
		kthread_stop(arr_thread);
	}
	if(dep_thread)
	{
		kthread_stop(dep_thread);
	}
	struct passenger *passenger, *next;
	list_for_each_entry_safe(passenger, next, &elev.passengers, list)
	{
		list_del(&passenger->list);
		kfree(passenger);
	}
	print(KERN_INFO "Elevator module unloaded\n");
	//remove_proc_entry(ENTRY_NAME, PARENT);//was this meant to be for pt3(5)?
	//pt3(5) additions
	remove_proc_entry("elevator", NULL);
}

module_init(elevator_init);
module_exit(elevator_exit);
