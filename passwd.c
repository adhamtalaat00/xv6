// passwd.c - Password change utility

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "types.h"
#include "user_auth.h"

// Simple getpass replacement (not secure - for demo only)
char* read_password(const char *prompt) {
  static char password[MAX_PASSWORD];
  printf("%s", prompt);
  fflush(stdout);
  
  int i = 0;
  int c;
  while ((c = getchar()) != '\n' && c != EOF && i < MAX_PASSWORD - 1) {
    password[i++] = c;
  }
  password[i] = '\0';
  return password;
}

int main(int argc, char *argv[]) {
  char username[MAX_USERNAME];
  char old_password[MAX_PASSWORD];
  char new_password1[MAX_PASSWORD];
  char new_password2[MAX_PASSWORD];
  
  if (argc < 2) {
    // Change own password
    printf("Current user: ");
    fgets(username, MAX_USERNAME - 1, stdin);
    // Remove newline
    int len = strlen(username);
    if (len > 0 && username[len-1] == '\n') {
      username[len-1] = '\0';
    }
  } else {
    strncpy(username, argv[1], MAX_USERNAME - 1);
    username[MAX_USERNAME - 1] = '\0';
  }
  
  // Verify current password
  uid_t uid;
  gid_t gid;
  
  printf("Old password: ");
  fgets(old_password, MAX_PASSWORD - 1, stdin);
  int len = strlen(old_password);
  if (len > 0 && old_password[len-1] == '\n') {
    old_password[len-1] = '\0';
  }
  
  if (!authenticate_user(username, old_password, &uid, &gid)) {
    fprintf(stderr, "Authentication failed\n");
    exit(1);
  }
  
  // Get new password
  printf("New password: ");
  fgets(new_password1, MAX_PASSWORD - 1, stdin);
  len = strlen(new_password1);
  if (len > 0 && new_password1[len-1] == '\n') {
    new_password1[len-1] = '\0';
  }
  
  printf("Confirm password: ");
  fgets(new_password2, MAX_PASSWORD - 1, stdin);
  len = strlen(new_password2);
  if (len > 0 && new_password2[len-1] == '\n') {
    new_password2[len-1] = '\0';
  }
  
  if (strcmp(new_password1, new_password2) != 0) {
    fprintf(stderr, "Passwords do not match\n");
    exit(1);
  }
  
  int ret = change_password(username, new_password1);
  if (ret < 0) {
    fprintf(stderr, "Failed to change password\n");
    exit(1);
  }
  
  printf("Password changed successfully\n");
  return 0;
}
