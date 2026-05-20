// user_auth.c - User authentication implementation

#include "types.h"
#include "user_auth.h"
#include <string.h>
#include <stdio.h>

// Global user table
struct user_table users;
int users_initialized = 0;

// Simple SHA256-like hash (simplified for demonstration)
// In production, use proper cryptographic hash
void hash_password(char *password, char *output_hash) {
  unsigned int hash = 5381;
  int c;
  int len = 0;
  
  // Simple hash - in real impl use SHA256
  while ((c = *password++) && len < 64) {
    hash = ((hash << 5) + hash) + c;
    len++;
  }
  
  // Convert to hex string (simplified)
  int i;
  for (i = 0; i < 8 && i < 64; i++) {
    char byte = (hash >> (i * 4)) & 0xFF;
    output_hash[i * 2] = "0123456789abcdef"[byte >> 4];
    output_hash[i * 2 + 1] = "0123456789abcdef"[byte & 0x0F];
  }
  output_hash[i * 2] = '\0';
}

int verify_password(char *password, char *stored_hash) {
  char computed_hash[64];
  hash_password(password, computed_hash);
  return strcmp(computed_hash, stored_hash) == 0;
}

void init_user_table(void) {
  printf("DEBUG: init_user_table - checking if initialized\n");
  fflush(stdout);
  if (users_initialized) return;
  
  printf("DEBUG: init_user_table - initializing\n");
  fflush(stdout);
  users.num_users = 0;
  users_initialized = 1;
  
  // Initialize root user
  struct user_entry root;
  printf("DEBUG: Creating root user entry\n");
  fflush(stdout);
  root.uid = 0;
  root.gid = 0;
  strncpy(root.username, "root", MAX_USERNAME - 1);
  root.username[MAX_USERNAME - 1] = '\0';
  strncpy(root.homedir, "/root", 127);
  root.homedir[127] = '\0';
  strncpy(root.shell, "/bin/sh", 127);
  root.shell[127] = '\0';
  
  printf("DEBUG: Adding root to user table\n");
  fflush(stdout);
  users.users[0] = root;
  
  // Initialize root shadow
  struct shadow_entry root_shadow;
  printf("DEBUG: Creating root shadow entry\n");
  fflush(stdout);
  root_shadow.uid = 0;
  strncpy(root_shadow.username, "root", MAX_USERNAME - 1);
  root_shadow.username[MAX_USERNAME - 1] = '\0';
  printf("DEBUG: Hashing root password\n");
  fflush(stdout);
  hash_password("root", root_shadow.password_hash);
  
  printf("DEBUG: Adding root shadow to table\n");
  fflush(stdout);
  users.shadows[0] = root_shadow;
  users.num_users = 1;
  printf("DEBUG: init_user_table complete\n");
  fflush(stdout);
}

int authenticate_user(char *username, char *password, uid_t *uid, gid_t *gid) {
  init_user_table();
  
  for (int i = 0; i < users.num_users; i++) {
    if (strncmp(users.users[i].username, username, MAX_USERNAME) == 0) {
      if (verify_password(password, users.shadows[i].password_hash)) {
        *uid = users.users[i].uid;
        *gid = users.users[i].gid;
        return 1;  // Success
      }
      return 0;   // Wrong password
    }
  }
  return 0;       // User not found
}

int add_user(char *username, char *password) {
  init_user_table();
  
  if (users.num_users >= MAX_USERS) {
    return -1;  // User table full
  }
  
  // Check if user already exists
  for (int i = 0; i < users.num_users; i++) {
    if (strncmp(users.users[i].username, username, MAX_USERNAME) == 0) {
      return -1;  // User already exists
    }
  }
  
  int idx = users.num_users;
  
  // Add user entry
  struct user_entry new_user;
  new_user.uid = FIRST_USER_UID + idx;
  new_user.gid = FIRST_USER_UID + idx;
  strncpy(new_user.username, username, MAX_USERNAME - 1);
  new_user.username[MAX_USERNAME - 1] = '\0';
  
  // Create home directory path
  char homedir_buf[128];
  snprintf(homedir_buf, 127, "/home/%s", username);
  strncpy(new_user.homedir, homedir_buf, 127);
  new_user.homedir[127] = '\0';
  
  strncpy(new_user.shell, "/bin/sh", 127);
  new_user.shell[127] = '\0';
  
  users.users[idx] = new_user;
  
  // Add shadow entry
  struct shadow_entry new_shadow;
  new_shadow.uid = new_user.uid;
  strncpy(new_shadow.username, username, MAX_USERNAME - 1);
  new_shadow.username[MAX_USERNAME - 1] = '\0';
  hash_password(password, new_shadow.password_hash);
  
  users.shadows[idx] = new_shadow;
  users.num_users++;
  
  return new_user.uid;
}

int delete_user(char *username) {
  init_user_table();
  
  for (int i = 0; i < users.num_users; i++) {
    if (strncmp(users.users[i].username, username, MAX_USERNAME) == 0) {
      // Don't allow deleting root
      if (users.users[i].uid == 0) {
        return -1;
      }
      
      // Shift remaining users
      for (int j = i; j < users.num_users - 1; j++) {
        users.users[j] = users.users[j + 1];
        users.shadows[j] = users.shadows[j + 1];
      }
      users.num_users--;
      return 0;  // Success
    }
  }
  return -1;     // User not found
}

int change_password(char *username, char *new_password) {
  init_user_table();
  
  for (int i = 0; i < users.num_users; i++) {
    if (strncmp(users.users[i].username, username, MAX_USERNAME) == 0) {
      hash_password(new_password, users.shadows[i].password_hash);
      return 0;  // Success
    }
  }
  return -1;     // User not found
}
