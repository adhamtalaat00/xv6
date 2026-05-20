// perms.h - File permission checking definitions

#ifndef PERMS_H
#define PERMS_H

#include "types.h"
#include "fs.h"

// Permission bits
#define S_IRUSR  0400   // Owner read
#define S_IWUSR  0200   // Owner write
#define S_IXUSR  0100   // Owner execute
#define S_IRGRP  0040   // Group read
#define S_IWGRP  0020   // Group write
#define S_IXGRP  0010   // Group execute
#define S_IROTH  0004   // Others read
#define S_IWOTH  0002   // Others write
#define S_IXOTH  0001   // Others execute

// File type bits
#define S_IFMT   0170000
#define S_IFREG  0100000
#define S_IFDIR  0040000
#define S_IFLNK  0120000
#define S_IFBLK  0060000
#define S_IFCHR  0020000

// Permission checking
#define PERM_READ   1
#define PERM_WRITE  2
#define PERM_EXEC   4

// Function declarations
extern int check_permission(struct dinode *ip, int perm, uid_t uid, gid_t gid);
extern int check_exec_permission(struct dinode *ip, uid_t uid, gid_t gid);
extern int is_admin(uid_t uid);

#endif // PERMS_H
