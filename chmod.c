// chmod.c - Change file permissions

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "types.h"
#include "perms.h"

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr, "Usage: chmod <mode> <file>\n");
    fprintf(stderr, "  mode: octal permission (e.g., 755, 644)\n");
    exit(1);
  }
  
  char *mode_str = argv[1];
  char *file = argv[2];
  
  // Parse octal mode
  int mode = 0;
  sscanf(mode_str, "%o", &mode);
  
  // Call chmod (would be syscall in real xv6)
  if (chmod(file, mode) < 0) {
    perror("chmod failed");
    exit(1);
  }
  
  printf("Changed %s permissions to %s\n", file, mode_str);
  return 0;
}
