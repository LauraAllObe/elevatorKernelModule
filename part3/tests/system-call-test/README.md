## How to Use

Run ```make``` to generate an the kernel module ```syscheck``` and the executable ```test-syscalls```.

Run the following commands:
```
sudo insmod syscheck.ko
make run
```
You should get a result that tells you if you have the system calls correctly installed in your kernel module. If they are not installed, you will have to recompile your kernel correctly.

After checking, you can remove the kernel module that was used for testing.
```
sudo rmmod syscheck.ko
```
