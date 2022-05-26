#include "kernel/types.h"
#include "kernel/defs.h"
#include "kernel/param.h"
#include "kernel/memlayout.h"
#include "kernel/mmu.h"
#include "kernel/proc.h"
#include "kernel/x86.h"
#include "kernel/syscall.h"

// User code makes a system call with INT T_SYSCALL.
// System call number in %eax.
// Arguments on the stack, from the user call to the C
// library system call function. The saved user %esp points
// to a saved program counter, and then the first argument.

static char* events[N_num]; //kernel memory
static int end = 0;
static int refresh = 0;
static int sign = 0;
static int command = 0;

// Fetch the int at addr from the current process.
int fetchint(uint addr, int *ip) {
  if (addr >= proc->sz || addr + 4 > proc->sz)
    return -1;
  *ip = *(int *)(addr);
  return 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Doesn't actually copy the string - just sets *pp to point at it.
// Returns length of string, not including nul.
int fetchstr(uint addr, char **pp) {
  char *s, *ep;

  if (addr >= proc->sz)
    return -1;
  *pp = (char *)addr;
  ep = (char *)proc->sz;
  for (s = *pp; s < ep; s++)
    if (*s == 0)
      return s - *pp;
  return -1;
}

// Fetch the nth 32-bit system call argument.
int argint(int n, int *ip) { return fetchint(proc->tf->esp + 4 + 4 * n, ip); }

// Fetch the nth word-sized system call argument as a pointer
// to a block of memory of size n bytes.  Check that the pointer
// lies within the process address space.
int argptr(int n, char **pp, int size) {
  int i;

  if (argint(n, &i) < 0)
    return -1;
  if ((uint)i >= proc->sz || (uint)i + size > proc->sz)
    return -1;
  *pp = (char *)i;
  return 0;
}

// Fetch the nth word-sized system call argument as a string pointer.
// Check that the pointer is valid and the string is nul-terminated.
// (There is no shared writable memory, so the string can't change
// between this check and being used by the kernel.)
int argstr(int n, char **pp) {
  int addr;
  if (argint(n, &addr) < 0)
    return -1;
  return fetchstr(addr, pp);
}

extern int sys_chdir(void);
extern int sys_close(void);
extern int sys_dup(void);
extern int sys_exec(void);
extern int sys_exit(void);
extern int sys_fork(void);
extern int sys_fstat(void);
extern int sys_getpid(void);
extern int sys_kill(void);
extern int sys_link(void);
extern int sys_mkdir(void);
extern int sys_mknod(void);
extern int sys_open(void);
extern int sys_pipe(void);
extern int sys_read(void);
extern int sys_sbrk(void);
extern int sys_sleep(void);
extern int sys_unlink(void);
extern int sys_wait(void);
extern int sys_write(void);
extern int sys_uptime(void);
extern int sys_trace(void);
extern int sys_helper(void);

static int (*syscalls[])(void) = {
    [SYS_fork] sys_fork,   [SYS_exit] sys_exit,     [SYS_wait] sys_wait,
    [SYS_pipe] sys_pipe,   [SYS_read] sys_read,     [SYS_kill] sys_kill,
    [SYS_exec] sys_exec,   [SYS_fstat] sys_fstat,   [SYS_chdir] sys_chdir,
    [SYS_dup] sys_dup,     [SYS_getpid] sys_getpid, [SYS_sbrk] sys_sbrk,
    [SYS_sleep] sys_sleep, [SYS_uptime] sys_uptime, [SYS_open] sys_open,
    [SYS_write] sys_write, [SYS_mknod] sys_mknod,   [SYS_unlink] sys_unlink,
    [SYS_link] sys_link,   [SYS_mkdir] sys_mkdir,   [SYS_close] sys_close,
    [SYS_trace] sys_trace, [SYS_helper] sys_helper,
};

static char *syscall_names[] = {
    [SYS_fork]    "fork",  [SYS_exit]    "exit",    [SYS_wait]    "wait",
    [SYS_pipe]    "pipe",  [SYS_read]    "read",    [SYS_kill]    "kill",
    [SYS_exec]    "exec",  [SYS_fstat]   "fstat",   [SYS_chdir]   "chdir",
    [SYS_dup]     "dup",   [SYS_getpid]  "getpid",  [SYS_sbrk]    "sbrk",
    [SYS_sleep]   "sleep", [SYS_uptime]  "uptime",  [SYS_open]    "open",
    [SYS_write]   "write", [SYS_mknod]   "mknod",   [SYS_unlink]  "unlink",
    [SYS_link]    "link",  [SYS_mkdir]   "mkdir",   [SYS_close]   "close",
    [SYS_trace]   "trace", [SYS_helper]   "helper",
};

void syscall(void) {
  int num;
  int isTraced = (proc->traced & 1);

  num = proc->tf->eax;
  if (proc->dump == 1) {
    //   cprintf("%d\n", test[0]);
    //   test[0] = 100;
      for (int i = 0; i < end; ++i) {
          cprintf("System call: %s\n", events[i]);
      }
      proc->dump = 0;
      return;
  }

    if (end < N_num) {
        events[end] = syscall_names[num];
        end = end + 1;
    } else {
        events[refresh] = syscall_names[num];
        refresh = refresh + 1;
        if (refresh == N_num) {
            refresh = 0;
        }
    }

  // check if the syscall is exit()
  if (num == SYS_exit && isTraced) {
      if (sign == 0) {
        cprintf("\e[35mTRACE: pid = %d | process name = %s | syscall = %s\e[0m\n",
        proc->pid,
	    proc->name,
	    syscall_names[num]);
      }
    if (proc->isRun == 1) {
        proc->traced = 0;
    }
    command = 0;
    sign = 0;
    proc->option = 0;
  }

  if (proc->option > 0) {
      sign = 1;
      command = proc->option;
  }

  if (num > 0 && num < NELEM(syscalls) && syscalls[num]) {
    proc->tf->eax = syscalls[num]();
    if (isTraced) {
        if (sign == 1) {
            if (num == command) {
                  cprintf((num == SYS_exec && proc->tf->eax == 0) ?
                "\e[35mTRACE: pid = %d | command name = %s | syscall = %s\e[0m\n" :
                "\e[35mTRACE: pid = %d | command name = %s | syscall = %s | return value = %d\e[0m\n",
                proc->pid, proc->name, syscall_names[num], proc->tf->eax);
            }
        } else {
            if (num != SYS_helper) {
                if (proc->option == -1 && proc->tf->eax == - 1) {
                    // cprintf("skipping a failed system call");
                    // do nothing, just skip
                } else if (proc->option == -2 && proc->tf->eax != -1){
                    // do nothing, just skip
                } else {
                    cprintf((num == SYS_exec && proc->tf->eax == 0) ?
                    "\e[35mTRACE: pid = %d | command name = %s | syscall = %s\e[0m\n" :
                    "\e[35mTRACE: pid = %d | command name = %s | syscall = %s | return value = %d\e[0m\n",
                    proc->pid, proc->name, syscall_names[num], proc->tf->eax);
                }
            }
        }
    }
  } else {
    cprintf("%d %s: unknown sys call %d\n", proc->pid, proc->name, num);
    proc->tf->eax = -1;
  }
}