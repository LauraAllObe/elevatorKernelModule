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
	struct mutex mutex1; //handles matters pertaining to adding to floors
	struct mutex mutex2; //handles matters pertaining to adding to elevators

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
	printk(KERN_INFO "SE");
	if(elev.running == 1)
	{
		return 1;
	} else
	{
		elev.running = 1;
		elev.stopped = 0;
		elev.status = IDLE;
	}
	return 0;
}

int issue_request(int start_floor, int destination_floor, int type) {
	printk(KERN_INFO "IR");
	/*if(!elev.running)
	{
		return 1;
	}*/
    if(type==0)
    {
    	type = 100;
	} else if(type==1)
    {
    	type = 150;
	}else if(type==2)
    {
    	type = 200;
	}else if(type==3)
    {
    	type = 250;
	} else
	{
		printk(KERN_INFO "Error: Invalid Year");
		return -ENOMEM;
	}
	
	if((start_floor>6)||(start_floor<1))
	{
		printk(KERN_INFO "Error: Invalid Starting Floor");
		return -ENOMEM;
	}
	if((destination_floor>6)||(destination_floor<1))
	{
		printk(KERN_INFO "Error: Invalid Destination Floor");
		return -ENOMEM;
	}
	if(mutex_lock_interruptible(&thread.mutex1) == 0)
	{
		struct passenger *new_passenger = kmalloc(sizeof(struct passenger), GFP_KERNEL);
		if (!new_passenger)
	    {
	        mutex_unlock(&thread.mutex1);
	        return -ENOMEM;
	    }
		new_passenger->year = type;
		new_passenger->current_floor = start_floor-1;
		new_passenger->destination_floor = destination_floor-1;
		/*new_passenger->id = next_passenger_id++;
		if(next_passenger_id > 'Z')
		{
			next_passenger_id = 'A';
		}*/
		INIT_LIST_HEAD(&new_passenger->list);
		//list_add_tail(&new_passenger.list, &passenger.list);
		list_add_tail(&new_passenger->list, &elev.floor[new_passenger->current_floor].list);
		elev.floor[new_passenger->current_floor].num_passengers++;
		waiting++;
		mutex_unlock(&thread.mutex1);
	}
	
	return 0;
}

int stop_elevator(void) {
		printk(KERN_INFO "STOPE");
	if((!elev.running)||(elev.stopped))
	{
		return 1;
	}
	if((elev.status == IDLE)&&(list_empty(&elev.list)))
	{
		elev.status = OFFLINE;
	}
	elev.running = 0;
	elev.stopped = 1;
	return 0;
}
//need to setup in init and set to NULL in exit


//thread init

//next elevator move
int travel(int curfl, int destfl)
{
		printk(KERN_INFO "T");
	if(curfl < destfl)
	{
		printk(KERN_INFO "UP");
		ssleep(2);
		elev.status = UP;
		return(curfl+1);
	} else if(curfl > destfl)
	{
		printk(KERN_INFO "DOWN");
		ssleep(2);
		elev.status = DOWN;
		return(curfl-1);
	} else
	{
		printk(KERN_INFO "IDLE");
		elev.status = IDLE;
		return(destfl);
	}
	
}
int loading(void) {
		printk(KERN_INFO "L");
	struct list_head *temp1;
	struct list_head *temp2;
	struct list_head temp_list;
	INIT_LIST_HEAD(&temp_list);
	if (mutex_lock_interruptible(&thread.mutex2) == 0)
	{
		list_for_each_safe(temp1, temp2, &elev.floor[elev.current_floor].list)
		{
			struct passenger *headcopy = list_first_entry(&elev.floor[elev.current_floor].list, struct passenger, list);
			if(((headcopy->year+elev.current_weight) <= 750)&&(elev.current_passengers < 5))
			{
				elev.current_passengers++;
				elev.floor[elev.current_floor].num_passengers--;
				list_del(&headcopy->list);
				list_add_tail(&headcopy->list, &elev.list);
				elev.current_weight += headcopy->year;
				waiting--;
			} else
			{
				break;
			}
		}
				
		mutex_unlock(&thread.mutex2);
	}
	
	return 0; 
}

