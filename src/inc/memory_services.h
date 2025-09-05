#pragma once

#include <efi.h>
#include <stddef.h>
#include <stdint.h>
#include "boot_protocol/snowboot.h"
#include "efi/x86_64/efibind.h"
#include "virtual_memory.h"

VOID *memset(void *s, int c, size_t n);

VOID *memcpy(void *dest, const void *src, size_t n);

INT32 memcmp(const void *s1, const void *s2, size_t n);

SNOWBOOT_MEM_TYPE BlUefiTypeToNative(EFI_MEMORY_TYPE type);

EFI_MEMORY_DESCRIPTOR* BlGetMemoryMap(pagemap_t pagemap);

VOID BlSetMemoryTypes(EFI_MEMORY_DESCRIPTOR *MemoryMap);

// Returns the number of entries in the Memory Map
UINT64 BlGetEntryCount();

UINT64 BlGetMapKey();

UINT64 BlGetDescSize();

// Allocates size amount of bytes and returns the memory allocated in buffer
VOID UefiAllocatePool(UINTN size, void *buffer);
VOID UefiAllocatePages(UINTN pages, uint64_t *memory, EFI_MEMORY_TYPE memType);