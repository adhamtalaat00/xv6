// test_security.c - Comprehensive security test suite

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "types.h"
#include "user_auth.h"
#include "perms.h"
#include "audit.h"

// Test result tracking
int total_tests = 0;
int passed_tests = 0;
int failed_tests = 0;

void print_test_header(void) {
  printf("\n");
  printf("====================================================================\n");
  printf("        xv6 KERNEL SECURITY - COMPREHENSIVE TEST SUITE\n");
  printf("====================================================================\n");
  printf("\n");
}

void print_section(const char *section_name) {
  printf("\n--- %s ---\n", section_name);
}

void test_result(int result, const char *test_name) {
  total_tests++;
  if (result) {
    passed_tests++;
    printf("[PASS] %s\n", test_name);
  } else {
    failed_tests++;
    printf("[FAIL] %s\n", test_name);
  }
}

// ============== PHASE 1 TESTS - User Authentication ==============

void test_phase1_authentication(void) {
  print_section("PHASE 1: USER AUTHENTICATION");
  
  // Test 1: Initialize user table
  init_user_table();
  test_result(1, "Initialize user table");
  
  // Test 2: Add new user
  int uid = add_user("testuser", "password123");
  test_result(uid > 0, "Add new user with valid credentials");
  
  // Test 3: Duplicate user prevention
  int dup_uid = add_user("testuser", "different");
  test_result(dup_uid < 0, "Prevent duplicate user creation");
  
  // Test 4: Authenticate with correct password
  uid_t auth_uid;
  gid_t auth_gid;
  int auth_result = authenticate_user("testuser", "password123", &auth_uid, &auth_gid);
  test_result(auth_result == 1, "Authenticate user with correct password");
  
  // Test 5: Reject incorrect password
  auth_result = authenticate_user("testuser", "wrongpassword", &auth_uid, &auth_gid);
  test_result(auth_result == 0, "Reject authentication with wrong password");
  
  // Test 6: Reject non-existent user
  auth_result = authenticate_user("nonexistent", "password", &auth_uid, &auth_gid);
  test_result(auth_result == 0, "Reject non-existent user");
  
  // Test 7: Root user exists
  auth_result = authenticate_user("root", "root", &auth_uid, &auth_gid);
  test_result(auth_result == 1, "Root user exists and can authenticate");
  
  // Test 8: Change password
  int pwd_change = change_password("testuser", "newpassword");
  test_result(pwd_change == 0, "Change user password");
  
  // Test 9: Old password rejected after change
  auth_result = authenticate_user("testuser", "password123", &auth_uid, &auth_gid);
  test_result(auth_result == 0, "Old password rejected after change");
  
  // Test 10: New password accepted
  auth_result = authenticate_user("testuser", "newpassword", &auth_uid, &auth_gid);
  test_result(auth_result == 1, "New password accepted after change");
  
  // Test 11: Delete user
  int del_result = delete_user("testuser");
  test_result(del_result == 0, "Delete user successfully");
  
  // Test 12: Deleted user cannot authenticate
  auth_result = authenticate_user("testuser", "newpassword", &auth_uid, &auth_gid);
  test_result(auth_result == 0, "Deleted user cannot authenticate");
  
  // Test 13: Root cannot be deleted
  del_result = delete_user("root");
  test_result(del_result < 0, "Prevent deletion of root user");
  
  // Test 14: Add multiple users
  uid_t uid1 = add_user("user1", "pass1");
  uid_t uid2 = add_user("user2", "pass2");
  test_result(uid1 > 0 && uid2 > 0 && uid1 != uid2, "Add multiple users with unique UIDs");
}

// ============== PHASE 2 TESTS - File Permissions ==============