int unloading(void) {
		printk(KERN_INFO "U");
	struct list_head *temp1;
	struct list_head *temp2;
	struct list_head temp_list;
	struct passenger *p;
	INIT_LIST_HEAD(&temp_list);
	
	if (mutex_lock_interruptible(&thread.mutex2) == 0)
	{
		list_for_each_safe(temp1, temp2, &elev.list)
		{
			p = list_entry(temp1, struct passenger, list);
			if(p->destination_floor == elev.current_floor)
			{
				elev.current_passengers--;
				elev.current_weight -= p->year;
				list_move_tail(temp1, &temp_list);
				serviced++;
			}
		}
			
				
		mutex_unlock(&thread.mutex2);
	}
	
	list_for_each_safe(temp1, temp2, &temp_list)
	{
		p = list_entry(temp1, struct passenger, list);
		list_del(temp1);
		kfree(p);
		
	}
	if((elev.stopped)&&(list_empty(&elev.list)))
	{
		elev.status = OFFLINE;
	}
	
	return 0; 
}

int elev_thread_run(void *data)
{
		printk(KERN_INFO "ETR");
	while(!kthread_should_stop())
	{
		if((elev.running)||(!list_empty(&elev.list)))
		{
			switch(elev.status)
			{
				case LOADING:
				{	printk(KERN_INFO "LOADING");
					ssleep(1);
					loading();
					unloading();
					if(!list_empty(&elev.list))
					{
						struct passenger *headcopy = list_first_entry(&elev.list, struct passenger, list);
						elev.current_floor = travel(elev.current_floor, headcopy->destination_floor);	
					} else
					{
						elev.status = IDLE;
					}
					break;
					
				} case UP:
				case DOWN:
				{
					
					if(elev.floor[elev.current_floor].num_passengers > 0)
					{
						elev.status = LOADING;
						
					} else if(!list_empty(&elev.list))
					{
						struct passenger *headcopy = list_first_entry(&elev.list, struct passenger, list);
						printk(KERN_INFO "TRAVEL FROM UP/DOWN");
						elev.current_floor = travel(elev.current_floor, headcopy->destination_floor);
						
					} else
					{
						if(elev.status == DOWN)
						{
							printk(KERN_INFO "TRAVEL DOWN FROM UP/DOWN");
							elev.current_floor = travel(elev.current_floor, elev.current_floor - 1);
						} else
						{
							printk(KERN_INFO "TRAVEL UP FROM UP/DOWN");
							elev.current_floor = travel(elev.current_floor, elev.current_floor + 1);
						}
					}
					break;
				} case IDLE:
				{
					
					if((elev.floor[elev.current_floor].num_passengers > 0)||(!list_empty(&elev.list)))
					{
						elev.status = LOADING;
					} else if(waiting>=1)
					{
						printk(KERN_INFO "CHECKING SPTP");
						int c = elev.current_floor;
						int sptp = 6; //shortest path to passenger
						int ud = 0; //0 = down, 1 = up
						for(int i = 0; i < 6; i++)
						{
							if(elev.floor[i].num_passengers > 0)
							{
								if(c > i)
								{
									if((c-i) < sptp)
									{
										sptp = c-i;
										ud = 0;
									}
								} else
								{
									if((i-c) < sptp)
									{
										sptp = i-c;
										ud = 1;
									}
								}
							}
						}
						if(ud == 0)
						{
							printk(KERN_INFO "TRAVEL DOWN FROM SPTP");
							elev.current_floor = travel(elev.current_floor, elev.current_floor - 1);
						} else
						{
							printk(KERN_INFO "TRAVEL UP FROM SPTP");
							elev.current_floor = travel(elev.current_floor, elev.current_floor + 1);
						}
						
						
					} else
					{
						printk(KERN_INFO "REMAIN IDLE");
						ssleep(1);	
					}
					break;
		
				}
				case OFFLINE:
					{
						printk(KERN_INFO "OFFLINE FROM SWITCH");
						elev.running = 0;
						elev.stopped = 1;
						ssleep(1);
						break;
					}
				default :
				{
					printk(KERN_INFO "DEFAULT TRIGGERED");
					elev.status = OFFLINE;
					break;
				}
			}
		} else
		{
			printk(KERN_INFO "OFFLINE");
			ssleep(1);
		}
		
		

	}
	return 0;
}
//filing in passengers

