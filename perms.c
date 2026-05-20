// perms.c - File permission checking implementation

#include "types.h"
#include "perms.h"
#include "fs.h"
#include "user_auth.h"

int is_admin(uid_t uid) {
  return uid == ROOT_UID;
}

int check_permission(struct dinode *ip, int perm, uid_t uid, gid_t gid) {
  if (!ip) return 0;
  
  // Root can do anything
  if (is_admin(uid)) {
    return 1;
  }
  
  ushort mode = ip->mode;
  
  // Check owner permissions
  if (ip->uid == uid) {
    if ((perm & PERM_READ) && (mode & S_IRUSR)) return 1;
    if ((perm & PERM_WRITE) && (mode & S_IWUSR)) return 1;
    if ((perm & PERM_EXEC) && (mode & S_IXUSR)) return 1;
    return 0;
  }
  
  // Check group permissions
  if (ip->gid == gid) {
    if ((perm & PERM_READ) && (mode & S_IRGRP)) return 1;
    if ((perm & PERM_WRITE) && (mode & S_IWGRP)) return 1;
    if ((perm & PERM_EXEC) && (mode & S_IXGRP)) return 1;
    return 0;
  }
  
  // Check other permissions
  if ((perm & PERM_READ) && (mode & S_IROTH)) return 1;
  if ((perm & PERM_WRITE) && (mode & S_IWOTH)) return 1;
  if ((perm & PERM_EXEC) && (mode & S_IXOTH)) return 1;
  
  return 0;
}

int check_exec_permission(struct dinode *ip, uid_t uid, gid_t gid) {
  return check_permission(ip, PERM_EXEC, uid, gid);
}
