#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("cop4610t");
MODULE_DESCRIPTION("A simple Linux kernel module to check systemcalls.");
MODULE_VERSION("0.1");

int start_elevator(void);                                                           // starts the elevator to pick up and drop off passengers
int issue_request(int start_floor, int destination_floor, int type);                // add passengers requests to specific floors
int stop_elevator(void);                                                            // stops the elevator

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

static int __init syscheck_init(void) {
    STUB_start_elevator = start_elevator;
	STUB_issue_request = issue_request;
	STUB_stop_elevator = stop_elevator;
    return 0;  // Return 0 to indicate successful loading
}

static void __exit syscheck_exit(void) {
    STUB_start_elevator = NULL;
	STUB_issue_request = NULL;
	STUB_stop_elevator = NULL;
}

module_init(syscheck_init);  // Specify the initialization function
module_exit(syscheck_exit);  // Specify the exit/cleanup function
