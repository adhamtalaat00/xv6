// Proc.h - Process structure with security fields

#ifndef PROC_H
#define PROC_H

#include "types.h"

#define NPROC        64  // maximum number of processes
#define KSTACKSIZE 4096  // size of per-process kernel stack
#define NOFILE       16  // open files per process
#define MAXARG       32  // max exec arguments
#define MAXPATH     128  // maximum file path length

enum procstate { UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

// Per-process state
struct proc {
  uint sz;                      // Size of process memory (bytes)
  char *mem;                    // Virtual address of process memory
  uint ustack;                  // User stack pointer
  char *kstack;                 // Kernel stack
  enum procstate state;         // Process state
  int pid;                      // Process ID
  struct proc *parent;          // Parent process
  void *chan;                   // If non-zero, sleeping on chan
  int killed;                   // If non-zero, have been killed
  struct file *ofile[NOFILE];   // Open files
  struct inode *cwd;            // Current working directory
  char name[16];                // Process name (debugging)
  
  // Security fields - Phase 1
  uid_t uid;                    // User ID
  gid_t gid;                    // Group ID
  uid_t euid;                   // Effective user ID
  gid_t egid;                   // Effective group ID
};

struct ptable {
  int lock;
  struct proc proc[NPROC];
};

#endif // PROC_H
