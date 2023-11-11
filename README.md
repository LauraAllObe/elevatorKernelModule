# Dorm Elevator
This project is designed to provide an in-depth understanding of system calls, kernel programming, concurrency, synchronization, and 
elevator scheduling algorithms through a multi-part approach. Part 1 introduces system calls in a C program, teaching their integration 
and usage verified via the "strace" tool. Part 2 advances into kernel programming with the development of a "my_timer" module that 
utilizes ktime_get_real_ts64() to manage current and elapsed time, demonstrating how kernel modules function and interact with 
kernel functions. The final part, Part 3, is a complex task involving the creation of an elevator scheduling algorithm within a 
kernel module, focusing on concurrency and synchronization challenges. Each part builds upon the previous, cumulatively enhancing 
understanding and skills in kernel operations, system calls, and efficient scheduling, crucial for robust software system development 
in operating systems and low-level programming.

## Group Members
- **Jeyma Rodriguez**: jdr21d@fsu.edu
- **Autumn Harp**: aom21a@fsu.edu
- **Laura Obermaier**: lao21@fsu.edu
## Division of Labor

### Part 1: System Call Tracing
- **Responsibilities**: Creates two distinct programs to demonstrate and analyze system call operations. The first, named "empty",
- serves as a baseline and contains no operational system calls. The second, "part1", builds upon the empty program by incorporating
- four predefined system calls. The task includes utilizing the strace utility to trace the system calls executed by both programs
- and comparing the results to identify the additional calls in part1.
- **Assigned to**: Jeyma Rodriguez

### Part 2: Timer Kernel Module
- **Responsibilities**: Part 2 of the project advances into deeper aspects of kernel programming by developing a kernel module named
-  "my_timer." This module leverages the ktime_get_real_ts64() function to fetch and store the current time. It features a proc entry,
-  enabling users to read both the current time and the time elapsed since the last call. This phase of the project provides valuable
-  insights into kernel module operations, interactions with kernel functions, and the utilization of proc interfaces for effective
-   communication within the kernel space.
- **Assigned to**: Laura Obermaier

### Part 3a: Adding System Calls
- **Responsibilities**: This project segment enhances the Linux kernel by introducing custom system calls for an elevator simulation,
- involving kernel source modification in the /usr/src/ directory. Three system calls - start_elevator(), issue_request(), and stop_elevator() -
- are implemented with unique numbers (548, 549, 550) to initiate elevator service, create passenger requests, and gracefully deactivate the
- elevator, respectively. The implementation requires modifying kernel files like syscalls.c, syscall_64.tbl, and syscalls.h, and creating a
- Makefile, offering practical experience in kernel programming and system call implementation.
- **Assigned to**: Laura Obermaier

### Part 3b: Kernel Compilation
- **Responsibilities**: The process of integrating system calls involves compiling the modified kernel. This compilation is done using a series
- of commands: initially configuring the kernel with make menuconfig, followed by the parallel compilation process make -j$(nproc), and then
- installing the compiled modules and the kernel itself with sudo make modules_install and sudo make install. A system reboot was necessary to
- apply these changes. Post-reboot, the successful installation of the new kernel was verified using uname -r in the terminal, which reflected
- the updated kernel version.
- **Assigned to**: Laura Obermaier

### Part 3c: Threads
- **Responsibilities**: In this component of the project, a kernel thread (kthread) is employed to manage the movement of the elevator
- in the elevator simulation. The use of a kthread is integral to controlling the elevator's operational dynamics, such as navigating
- between floors and handling passenger boarding and disembarking. This approach leverages the kernel's threading capabilities to ensure
- smooth and efficient elevator operations within the Linux kernel environment, aligning with the project's goal of simulating an elevator
- system with specific passenger and weight constraints.
- **Assigned to**: Autumn Harp

