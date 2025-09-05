#pragma once

#include "efi.h"
#include <stdint.h>

typedef struct _DISK {
    UINT64 Sectors;
    UINT32 Id;
    UINT16 BlockSize;
    EFI_BLOCK_IO_PROTOCOL *BlockIo;
    EFI_DISK_IO_PROTOCOL *DiskIo;
} DISK, *PDISK;

// Enumerates through disks (as well as partitions) and stores them in an array for later use
VOID BlInitDiskServices();

INT32 BlGetDiskCount();
INT32 BlGetPartCount();

// Reads from disk defined by idx, at offset, with the size in bytes, and returns the bytes read in buffer
VOID BlReadDisk(INT32 Index, UINT64 Offset, INT32 Bytes, VOID *Buffer);

// Reads from partition defined by idx, at offset, with the size in bytes, and returns the bytes read in buffer
VOID BlReadPart(INT32 Index, UINT64 Offset, INT32 Bytes, VOID *Buffer);