#include <efi.h>
#include <stdarg.h>
#include <stdint.h>
#include "inc/boot_protocol/snowboot.h"
#include "efi/efidef.h"
#include "efi/efiprot.h"
#include <inc/error.h>
#include "efi/x86_64/efibind.h"
#include "inc/memory_services.h"
#include "inc/print.h"
#include "inc/log.h"
#include "inc/disk_services.h"
#include <inc/fs/filesystem.h>

#define SNOWBOOT_MAJOR 0
#define SNOWBOOT_MINOR 2
#define SNOWBOOT_PATCH 0

EFI_SYSTEM_TABLE *sysT = NULL;
EFI_HANDLE imgH = NULL;

EFI_STATUS status;
EFI_MEMORY_DESCRIPTOR *memoryMap;

uint64_t mapKey;

uint64_t hhdmOffset = 0xFFFF800000000000;

void hlt()
{
    asm volatile ( "hlt" );
}

CHAR16 *errorToString(enum SNOWBOOT_ERROR errorCode) {
    switch(errorCode) {
        case (INITIALIZATON_FAILURE):
            return u"INITIALIZATON_FAILURE";
        case (FILE_NOT_FOUND):
            return u"FILE_NOT_FOUND";
        case (VOLUME_OPEN_FAILURE):
            return u"VOLUME_OPEN_FAILURE";
        case (DISK_READ_FAILURE):
            return u"DISK_READ_FAILURE";
        case (FILE_READ_FAILURE):
            return u"FILE_READ_FAILURE";
    }
}

void bpanic(enum SNOWBOOT_ERROR errorCode) {
    //sysT->ConOut->ClearScreen(sysT->ConOut);
    print(u"Error code: ");
    print(errorToString(errorCode));
    sysT->ConOut->OutputString(sysT->ConOut, L"\r\nBOOTLOADER PANIC! ABORTING...\r\n");
    sysT->BootServices->Stall(10 * 1000 * 1000);
    
    sysT->BootServices->Exit(imgH, EFI_ABORTED, 0, NULL);
}

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
    sysT = SystemTable;
    imgH = ImageHandle;

    // Clear the screen
    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

    bdebug(INFO, "Initializing Serial Services...\r\n");
    initSerialServices();

    bdebug(INFO, "Initializing Disk Services...\r\n");
    initDiskServices();

    bdebug(INFO, "Initializing Filesystem Services...\r\n");
    if (EFI_ERROR(initFsServices())) {
        bpanic(INITIALIZATON_FAILURE);
    }
    
    print(u"=======================                  Snowboot                    =======================\r\n");
    print(u"1 | Run SnowOS\r\n");
    print(u"2 | Recovery Mode\r\n");
    print(u"======================= Version %d.%d.%d (c) 2023, 2025 BlueSillyDragon =======================\r\n",
        SNOWBOOT_MAJOR,
        SNOWBOOT_MINOR,
        SNOWBOOT_PATCH);
    
    // Search for snowboot.conf
    if (openVolume() != 0) {
        bpanic(VOLUME_OPEN_FAILURE);
    }

    EFI_FILE_PROTOCOL *configFile = openFile(u"EFI\\BOOT\\snowboot.conf", EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
    if (configFile == NULL) {
        print(u"snowboot.conf could not be found!\r\n");
        bpanic(FILE_NOT_FOUND);
    }

    char *buffer = NULL;
    if (readFile(configFile, (void *)buffer) != 0) {
        bpanic(FILE_READ_FAILURE);
    }

    for (int i = 0; i < 20; i++) {
        serPutchar(buffer[i]);
    }

    // Shouldn't ever reach here, but if we do, panic
    for(;;) {
        hlt();
    }
}