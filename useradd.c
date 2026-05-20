// useradd.c - User addition utility

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "types.h"
#include "user_auth.h"

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr, "Usage: useradd <username> <password>\n");
    exit(1);
  }
  
  char *username = argv[1];
  char *password = argv[2];
  
  // Check username length
  if (strlen(username) >= MAX_USERNAME) {
    fprintf(stderr, "Username too long\n");
    exit(1);
  }
  
  // Check password length
  if (strlen(password) >= MAX_PASSWORD) {
    fprintf(stderr, "Password too long\n");
    exit(1);
  }
  
  int uid = add_user(username, password);
  if (uid < 0) {
    fprintf(stderr, "Failed to add user\n");
    exit(1);
  }
  
  printf("User %s added with UID %d\n", username, uid);
  return 0;
}