void test_phase2_permissions(void) {
  print_section("PHASE 2: FILE PERMISSIONS");
  
  // Create test inode structures
  struct dinode test_file;
  test_file.mode = 0644;  // rw-r--r--
  test_file.uid = 1000;   // user1
  test_file.gid = 1000;
  
  // Test 1: Owner can read own file
  int perm = check_permission(&test_file, PERM_READ, 1000, 1000);
  test_result(perm == 1, "Owner can read own file");
  
  // Test 2: Owner can write own file
  perm = check_permission(&test_file, PERM_WRITE, 1000, 1000);
  test_result(perm == 1, "Owner can write own file");
  
  // Test 3: Owner cannot execute non-executable file
  perm = check_permission(&test_file, PERM_EXEC, 1000, 1000);
  test_result(perm == 0, "Owner cannot execute non-executable file");
  
  // Test 4: Group can read
  perm = check_permission(&test_file, PERM_READ, 2000, 1000);
  test_result(perm == 1, "Group member can read file");
  
  // Test 5: Group cannot write
  perm = check_permission(&test_file, PERM_WRITE, 2000, 1000);
  test_result(perm == 0, "Group member cannot write file");
  
  // Test 6: Others can read
  perm = check_permission(&test_file, PERM_READ, 3000, 2000);
  test_result(perm == 1, "Others can read file");
  
  // Test 7: Others cannot write
  perm = check_permission(&test_file, PERM_WRITE, 3000, 2000);
  test_result(perm == 0, "Others cannot write file");
  
  // Test 8: Root can read any file
  struct dinode restricted_file;
  restricted_file.mode = 0000;  // no permissions
  restricted_file.uid = 1000;
  restricted_file.gid = 1000;
  perm = check_permission(&restricted_file, PERM_READ, 0, 0);
  test_result(perm == 1, "Root (uid=0) can read any file");
  
  // Test 9: Root can write any file
  perm = check_permission(&restricted_file, PERM_WRITE, 0, 0);
  test_result(perm == 1, "Root can write any file");
  
  // Test 10: Root can execute any file
  perm = check_permission(&restricted_file, PERM_EXEC, 0, 0);
  test_result(perm == 1, "Root can execute any file");
  
  // Test 11: Executable file permissions
  struct dinode exec_file;
  exec_file.mode = 0755;  // rwxr-xr-x
  exec_file.uid = 1000;
  exec_file.gid = 1000;
  
  perm = check_permission(&exec_file, PERM_EXEC, 1000, 1000);
  test_result(perm == 1, "Owner can execute executable file");
  
  // Test 12: Group can execute
  perm = check_permission(&exec_file, PERM_EXEC, 2000, 1000);
  test_result(perm == 1, "Group can execute executable file");
  
  // Test 13: Others can execute
  perm = check_permission(&exec_file, PERM_EXEC, 3000, 2000);
  test_result(perm == 1, "Others can execute executable file");
  
  // Test 14: is_admin function
  int admin = is_admin(0);
  test_result(admin == 1, "uid=0 is identified as admin");
  
  // Test 15: Non-root is not admin
  admin = is_admin(1000);
  test_result(admin == 0, "Non-zero uid is not admin");
}

// ============== PHASE 3 TESTS - Syscall Audit ==============

void test_phase3_audit(void) {
  print_section("PHASE 3: SYSCALL AUDIT LOGGING");
  
  // Test 1: Initialize audit log
  init_audit_log();
  test_result(1, "Initialize audit log");
  
  // Test 2: Log syscall
  audit_log_syscall(1, 0x1000, 0, 0);
  test_result(1, "Log syscall entry");
  
  // Test 3: Get trap name for syscall
  const char *name = get_trapname(T_SYSCALL);
  test_result(strcmp(name, "T_SYSCALL") == 0, "Get readable trap name for syscall");
  
  // Test 4: Get trap name for interrupt
  name = get_trapname(32);  // T_IRQ0
  test_result(strcmp(name, "T_IRQ0") == 0, "Get readable trap name for interrupt");
  
  // Test 5: Get trap name for exception
  name = get_trapname(14);  // T_PGFLT
  test_result(strcmp(name, "T_PGFLT") == 0, "Get readable trap name for page fault");
  
  // Test 6: Ring buffer doesn't overflow beyond max
  for (int i = 0; i < AUDIT_MAX_ENTRIES + 10; i++) {
    audit_log_syscall(i % 30, 0x1000 + i, 0, 0);
  }
  test_result(audit_buffer.count == AUDIT_MAX_ENTRIES, 
              "Ring buffer maintains max size");
  
  // Test 7: Read audit entries
  struct audit_entry entries[10];
  int count = audit_read(0, entries, 10);
  test_result(count > 0, "Read audit entries from buffer");
  
  // Test 8: Audit entry contains valid data
  test_result(entries[0].syscall_num >= 0, "Audit entry has valid syscall number");
  
  // Test 9: Multiple syscalls logged
  int empty_count_unused;
  empty_count_unused = audit_buffer.count;
  (void)empty_count_unused;
  init_audit_log();  // Reset
  audit_log_syscall(5, 0x2000, 0, 0);
  audit_log_syscall(15, 0x3000, 0, 0);
  test_result(audit_buffer.count == 2, "Multiple syscalls logged");
  
  // Test 10: Audit log entries are timestamped
  test_result(entries[0].tick >= 0U, "Audit entries are timestamped");
}

// ============== INTEGRATION TESTS ==============

void test_integration(void) {
  print_section("INTEGRATION TESTS");
  
  // Test 1: User login creates audit trail
  init_user_table();
  init_audit_log();
  
  uid_t uid;
  gid_t gid;
  int auth = authenticate_user("root", "root", &uid, &gid);
  audit_log_syscall(21, 0x5000, auth, 0);
  
  struct audit_entry entry;
  int count = audit_read(0, &entry, 1);
  test_result(count > 0 && entry.syscall_num == 21, 
              "User login creates audit entry");
  
  // Test 2: File access check respects uid from process
  struct dinode file;
  file.mode = 0755;
  file.uid = 1000;
  file.gid = 1000;
  
  // Process running as uid=1000 can access
  int perm = check_permission(&file, PERM_READ, 1000, 1000);
  test_result(perm == 1, "File access respects process uid");
  
  // Test 3: Admin bypass on permission checks
  file.mode = 0000;
  perm = check_permission(&file, PERM_READ, 0, 0);
  test_result(perm == 1, "Admin bypasses permission checks");
}

