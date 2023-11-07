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
//static DEFINE_MUTEX(elev_mutex);


enum state {OFFLINE, IDLE, LOADING, UP, DOWN};
enum weight{FRESHMAN = 100, SOPHMORE = 150, JUNIOR = 200, SENIOR = 250};
typedef struct thread_param
{
	int id;
	int cnt;
	struct task_struct *kthread;
	struct mutex mutex1;
	struct mutex mutex2;

};
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
	int num_passengers;
	struct list_head list;	//people waiting on a floor
};

struct elev
{
	int current_floor;
	int current_weight;
	int current_passengers;
	int running;
	int stopped;
	enum state status;
	struct list_head list;	//people on the elevator
	struct floor floor[6];
};
struct thread_param thread;
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
static struct passenger passenger;
static struct elev elev;

int start_elevator(void);
int issue_request(int start_floor, int destination_floor, int type);
int stop_elevator(void);
int travel(int curfl, int destfl);

extern int (*STUB_start_elevator)(void);
extern int (*STUB_issue_request)(int,int,int);
extern int (*STUB_stop_elevator)(void);

int start_elevator(void) {
	if(elev.running == 1)
	{
		return 1;
	} else
	{
		elev.running = 1;
		elev.stopped = 0;
	}
	return 0;
}

int issue_request(int start_floor, int destination_floor, int type) {

		
	if ((mutex_lock_interruptible(&thread.mutex1) == 0) &&(elev.current_passengers < 5))
	{
		
		elev.current_passengers++;
		elev.floor[elev.current_floor].num_passengers--;
		struct passenger *headcopy = list_first_entry(&elev.floor[elev.current_floor].list, struct passenger, list);
		list_del(&headcopy->list);
		list_add_tail(&headcopy->list, &elev.list);
		
	}
	mutex_unlock(&thread.mutex1);

	return 0;
}

int stop_elevator(void) {

	
	if ((mutex_lock_interruptible(&thread.mutex2) == 0) &&(elev.current_passengers > 0))
	{
		
		elev.current_passengers--;
		struct passenger *headcopy = list_first_entry(&elev.list, struct passenger, list);
		list_del(&headcopy->list);
	}
	mutex_unlock(&thread.mutex2);
	serviced++;

	return 0;
}
//need to setup in init and set to NULL in exit


//thread init

//next elevator move
int travel(int curfl, int destfl)
{
	if(curfl < destfl)
	{
		ssleep(2);
		elev.status = UP;
		elev.running = 1;
		return(curfl++);
	} else if(curfl > destfl)
	{
		ssleep(2);
		elev.status = DOWN;
		elev.running = 1;
		return(curfl--);
	} else
	{
		elev.status = IDLE;
		elev.stopped = 1;
		return(destfl);
	}
	
}

