# Project development roadmap

The goal is to grow the current bare-metal RISC-V kernel into a small,
testable system that can run isolated user programs. Work through the milestones
in order; paging, traps, and privilege transitions are prerequisites for
preemptive userspace.

## Current foundations

These pieces are present, but need continued integration and testing:

- Device Tree parsing for RAM and CLINT discovery.
- A bitmap-based physical page allocator.
- Initial machine- and supervisor-mode trap diagnostics and an M-to-S-mode
  transition helper.

## 1. Make boot and platform support reliable

- [ ] Complete the QEMU `virt` boot path: initialize the UART from the Device
  Tree, initialize memory and devices in a documented order, and enter the
  intended kernel mode instead of shutting down after initialization.
- [ ] Check Device Tree parsing and memory-map assumptions against the actual
  platform; reject malformed or unsupported layouts with useful diagnostics.
- [ ] Define which hart performs global initialization and how other harts are
  parked or brought online. Keep shared initialization from running twice.
- [ ] Add a repeatable QEMU smoke test that checks boot reaches a known
  milestone and reports failures.

## 2. Finish traps and interrupt handling

- [ ] Add assembly trap entry/return paths that preserve the interrupted
  register state before calling C handlers.
- [ ] Decode synchronous exceptions and interrupts, dispatch by cause, and
  return from recoverable traps; keep fatal traps informative.
- [ ] Replace blanket interrupt delegation with an explicit M-mode/S-mode
  policy. Verify the privilege level and enable bits for each supported source.
- [ ] Deliver timer events to the scheduler reliably, including rearming the
  timer and handling timer-counter access on RV32.
- [ ] Test exception, timer, and interrupt-return paths in QEMU.

## 3. Implement Sv32 virtual memory

- [ ] Define and document the RV32 virtual address layout and the kernel/user
  boundary. Reserve regions for kernel mappings, user code/data, heap, and
  stack.
- [ ] Implement page-table creation, destruction, walking, and 4 KiB
  map/unmap operations with validated PTE permissions.
- [ ] Connect virtual-page allocation to the physical page allocator; support
  freeing pages and page-table pages, and clear newly allocated pages.
- [ ] Create the kernel page table and switch to it safely. Keep all code,
  stacks, and device mappings needed across the `satp` switch reachable.
- [ ] Add the required `sfence.vma` operations and define how address-space
  identifiers are handled (or keep them disabled initially).
- [ ] Test alignment, invalid addresses, permissions, allocation exhaustion,
  and map/unmap behavior before enabling paging during normal boot.

## 4. Establish a safe user-mode boundary

- [ ] Set PMP permissions deliberately so S-mode can access kernel memory while
  U-mode is restricted to its own mappings.
- [ ] Enter a minimal user program and handle `ecall` system calls and user
  exceptions without losing or corrupting the saved user state.
- [ ] Add a small syscall ABI, starting with console write, process exit, yield,
  and sleep.
- [ ] Test that user code cannot read or write kernel-only pages and that faults
  are reported and contained.

## 5. Add processes and scheduling

- [ ] Define `struct proc` with an ID, lifecycle state, saved context, kernel
  stack, user entry/stack, and page-table root.
- [ ] Separate the trap frame (state required to resume a trap) from the
  scheduler context (state required to switch kernel execution).
- [ ] Implement a simple runnable queue and cooperative yield, then add
  timer-driven preemption after trap and interrupt return paths are reliable.
- [ ] Define process creation, sleep/wakeup, exit, and cleanup behavior,
  including reclaiming address spaces and physical pages.
- [ ] Do not store a hardware TLB-entry index per process; translations are
  hardware-managed. Track an ASID only if the implementation later enables
  ASIDs.
- [ ] Test switching between at least two user tasks and verify that their
  memory spaces remain isolated.

## 6. Load and run user programs

- [ ] Implement a bounded ELF32 RISC-V loader: validate headers and segments,
  map segments with appropriate permissions, zero BSS, and reject malformed
  files.
- [ ] Start with a built-in test program, then add a simple way to provide
  executable images to the kernel.
- [ ] Add a minimal init process and report process exit status.

## 7. Improve usability and expand the system

- [ ] Provide a serial interactive shell with a few built-in commands such as
  `help`, `uname`, and `echo`.
- [ ] Add storage and a simple filesystem before pursuing persistent user
  accounts, home directories, or password handling.
- [ ] Consider a graphical console only after the serial shell and basic
  userspace are stable; it will require a display device/driver and a QEMU
  configuration that exposes it.

## Ongoing quality work

- [ ] Add focused tests for bitmap allocation, physical/virtual memory
  boundaries, page-table permissions, and scheduler state transitions.
- [ ] Document supported QEMU settings, build/run commands, and known
  limitations in the README.
- [ ] Keep architecture-specific assumptions explicit (initially RV32 on QEMU
  `virt`) and make unsupported configurations fail clearly.