### Part 3d: Linked List
- **Responsibilities**: In this segment of the project, linked lists are utilized to efficiently manage passengers associated with the
- elevator simulation. Specifically, they are used to maintain a list of passengers waiting on each floor and those currently in the elevator.
- This approach allows for real-time tracking of the total number of waiting passengers, as well as the total number of passengers serviced.
- Furthermore, for each waiting passenger, the linked list stores two characters - one indicating the passenger type (freshman, sophomore, etc.)
- and the other specifying the destination floor. This structure enables effective and flexible management of passenger data, crucial for the
- implementation of the elevator's scheduling and movement logic within the kernel module.
- **Assigned to**: Autumn Harp

### Part 3e: Mutexes
- **Responsibilities**: In this part of the project, a mutex (mutual exclusion lock) is employed to manage access to shared data between the
- floors and the elevator in the simulation. The mutex plays a crucial role in ensuring data integrity and consistency by preventing concurrent
- access to shared resources, such as the passenger list or elevator status. When either the elevator or a floor needs to update or read shared
- information, the mutex is locked to provide exclusive access to that resource, thereby avoiding potential conflicts or data corruption that could
- arise from simultaneous access by multiple threads.
- **Assigned to**: Autumn Harp

### Part 3f: Scheduling Algorithm
- **Responsibilities**: This part implements a specialized scheduling algorithm for a dormitory elevator as a Linux kernel module. It focuses
- on efficient scheduling, concurrency management, and synchronization within the kernel environment. Key functionalities include starting and
- stopping the elevator, issuing floor requests, and a /proc/elevator entry for real-time status monitoring. The project addresses unique challenges
- of operating in a kernel space, such as memory management and error handling, while optimizing the elevator's operations to minimize wait times and
- improve overall efficiency in a dormitory setting.
- **Assigned to**: Autumn Harp

## File Listing
```
elevator/
├── Makefile
├── part1/
│   ├── empty.c
│   ├── empty.trace
│   ├── empty.rs
│   ├── part1.c
│   ├── part1.trace
│   ├── part1.rs
│   └── Makefile
├── part2/
│   ├── src/
|   |   └──my_timer.c
│   └── Makefile
├── part3/
│   ├── src/
|   |   └──elevator.c
│   ├── tests/
|   |   ├── system-call-test/
|   |   |   ├── Makefile
|   |   |   ├── README.md
|   |   |   ├── test-syscalls.h
|   |   |   └── test-syscalls.c
|   |   └── elevator-test/
|   |       ├── Makefile
|   |       ├── README.md
|   |       ├── wrappers.h
|   |       ├── producer.c
|   |       └── consumer.c
│   ├── Makefile
│   └── syscalls.c
├── Makefile
└── README.md

```
# How to Compile & Execute

### Requirements
- **Compiler**: `gcc`

## Part 1

### Compilation
make (in pt1 directory)

This will build the executable part1 and empty in the project-2-group-23/part1 directory.

### Execution
These commands compile the empty and part1 programs and then use strace to trace the system calls each program makes, 
outputting the results to empty.trace and part1.trace, respectively.

## Part 2

### Compilation
make (in pt2 directory)

This will build the executable in project-2-group-23/part2 directory
### Execution
sudo insmod src/my_timer.ko
cat /proc/my_timer

This will generate a message that tells if the system calls installed or not

## Part 3

### Compilation
make (in pt3 directory)

This will build the executable in project-2-group-23/part3 directory
### Execution
sudo insmod src/elevator.ko
sudo insmod syscalls.ko
./test_syscalls                     in part3/test/system-call-test directory
./producer [number_of_passengers]   in part3/test/elevator-test directory
./consumer --start                  in part3/test/elevator-test directory
./consumer --stop                   in part3/test/elevator-test directory

This will creates passengers and issues requests to the elevator and calls the start_elevator() or the 
stop_elevator() system call respectively.
note: For part 2 and part 3, makefile produce a kernel module, my_timer.ko and elevator.ko respectively

## Bugs
None that we are aware of.

## Considerations
.ko files are produced in the same directory as their .c files. For example,
for elevator.c located at part3/src/elevator.c, the .ko file will be prodiced
at part3/src/elevator.ko. No guarantee project directory makefile works.