int elev_thread_run(void *data)
{
	while(!kthread_should_stop())
	{
		switch(elev.status)
		{
			case LOADING:
			{
				ssleep(1);
				stop_elevator();
				struct passenger *headcopy = list_first_entry(&elev.list, struct passenger, list);
				issue_request(headcopy->current_floor, headcopy->destination_floor, headcopy->year);
				elev.current_floor = travel(elev.current_floor, headcopy->destination_floor);
				
				
			} case UP:
			case DOWN:
			{
				if(!list_empty(&elev.list))
				{
					struct passenger *headcopy = list_first_entry(&elev.floor[elev.current_floor].list, struct passenger, list);
					if((headcopy->year+elev.current_weight) <= 750)
					{
						elev.status = LOADING;
					}
					
				} else
				{
					struct passenger *headcopy = list_first_entry(&elev.list, struct passenger, list);
					elev.current_floor = travel(elev.current_floor, headcopy->destination_floor);
					
				}
				
			} case IDLE:
			{
				if(!list_empty(&elev.list))
				{
					struct passenger *headcopy = list_first_entry(&elev.floor[elev.current_floor].list, struct passenger, list);
					if((headcopy->year+elev.current_weight) <= 750)
					{
						elev.status = LOADING;
					} else
					{
						struct passenger *headcopy = list_first_entry(&elev.list, struct passenger, list);
						elev.current_floor = travel(elev.current_floor, headcopy->destination_floor);
					}
				} default :
				{
					elev.status = OFFLINE;
					break;
				}
			}
		}
	}
	return 0;
}
//filing in passengers
static ssize_t line_up(struct file *file, char __user *ubuf, size_t count, loff_t *ppos)
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
		printk(KERN_INFO "Error: Invalid Year");
		return -ENOMEM;
	}
	
	if((cur>5)||(cur<0))
	{
		printk(KERN_INFO "Error: Invalid Starting Floor");
		return -ENOMEM;
	}
	if((des>5)||(des<0))
	{
		printk(KERN_INFO "Error: Invalid Destination Floor");
		return -ENOMEM;
	}
	
	struct passenger new_passenger;
	new_passenger.year = ye;
	new_passenger.current_floor = cur;
	new_passenger.destination_floor = des;
	/*new_passenger->id = next_passenger_id++;
	if(next_passenger_id > 'Z')
	{
		next_passenger_id = 'A';
	}*/
	INIT_LIST_HEAD(&new_passenger.list);
	list_add_tail(&new_passenger.list, &passenger.list);
	list_add_tail(&new_passenger.list, &elev.floor[new_passenger.current_floor].list);
	waiting++;
	
}
void thread_init_param(struct thread_param *param)
{
	static int id = 1;
	param->id = id++;
	param->cnt = 0;
	mutex_init(&param->mutex1);
	mutex_init(&param->mutex2);
	param->kthread = kthread_run(elev_thread_run, param, "thread example %d", param->id);
}
//part 3(5) additions (not done)
static ssize_t elevator_read(struct file *file, char __user *ubuf, size_t count, loff_t *ppos) {
	if ((mutex_lock_interruptible(&thread.mutex1) == 0) && ((mutex_lock_interruptible(&thread.mutex2) == 0)
   	{
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
		    (i == elev.current_floor ? '*' : ' '), i + 1, elev.floor[i].num_passengers);
		struct passenger *floor_pass;
		list_for_each_entry(floor_pass, &elev.floor[i].list, list) {
		    len += snprintf(ptr + len, 4096 - len, "%c%d ", passenger_type_to_char(floor_pass->year), floor_pass->destination_floor);
		}
		len += snprintf(ptr + len, 4096 - len, "\n");
	    }
		
	    len += snprintf(ptr + len, 4096 - len, "Number of passengers: %d\n", total_passengers);

	    // Number of passengers waiting on each floor
	    len += snprintf(ptr + len, 4096 - len, "Number of passengers waiting: %d\n", waiting);

	    // Number of passengers serviced
	    len += snprintf(ptr + len, 4096 - len, "Number of passengers waiting: %d\n", serviced);

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
    mutex_unlock(&thread.mutex1);
    mutex_unlock(&thread.mutex2);
}

static const struct proc_ops elevator_fops = 
{
	.proc_read = elevator_read,
};
static int __init elevator_init(void)
{
	STUB_start_elevator = start_elevator;
	STUB_issue_request = issue_request;
	STUB_stop_elevator = stop_elevator;
	mutex_init(&thread.mutex1);
	mutex_init(&thread.mutex2);
	/*elev.current_floor = 1;
	elev.current_weight = 0;
	elev.current_passengers = 0;
	elev.status = IDLE;*/
	INIT_LIST_HEAD(&elev.list);
	for(int i=0; i < 6; i++)
	{
		INIT_LIST_HEAD(&elev.floor[i].list);
	}
	
	INIT_LIST_HEAD(&passenger.list);
	thread_init_param(&thread);
	if(IS_ERR(thread.kthread))
	{
		printk(KERN_WARNING "Error creating thread");
		return PTR_ERR(thread.kthread);
	}
	elevator_entry = proc_create(ENTRY_NAME, PERMS, PARENT, &elevator_fops);
	if(!elevator_entry) {
		return -ENOMEM;
	}
	return 0;
}
static void __exit elevator_exit(void)
{
	STUB_start_elevator = NULL;
	STUB_issue_request = NULL;
	STUB_stop_elevator = NULL;
	kthread_stop(thread.kthread);
	mutex_destroy(&thread.mutex1);
	mutex_destroy(&thread.mutex2);
	struct passenger *passenger, *next;
	list_for_each_entry_safe(passenger, next, &elev.list, list)
	{
		list_del(&passenger->list);
		kfree(passenger);
	}
	printk(KERN_INFO "Elevator module unloaded\n");
	//remove_proc_entry(ENTRY_NAME, PARENT);//was this meant to be for pt3(5)?
	//pt3(5) additions
	remove_proc_entry("elevator", NULL);
}

module_init(elevator_init);
module_exit(elevator_exit);
