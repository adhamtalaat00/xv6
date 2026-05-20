// user_auth.h - User authentication definitions

#ifndef USER_AUTH_H
#define USER_AUTH_H

#include "types.h"

#define MAX_USERS    32
#define MAX_USERNAME 32
#define MAX_PASSWORD 64
#define PASSWD_FILE  "/etc/passwd"
#define SHADOW_FILE  "/etc/shadow"

// User entry (simplified /etc/passwd)
struct user_entry {
  uid_t uid;
  gid_t gid;
  char username[MAX_USERNAME];
  char homedir[128];
  char shell[128];
};

// Shadow entry (password hashes)
struct shadow_entry {
  uid_t uid;
  char username[MAX_USERNAME];
  char password_hash[64];  // SHA256 hash
};

// In-memory user table
struct user_table {
  int num_users;
  struct user_entry users[MAX_USERS];
  struct shadow_entry shadows[MAX_USERS];
};

// Default users
#define ROOT_UID 0
#define FIRST_USER_UID 1000

// Hash function for passwords
extern void hash_password(char *password, char *output_hash);
extern int verify_password(char *password, char *stored_hash);
extern int authenticate_user(char *username, char *password, uid_t *uid, gid_t *gid);
extern int add_user(char *username, char *password);
extern int delete_user(char *username);
extern int change_password(char *username, char *new_password);
extern void init_user_table(void);

#endif // USER_AUTH_H

