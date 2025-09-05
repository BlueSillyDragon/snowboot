#include <efi.h>
#include <inc/error.h>
#include "efi/efiprot.h"
#include "efi/x86_64/efibind.h"
#include "inc/fs/pe.h"
#include "inc/print.h"
#include "inc/log.h"
#include "inc/disk_services.h"
#include <inc/fs/filesystem.h>

#define SNOWBOOT_MAJOR 1
#define SNOWBOOT_MINOR 0
#define SNOWBOOT_PATCH 0

EFI_SYSTEM_TABLE *SysT = NULL;
EFI_HANDLE ImgH = NULL;

EFI_STATUS Status;
EFI_MEMORY_DESCRIPTOR *MemoryMap;

UINT64 MapKey;

const UINT64 HhdmOffset = 0xFFFF800000000000;

void Halt()
{
    asm volatile ( "hlt" );
}

EFI_SYSTEM_TABLE *BlGetSystemTable()
{
    return SysT;
}

EFI_HANDLE BlGetImageHandle()
{
    return ImgH;
}

CHAR16 *BlErrorToString(enum SNOWBOOT_ERROR ErrorCode)
{
    switch(ErrorCode)
    {
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
        case (INVALID_HEADER):
            return u"INVALID_HEADER";
    }
}

VOID BlPanic(enum SNOWBOOT_ERROR ErrorCode, CHAR16* ExtraInfo)
{
    BlGetSystemTable()->ConOut->ClearScreen(BlGetSystemTable()->ConOut);
    BlPrint(u"Snowboot ran into a fatal error!\r\n\r\n");

    BlPrint(u"Error code: ");
    BlPrint(BlErrorToString(ErrorCode));

    if (ExtraInfo != NULL)
    {
        BlPrint(u"\r\nExtra Info: ");
        BlPrint(ExtraInfo);
    }

    BlGetSystemTable()->ConOut->OutputString(BlGetSystemTable()->ConOut, L"\r\nAborting...\r\n");
    BlGetSystemTable()->BootServices->Stall(10 * 1000 * 1000);
    
    BlGetSystemTable()->BootServices->Exit(BlGetImageHandle(), EFI_ABORTED, 0, NULL);
}

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
    SysT = SystemTable;
    ImgH = ImageHandle;

    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

    BlInitSerialServices();

    BlDebug(INFO, "Initializing Disk Services...\r\n");
    BlInitDiskServices();

    BlDebug(INFO, "Initializing Filesystem Services...\r\n");
    if (EFI_ERROR(BlInitFsServices()))
    {
        BlPanic(INITIALIZATON_FAILURE, NULL);
    }
    
    BlPrint(u"=======================                  Snowboot                    =======================\r\n");
    BlPrint(u"1 | Run SnowOS\r\n");
    BlPrint(u"2 | Recovery Mode\r\n");
    BlPrint(u"======================= Version %d.%d.%d (c) 2023, 2025 BlueSillyDragon =======================\r\n",
        SNOWBOOT_MAJOR,
        SNOWBOOT_MINOR,
        SNOWBOOT_PATCH);
    
    // Search for snowboot.conf
    if (BlOpenVolume() != 0)
    {
        BlPanic(VOLUME_OPEN_FAILURE, NULL);
    }

    EFI_FILE_PROTOCOL *ConfigFile = BlOpenFile(
        u"EFI\\BOOT\\snowboot.conf", 
        EFI_FILE_MODE_READ, 
        EFI_FILE_READ_ONLY);

    if (ConfigFile == NULL)
    {
        BlPanic(FILE_NOT_FOUND, u"snowboot.conf missing!");
    }

    CHAR *Buffer = NULL;
    if (BlReadFile(ConfigFile, (VOID *)Buffer) != 0)
    {
        BlPanic(FILE_READ_FAILURE, u"snowboot.conf could not be read!");
    }

    for (INT32 i = 0; i < 20; i++)
    {
        BlSerialPutchar(Buffer[i]);
    }
    BlSerialPutchar('\n');

    /*EFI_FILE_PROTOCOL *Kernel = BlOpenFile(u"kernel.exe", EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
    if (Kernel == NULL)
    {
        BlPanic(FILE_NOT_FOUND, u"kernel.exe missing!");
    }

    CHAR8 *KernelReadBuffer = NULL;
    if (BlReadFile(Kernel, (VOID *)KernelReadBuffer) != 0)
    {
        BlPanic(FILE_READ_FAILURE, u"kernel.exe could not be read!");
    }

    if (!BlValidatePeHeader(KernelReadBuffer))
    {
        BlPanic(INVALID_HEADER, u"PE Header of kernel.exe could not be validated. May be corrupted!");
    }*/

    for(;;)
    {
        Halt();
    }
}