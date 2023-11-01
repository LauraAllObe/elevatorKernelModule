/*#include <string.h>
#include <unistd.h>
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

MODULE_LICENSE("GPL");
MODULE_AUTHOR("cop4610g23");
MODULE_DESCRIPTION("kernel module for pt3/elevator");

#define ENTRY_NAME "elevator"
#define PERMS 0644
#define PARENT NULL

#define BUFFER_SIZE 5


int waiting = 0;
int serviced = 0;
struct task_struct *onboarding; //boarding elevator
struct task_struct *offboarding; //departing elevator
static DEFINE_MUTEX(buffer_mutex);

enum state {OFFLINE, IDLE, LOADING, UP, DOWN};
enum weight{FRESHMAN = 100, SOPHMORE = 150, JUNIOR = 200, SENIOR = 250};

struct passenger
{
	char id;
	enum weight year;
	int current_floor;
	int destination_floor;
	struct list_head list;	//passengers
};

struct floor
{
	int num_passengers;
	struct list_head list;	//people waiting on a floor
};

struct elev
{
	int current_floor;
	int current_weight;
	int current_passengers;
	enum state status;
	struct list_head list;	//people on the elevator
	struct floor floors[6];	
	struct task_struct *kthread;
};

//for pt3(5)
char passenger_type_to_char(enum weight year) {
    switch (year) {
        case FRESHMAN:
            return 'F';
        case SOPHOMORE:
            return 'O';
        case JUNIOR:
            return 'J';
        case SENIOR:
            return 'S';
        default:
            return '?';
    }
}

static struct proc_dir_entry *proc_entry;
static struct elev elev;
static char next_passenger_id = 'A';
//manage passenger arrivals
static int passarr(void *data)
{
	int i = 0;
	while(!kthread_should_stop())
	{
		mutex_lock(&buffer_mutex);
		if (elev->current_passengers < 5)
		{
			
			elev->current_passengers++;
			elev->floor[elev->current_floor]->num_passengers--;
			list_add_tail(&list_first_entry(&elev->floor[elev->current_floor]->list, elev->list);
			list_del(list_first_entry(&elev->floor[elev->current_floor]->list, struct passenger, list);
		}
		mutex_unlock(&buffer_mutex);
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
		if (elev->current_passengers > 0)
		{
			
			elev->current_passengers--;
			list_del(list_first_entry(&elev->list, struct passenger, list);
		}
		mutex_unlock(&buffer_mutex);
	}
	
	return 0;
}
//next elevator move
int travel(int curfl, int destfl)
{
	if(curfl < destfl)
	{
		ssleep(2);
		elev->status = UP;
		return(curfl++);
	} else if(curfl > destfl)
	{
		ssleep(2);
		elev->status = DOWN;
		return(curfl--);
	} else
	{
		elev->status = IDLE;
		serviced++;
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
	char buf[256];
	int len = 0;
	if(*ppos > 0 || count < 256)
	{
		return 0;
	}
	struct passenger *new_passenger = kmalloc(sizeof(struct passenger), GFP_KERNEL);
	if(!new_passenger)
	{
		printk(KERN_INFO "Error: Could not allocate memory for new passenger.\n");
		return -ENOMEM;
	}
	
	new_passenger->id = next_passenger_id++;
	if(next_passenger_id > 'Z')
	{
		next_passenger_id = 'A';
	}
	list_add_tail(&new_passenger, &customer->list);
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
    len += snprintf(ptr + len, 4096 - len, "Number of passengers: %d\n", total_passengers);

    // Number of passengers waiting on each floor
    len += snprintf(ptr + len, 4096 - len, "Number of passengers waiting: %d\n", waiting);
    for (int i = 0; i < 6; i++) {
        len += snprintf(ptr + len, 4096 - len, "[%c] Floor %d: %d ", 
            (i == elev.current_floor ? '*' : ' '), i + 1, elev.floors[i].num_passengers);
        struct passenger *floor_pass;
        list_for_each_entry(floor_pass, &elev.floors[i].list, list) {
            len += snprintf(ptr + len, 4096 - len, "%c%d ", passenger_type_to_char(floor_pass->year), floor_pass->destination_floor);
        }
        len += snprintf(ptr + len, 4096 - len, "\n");
    }

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
	INIT_LIST_HEAD(&elev.floors[0].list);
	proc_entry = proc_create(ENTRY_NAME, PERMS, PARENT, &procfile_pops);
	return 0;
}
static int __init elevator_exit(void)
{
	struct passenger *passenger, *next;
	list_for_each_entry_safe(passenger, next, &elev.floors[0].list, list)
	{
		list_del(&passenger->list);
		kfree(passenger);
	}
	remove_proc_entry(ENTRY_NAME, PARENT);
}

module_init(elevator_init);
module_exit(elevator_exit);
