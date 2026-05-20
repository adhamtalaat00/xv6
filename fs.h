// fs.h - File system structures with security fields

#ifndef FS_H
#define FS_H

#include "types.h"

#define ROOTINO 1      // root i-number
#define BSIZE 512      // block size
#define NDIRECT 12
#define NINDIRECT (BSIZE / sizeof(uint))
#define MAXFILE (NDIRECT + NINDIRECT)

// On-disk inode structure
struct dinode {
  short type;                  // File type
  short nlink;                 // Number of links to file
  uint size;                   // Size of file (bytes)
  uint addrs[NDIRECT+1];       // Data block addresses
  
  // Security fields - Phase 2
  ushort mode;                 // File permissions (rwxrwxrwx)
  uid_t uid;                   // Owner user ID
  gid_t gid;                   // Owner group ID
};

// Inodes per block
#define IPB           (BSIZE / sizeof(struct dinode))

// Block containing inode i
#define IBLOCK(i, sb)  ((i) / IPB + sb.inodestart)

// Bitmap bits per block
#define BPB           (BSIZE * 8)

// Block of free map containing bit for block b
#define BBLOCK(b, sb) (b/BPB + sb.bmapstart)

// Directory is a file containing a sequence of dirent structures.
#define DIRSIZ 14

struct dirent {
  ushort inum;
  char name[DIRSIZ];
};

#endif // FS_H
