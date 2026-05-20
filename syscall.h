// syscall.h - System call definitions

#ifndef SYSCALL_H
#define SYSCALL_H

// System calls
#define SYS_fork    1
#define SYS_exit    2
#define SYS_wait    3
#define SYS_pipe    4
#define SYS_read    5
#define SYS_kill    6
#define SYS_exec    7
#define SYS_fstat   8
#define SYS_chdir   9
#define SYS_dup    10
#define SYS_getpid 11
#define SYS_sbrk   12
#define SYS_sleep  13
#define SYS_uptime 14
#define SYS_open   15
#define SYS_write  16
#define SYS_close  17

// Phase 1 - User Authentication Syscalls
#define SYS_useradd    18
#define SYS_userdel    19
#define SYS_passwd     20
#define SYS_whoami     21
#define SYS_getuid     22
#define SYS_setuid     23

// Phase 2 - File Permissions Syscalls
#define SYS_chmod      24
#define SYS_chown      25
#define SYS_chmod_stat 26  // for stat with mode

// Phase 3 - Audit Logging Syscall
#define SYS_audit_read 27

#endif // SYSCALL_H
