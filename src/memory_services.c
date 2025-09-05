#include <efi.h>
#include <stdint.h>
#include <inc/boot_protocol/snowboot.h>
#include <inc/globals.h>
#include <inc/log.h>
#include "inc/memory_services.h"
#include <inc/virtual_memory.h>

UINT64 NumberOfEntries;

UINT64 BootloaderMapKey;

UINTN DescriptorSize = 0;

void *memset(void *s, int c, size_t n) {
    uint8_t *p = (uint8_t *)s;

    for (size_t i = 0; i < n; i++) {
        p[i] = (uint8_t)c;
    }

    return s;
}

void *memcpy(void *dest, const void *src, size_t n) {
    asm volatile(
        "rep movsb"
        : "=D"(dest), "=S"(src), "=c"(n)
        : "D"(dest), "S"(src), "c"(n)
        : "memory"
    );
    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *p1 = (const uint8_t *)s1;
    const uint8_t *p2 = (const uint8_t *)s2;

    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] < p2[i] ? -1 : 1;
        }
    }

    return 0;
}

SNOWBOOT_MEM_TYPE UefiTypeToNative(EFI_MEMORY_TYPE Type)
{
    switch (Type)
    {
        case EfiReservedMemoryType:
        case EfiUnusableMemory:
        case EfiACPIMemoryNVS:
        case EfiMemoryMappedIO:
        case EfiMemoryMappedIOPortSpace:
        case EfiUnacceptedMemoryType:
        case EfiPalCode:
        case EfiPersistentMemory:
        case EfiMaxMemoryType:
            return SNOWOS_RESERVED;
            break;
        case EfiLoaderCode:
        case EfiLoaderData:
        case EfiBootServicesCode:
        case EfiBootServicesData:
            return SNOWOS_BOOT_RECLAIM;
            break;
        case EfiConventionalMemory:
            return SNOWOS_FREE_MEMORY;
            break;
        case EfiRuntimeServicesCode:
        case EfiRuntimeServicesData:
            return SNOWOS_RUNTIME_SERVICES;
            break;
        case EfiACPIReclaimMemory:
            return SNOWOS_ACPI_RECLAIM;
            break;
    }
}

EFI_MEMORY_DESCRIPTOR* BlGetMemoryMap(pagemap_t PageMap)
{
    EFI_STATUS ExitStatus;

    UINTN MemoryMapSize = 0;
    EFI_MEMORY_DESCRIPTOR *MemoryMap = NULL;
    UINTN MapKey = 0;
    UINT32 DescriptorVersion = 0;

    // Get the size of the amount of memory we need to Allocate memory for the MemoryMap
    ExitStatus = BlGetSystemTable()->BootServices->GetMemoryMap(
        &MemoryMapSize,
        MemoryMap,
        &MapKey,
        &DescriptorSize,
        &DescriptorVersion);

    if (ExitStatus == EFI_BUFFER_TOO_SMALL)     // If status is not EFI_BUFFER_TOO_SMALL something has gone wrong
    {
        BlDebug(INFO, "Buffer too small, correct size has been returned in memory_map_size\r\n");
    }

    // Allocate memory for MemoryMap
    void *Buffer;
    ExitStatus = BlGetSystemTable()->BootServices->AllocatePool(
        EfiReservedMemoryType,
        (MemoryMapSize + (2 * DescriptorSize)),
        &Buffer);

    if (ExitStatus != EFI_SUCCESS)
    {
        BlDebug(ERROR, "Could not allocate memory for MemoryMap!\r\n");
    }

    MemoryMap = Buffer;

    // Ensure that the Memory Map is identity mapped
    mapPages(
        PageMap,
        ((uint64_t)MemoryMap & ~0xfff),
        ((uint64_t)MemoryMap & ~0xfff),
        0x1,
        0x5000);

    // Get the MemoryMap
    ExitStatus = BlGetSystemTable()->BootServices->GetMemoryMap(
        &MemoryMapSize,
        MemoryMap,
        &MapKey,
        &DescriptorSize,
        &DescriptorVersion);

    if (ExitStatus != EFI_SUCCESS)
    {
        BlDebug(ERROR, "Something went wrong when retrieving MemoryMap!\r\n");
        BlDebug(INFO, "MemoryMapSize: %d | MemoryMap Address: 0x%x | Desc Size: %d\r\n",
            MemoryMapSize,
            MemoryMap,
            DescriptorSize);
    }

    else
    {
        BlDebug(INFO, "MemoryMap was retrieved!\r\n");
        BlDebug(INFO, "MemoryMapSize: %d | MemoryMap Address: 0x%x | Desc Size: %d\r\n",
            MemoryMapSize,
            MemoryMap,
            DescriptorSize);
    }

    NumberOfEntries = MemoryMapSize / DescriptorSize;
    BootloaderMapKey = MapKey;

    return MemoryMap;
}

uint64_t BlGetEntryCount()
{
    return NumberOfEntries;
}

uint64_t BlGetMapKeyey()
{
    return BootloaderMapKey;
}

uint64_t BlGetDescSize()
{
    return DescriptorSize;
}

VOID BlSetMemoryTypes(EFI_MEMORY_DESCRIPTOR *MemoryMap)
{
    SNOWBOOT_MEMORY_DESCRIPTOR *Descriptor;
    for (int i = 0; i < NumberOfEntries; i++)
    {
        Descriptor = (EFI_MEMORY_DESCRIPTOR *)((uint8_t *)MemoryMap + (i * BlGetDescSize()));
        Descriptor->type = UefiTypeToNative(Descriptor->type);
    }
}

void UefiAllocatePool(UINTN size, void *buffer)
{
    EFI_STATUS status;
    status = BlGetSystemTable()->BootServices->AllocatePool(EfiLoaderData, size, &buffer);

    if(status != EFI_SUCCESS)
    {
        BlDebug(ERROR, "Failed to allocate %d bytes of memory!\r\n", size);
    } else BlDebug(INFO, "Allocated %d bytes! Starting address: 0x%x\r\n", size, buffer);
}

void UefiAllocatePages(UINTN pages, uint64_t *memory, EFI_MEMORY_TYPE mem_type)
{
    EFI_STATUS status;
    status = BlGetSystemTable()->BootServices->AllocatePages(AllocateAnyPages, mem_type, pages, memory);

    if(status != EFI_SUCCESS)
    {
        BlDebug(ERROR, "Failed to allocate %d pages!\r\n", pages);
        switch (status)
        {
            case EFI_OUT_OF_RESOURCES:
                BlDebug(ERROR, "Out of resources!\r\n");
                break;
            case EFI_INVALID_PARAMETER:
                BlDebug(ERROR, "Invalid Parameter!\r\n");
                break;
            case EFI_NOT_FOUND:
                BlDebug(ERROR, "Page not found!\r\n");
                break;
        }
    }
}