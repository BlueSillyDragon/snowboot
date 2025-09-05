// This header defines structures that AquaBoot will pass to the Kernel
#pragma once

#include <stdint.h>

// Memory Descriptor Types
typedef enum {
    SNOWOS_RESERVED,
    SNOWOS_RUNTIME_SERVICES,
    SNOWOS_ACPI_RECLAIM,
    SNOWOS_BOOT_RECLAIM,
    SNOWOS_FREE_MEMORY
} SNOWBOOT_MEM_TYPE;

typedef struct {
    uint64_t base;
    uint64_t size;
    uint32_t horizontalRes;
    uint32_t verticalRes;
    uint32_t pixelsPerScanline;
    uint32_t pitch;
} SNOWBOOT_FRAMEBUFFER;

typedef struct
{
    uint32_t type;
    uint64_t physicalStart;
    uint64_t virtualStart;
    uint64_t numOfPages;
    uint64_t attribute;
} SNOWBOOT_MEMORY_DESCRIPTOR;

typedef struct
{
    uint32_t SnowbootMajor;    // We'll pass the version of SnowBoot to Kernel for the sake of it
    uint32_t SnowbootMinor;
    uint32_t SnowbootPatch;

    uint64_t Hhdm;  // Pass the Higher-Half Direct Map to the Kernel

    SNOWBOOT_FRAMEBUFFER *Framebuffer;  // Obviously we need to pass the framebuffer to the Kernel

    uint64_t MemoryMapEntries;
    uint64_t DescriptorSize;
    SNOWBOOT_MEMORY_DESCRIPTOR *memoryMap;

    uint64_t KernelPhysicalAddress;  // Pass the physical address of the kernel so it can remap itself
} SNOWBOOT_INFO;