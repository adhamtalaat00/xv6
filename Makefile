# Makefile for xv6 Security Implementation

CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -g -O2
LDFLAGS = -lm

# Core kernel security modules
KERNEL_OBJS = user_auth.o perms.o audit.o

# User space utilities
UTILS = useradd userdel passwd whoami chmod chown

# Test suite
TEST_BIN = test_security

# Main target
all: $(TEST_BIN) $(UTILS)

# Compile kernel security modules
user_auth.o: user_auth.c user_auth.h types.h
	$(CC) $(CFLAGS) -c user_auth.c -o user_auth.o

perms.o: perms.c perms.h fs.h types.h
	$(CC) $(CFLAGS) -c perms.c -o perms.o

audit.o: audit.c audit.h types.h
	$(CC) $(CFLAGS) -c audit.c -o audit.o

# Link kernel security library
libsecurity.a: $(KERNEL_OBJS)
	ar rcs libsecurity.a $(KERNEL_OBJS)

# Build test suite
$(TEST_BIN): test_security.c libsecurity.a
	$(CC) $(CFLAGS) test_security.c -o $(TEST_BIN) libsecurity.a $(LDFLAGS)

# Build utilities
useradd: useradd.c libsecurity.a
	$(CC) $(CFLAGS) useradd.c -o useradd libsecurity.a $(LDFLAGS)

userdel: userdel.c libsecurity.a
	$(CC) $(CFLAGS) userdel.c -o userdel libsecurity.a $(LDFLAGS)

passwd: passwd.c libsecurity.a
	$(CC) $(CFLAGS) passwd.c -o passwd libsecurity.a $(LDFLAGS)

whoami: whoami.c
	$(CC) $(CFLAGS) whoami.c -o whoami $(LDFLAGS)

chmod: chmod.c
	$(CC) $(CFLAGS) chmod.c -o chmod $(LDFLAGS)

chown: chown.c
	$(CC) $(CFLAGS) chown.c -o chown $(LDFLAGS)

# Run all tests
test: all
	@echo "=========================================="
	@echo "Running xv6 Security Test Suite"
	@echo "=========================================="
	@./$(TEST_BIN)

# Clean build artifacts
clean:
	rm -f $(KERNEL_OBJS) $(TEST_BIN) $(UTILS) libsecurity.a *.o

# Clean everything including utilities
distclean: clean

# Phony targets
.PHONY: all test clean distclean
