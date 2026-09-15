# Project further roadmap

## Kernel space mandatory features that are not implemented yet

### Virtual Memory Management and Paging
Without VMM, userspace is almost impossible (well, there are other techniques, but paging is the RISC-V-native implementation). The following functionality needs to be implemented:
[ ] Utilities to create and remove Page Tables, requesting them from Physical Allocator
[ ] 

### Interrupt handling 
Interrupts are the key to communication between kernel and devices or between kernel and user processes.

### Mode switching (Machine -> User and vice versa in RISC-V)
The "switch button" between lowest and highest privileges of the hardware thread.

### Scheduler and `struct proc`
> [!TODO]
> 

## Not as important as these above

Although these are super useful and an adequate user space is impossible without them, a minimal kernel can exist without them on its early stages. It means there is no need to work on them right now.

### Users
Make it possible for a computer to belong to multiple users; each with their `$HOME` directory, password, and so forth.

### Loading ELF or custom format executables

## Userspace, optional

### Interactive shell
Simple commands, such as `uname`, `help`, then optionally add arguments passing: `echo`, `cat` (and maybe `grep` and `vi`!!!).

### Graphic for the said shell
Instead of using UART for the shell, configure QEMU to add a primitive graphic display.