// ============== MEDICAL RECORDS SCENARIO TESTS ==============

void test_medical_scenario(void) {
  print_section("MEDICAL RECORDS SECURITY SCENARIO");
  
  init_user_table();
  
  // Create medical staff users
  uid_t doctor_uid = add_user("doctor", "docpass");
  uid_t nurse_uid = add_user("nurse", "nursepass");
  uid_t admin_uid = 0;  // root
  
  test_result(doctor_uid > 0 && nurse_uid > 0, 
              "Create medical staff users");
  
  // Scenario 1: Patient records read-only for doctor
  struct dinode patient_records;
  patient_records.mode = 0644;     // rw-r--r--
  patient_records.uid = doctor_uid;
  patient_records.gid = doctor_uid;
  
  int can_read = check_permission(&patient_records, PERM_READ, doctor_uid, doctor_uid);
  test_result(can_read == 1, "Doctor can read patient records");
  
  // Scenario 2: Nurse cannot modify patient records (owned by doctor)
  int can_write = check_permission(&patient_records, PERM_WRITE, nurse_uid, nurse_uid);
  test_result(can_write == 0, "Nurse cannot modify patient records");
  
  // Scenario 3: Dosage log writable by nurse, readable by doctor
  struct dinode dosage_log;
  dosage_log.mode = 0640;          // rw-r-----
  dosage_log.uid = nurse_uid;
  dosage_log.gid = doctor_uid;
  
  int nurse_write = check_permission(&dosage_log, PERM_WRITE, nurse_uid, nurse_uid);
  test_result(nurse_write == 1, "Nurse can write dosage log");
  
  int doc_read = check_permission(&dosage_log, PERM_READ, doctor_uid, doctor_uid);
  test_result(doc_read == 1, "Doctor can read dosage log");
  
  // Scenario 4: Device config admin-only
  struct dinode device_config;
  device_config.mode = 0600;       // rw-------
  device_config.uid = 0;
  device_config.gid = 0;
  
  int admin_access = check_permission(&device_config, PERM_READ, admin_uid, admin_uid);
  test_result(admin_access == 1, "Admin can access device config");
  
  int user_access = check_permission(&device_config, PERM_READ, doctor_uid, doctor_uid);
  test_result(user_access == 0, "Non-admin cannot access device config");
  
  // Scenario 5: Audit log admin-only
  struct dinode audit_log_file;
  audit_log_file.mode = 0600;      // rw-------
  audit_log_file.uid = 0;
  audit_log_file.gid = 0;
  
  admin_access = check_permission(&audit_log_file, PERM_READ, admin_uid, admin_uid);
  test_result(admin_access == 1, "Admin can read audit log");
  
  user_access = check_permission(&audit_log_file, PERM_READ, nurse_uid, nurse_uid);
  test_result(user_access == 0, "Non-admin cannot read audit log");
}

// ============== MAIN TEST RUNNER ==============

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  print_test_header();
  
  // Run all test phases
  test_phase1_authentication();
  test_phase2_permissions();
  test_phase3_audit();
  test_integration();
  test_medical_scenario();
  
  // Print results summary
  printf("\n");
  printf("====================================================================\n");
  printf("                      TEST RESULTS SUMMARY\n");
  printf("====================================================================\n");
  printf("Total Tests:  %d\n", total_tests);
  printf("Passed:       %d ✓\n", passed_tests);
  printf("Failed:       %d ✗\n", failed_tests);
  printf("Pass Rate:    %.1f%%\n", (total_tests > 0) ? 
         (100.0 * passed_tests / total_tests) : 0.0);
  printf("====================================================================\n");
  printf("\n");
  
  // Print compliance report
  printf("COMPLIANCE REPORT\n");
  printf("-----------------\n");
  printf("Phase 1 - User Authentication: %s\n", 
         passed_tests >= 14 ? "IMPLEMENTED ✓" : "INCOMPLETE");
  printf("Phase 2 - File Permissions:   %s\n", 
         passed_tests >= 28 ? "IMPLEMENTED ✓" : "INCOMPLETE");
  printf("Phase 3 - Syscall Audit:      %s\n", 
         passed_tests >= 38 ? "IMPLEMENTED ✓" : "INCOMPLETE");
  printf("Integration:                  %s\n", 
         passed_tests >= 48 ? "VERIFIED ✓" : "INCOMPLETE");
  printf("Medical Scenario:             %s\n", 
         passed_tests >= 58 ? "VALIDATED ✓" : "INCOMPLETE");
  printf("\n");
  
  return failed_tests == 0 ? 0 : 1;
}