void thread_init_param(struct thread_param *param)
{	printk(KERN_INFO "TIP");
	static int id = 1;
	param->id = id++;
	param->cnt = 0;
	mutex_init(&param->mutex1);
	mutex_init(&param->mutex2);
	param->kthread = kthread_run(elev_thread_run, param, "thread example %d", param->id);
}
//part 3(5) additions (not done)
static ssize_t elevator_read(struct file *file, char __user *ubuf, size_t count, loff_t *ppos) {
		printk(KERN_INFO "ER");
	char buf[4096];
	char *ptr = buf;
	int len = 0;
	//if ((mutex_lock_interruptible(&thread.mutex1) == 0) && ((mutex_lock_interruptible(&thread.mutex2) == 0)))
   	//{
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

	    for (int i = 5; i >= 0; i--) {
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
	    len += snprintf(ptr + len, 4096 - len, "Number of passengers serviced: %d\n", serviced);

	    // Copy data to user space
	    if (*ppos > 0 || count < len) {
		return 0;
	    }
	    if (copy_to_user(ubuf, buf, len)) {
		return -EFAULT;
	    }
	    *ppos = len;
	    
	    //mutex_unlock(&thread.mutex1);
	    //mutex_unlock(&thread.mutex2);
    //}
    return len;
}

static const struct proc_ops elevator_fops = 
{
	.proc_read = elevator_read,
};
static int __init elevator_init(void)
{	printk(KERN_INFO "EI");
	STUB_start_elevator = start_elevator;
	STUB_issue_request = issue_request;
	STUB_stop_elevator = stop_elevator;
	mutex_init(&thread.mutex1);
	mutex_init(&thread.mutex2);
	elev.current_floor = 0;
	elev.current_weight = 0;
	elev.current_passengers = 0;
	elev.status = OFFLINE;
	INIT_LIST_HEAD(&elev.list);
	for(int i=0; i < 6; i++)
	{
		elev.floor[i].num_passengers = 0;
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
{	printk(KERN_INFO "EE");
	STUB_start_elevator = NULL;
	STUB_issue_request = NULL;
	STUB_stop_elevator = NULL;
	kthread_stop(thread.kthread);
	mutex_destroy(&thread.mutex1);
	mutex_destroy(&thread.mutex2);
	struct list_head *temp1;
	struct list_head *temp2;
	struct list_head temp_list;
	INIT_LIST_HEAD(&temp_list);
	for(int i = 0; i < 6; i++)
	{
		list_for_each_safe(temp1, temp2, &elev.floor[i].list)
		{
			list_move_tail(temp1, &temp_list);
		}
	}
	
	struct passenger *p;
	list_for_each_safe(temp1, temp2, &temp_list)
	{
		p = list_entry(temp1, struct passenger, list);
		list_del(temp1);
		kfree(p);
		
	}
	list_for_each_safe(temp1, temp2, &elev.list)
	{
		list_move_tail(temp1, &temp_list);
	}
	list_for_each_safe(temp1, temp2, &temp_list)
	{
		p = list_entry(temp1, struct passenger, list);
		list_del(temp1);
		kfree(p);
		
	}
	
	printk(KERN_INFO "Elevator module unloaded\n");
	//remove_proc_entry(ENTRY_NAME, PARENT);//was this meant to be for pt3(5)?
	//pt3(5) additions
	remove_proc_entry("elevator", NULL);
}

module_init(elevator_init);
module_exit(elevator_exit);
