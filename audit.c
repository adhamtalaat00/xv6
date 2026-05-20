// audit.c - System call audit logging implementation

#include "types.h"
#include "audit.h"
#include <string.h>

// Global audit log ring buffer
struct audit_log audit_buffer;
int audit_initialized = 0;

// Trap names mapping
const char *trapnames[] = {
  "T_DIVIDE",      // 0
  "T_DEBUG",       // 1
  "T_NMI",         // 2
  "T_BRKPT",       // 3
  "T_OFLOW",       // 4
  "T_BOUND",       // 5
  "T_ILLOP",       // 6
  "T_DEVICE",      // 7
  "T_DBLFLT",      // 8
  "T_COPROC",      // 9
  "T_TSS",         // 10
  "T_SEGNP",       // 11
  "T_STACK",       // 12
  "T_GPFLT",       // 13
  "T_PGFLT",       // 14
  "T_RES",         // 15
  "T_FPERR",       // 16
  "T_ALIGN",       // 17
  "T_MCHK",        // 18
  "T_SIMDERR",     // 19
  "T_RES",         // 20
  "T_RES",         // 21
  "T_RES",         // 22
  "T_RES",         // 23
  "T_RES",         // 24
  "T_RES",         // 25
  "T_RES",         // 26
  "T_RES",         // 27
  "T_RES",         // 28
  "T_RES",         // 29
  "T_RES",         // 30
  "T_RES",         // 31
  "T_IRQ0",        // 32
  "T_IRQ1",        // 33
  "T_IRQ2",        // 34
  "T_IRQ3",        // 35
  "T_IRQ4",        // 36
  "T_IRQ5",        // 37
  "T_IRQ6",        // 38
  "T_IRQ7",        // 39
  "T_IRQ8",        // 40
  "T_IRQ9",        // 41
  "T_IRQ10",       // 42
  "T_IRQ11",       // 43
  "T_IRQ12",       // 44
  "T_IRQ13",       // 45
  "T_IRQ14",       // 46
  "T_IRQ15",       // 47
  "T_RES",         // 48-63 reserved
  "T_RES",
  "T_RES",
  "T_RES",
  "T_RES",
  "T_RES",
  "T_RES",
  "T_RES",
  "T_RES",
  "T_RES",
  "T_RES",
  "T_RES",
  "T_RES",
  "T_RES",
  "T_RES",
  "T_SYSCALL"      // 64 - Syscall
};

const char* get_trapname(int trapno) {
  if (trapno >= 0 && trapno < 64) {
    return trapnames[trapno];
  }
  return "UNKNOWN";
}

void init_audit_log(void) {
  if (audit_initialized) return;
  
  audit_buffer.lock = 0;
  audit_buffer.head = 0;
  audit_buffer.tail = 0;
  audit_buffer.count = 0;
  
  memset(audit_buffer.entries, 0, sizeof(audit_buffer.entries));
  audit_initialized = 1;
}

void audit_log_syscall(int syscall_num, uint eip, int retval, int errno_val) {
  init_audit_log();
  
  // Get current process info (simplified - in real xv6, would access current proc)
  // For now, use dummy values
  int pid = 1;
  uid_t uid = 0;
  uint tick = 0;
  
  // Acquire lock (simplified - in real impl use spinlock)
  // while (__sync_lock_test_and_set(&audit_buffer.lock, 1));
  
  // Add entry to ring buffer
  int idx = audit_buffer.head;
  audit_buffer.entries[idx].pid = pid;
  audit_buffer.entries[idx].uid = uid;
  audit_buffer.entries[idx].syscall_num = syscall_num;
  audit_buffer.entries[idx].eip = eip;
  audit_buffer.entries[idx].retval = retval;
  audit_buffer.entries[idx].errno = errno_val;
  audit_buffer.entries[idx].tick = tick;
  
  // Update head pointer
  audit_buffer.head = (audit_buffer.head + 1) % AUDIT_MAX_ENTRIES;
  if (audit_buffer.count < AUDIT_MAX_ENTRIES) {
    audit_buffer.count++;
  } else {
    audit_buffer.tail = (audit_buffer.tail + 1) % AUDIT_MAX_ENTRIES;
  }
  
  // Release lock (simplified)
  // __sync_lock_release(&audit_buffer.lock);
}

int audit_read(int start_idx, struct audit_entry *buf, int nbuf) {
  init_audit_log();
  
  if (nbuf <= 0) return 0;
  if (nbuf > AUDIT_MAX_ENTRIES) nbuf = AUDIT_MAX_ENTRIES;
  
  // Acquire lock (simplified)
  // while (__sync_lock_test_and_set(&audit_buffer.lock, 1));
  
  int count = 0;
  int idx = start_idx;
  
  for (int i = 0; i < nbuf && count < audit_buffer.count; i++) {
    if (idx >= AUDIT_MAX_ENTRIES) break;
    if (idx < audit_buffer.tail || idx >= audit_buffer.head) break;
    
    buf[count] = audit_buffer.entries[idx];
    idx++;
    count++;
  }
  
  // Release lock (simplified)
  // __sync_lock_release(&audit_buffer.lock);
  
  return count;
}
