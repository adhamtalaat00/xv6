// chown.c - Change file owner

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "types.h"

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr, "Usage: chown <owner>[:group] <file>\n");
    exit(1);
  }
  
  char *owner_str = argv[1];
  char *file = argv[2];
  
  char owner_copy[256];
  strncpy(owner_copy, owner_str, 255);
  owner_copy[255] = '\0';
  
  // Parse owner:group format
  uid_t uid = atoi(owner_copy);
  gid_t gid = -1;  // -1 means don't change group
  
  char *colon = strchr(owner_copy, ':');
  if (colon) {
    *colon = '\0';
    uid = atoi(owner_copy);
    gid = atoi(colon + 1);
  }
  
  // Call chown (would be syscall in real xv6)
  if (chown(file, uid, gid) < 0) {
    perror("chown failed");
    exit(1);
  }
  
  printf("Changed %s owner to %d\n", file, uid);
  return 0;
}
