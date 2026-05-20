// userdel.c - User deletion utility

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "types.h"
#include "user_auth.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: userdel <username>\n");
    exit(1);
  }
  
  char *username = argv[1];
  
  int ret = delete_user(username);
  if (ret < 0) {
    fprintf(stderr, "Failed to delete user (user not found or is root)\n");
    exit(1);
  }
  
  printf("User %s deleted\n", username);
  return 0;
}
