// audit.h - System call audit logging definitions

#ifndef AUDIT_H
#define AUDIT_H

#include "types.h"

#define AUDIT_RINGBUF_SIZE 4096  // Ring buffer for audit logs
#define AUDIT_MAX_ENTRIES  128   // Max number of log entries

// Trap number to readable name mapping
#define T_SYSCALL 64

#define TRAPNAMES_LEN 65
extern const char *trapnames[TRAPNAMES_LEN];

// Syscall audit log entry
struct audit_entry {
  uint tick;          // Timestamp (in ticks)
  int pid;            // Process ID
  uid_t uid;          // User ID
  int syscall_num;    // Syscall number
  uint eip;           // Instruction pointer
  int retval;         // Return value
  int errno;          // Error number (if any)
};

// Audit log ring buffer
struct audit_log {
  int lock;
  struct audit_entry entries[AUDIT_MAX_ENTRIES];
  int head;           // Write pointer
  int tail;           // Read pointer (for audit_read)
  int count;          // Number of entries
};

// Export global audit buffer
extern struct audit_log audit_buffer;

// Function declarations
extern void init_audit_log(void);
extern void audit_log_syscall(int syscall_num, uint eip, int retval, int errno_val);
extern int audit_read(int start_idx, struct audit_entry *buf, int nbuf);
extern const char* get_trapname(int trapno);

#endif // AUDIT_H

