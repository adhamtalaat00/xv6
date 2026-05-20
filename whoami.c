// whoami.c - Show current user information

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "types.h"
#include "user_auth.h"

int main(int argc, char *argv[]) {
  // In real xv6, this would read from process credentials
  // For now, demonstrate with a hard-coded test
  
  char *user = getenv("USER");
  
  if (user) {
    printf("%s\n", user);
  } else {
    // Default to root if no USER env var
    printf("root\n");
  }
  
  return 0;
}
