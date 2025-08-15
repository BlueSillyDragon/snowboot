#include "efi/efierr.h"
#include <efi.h>
#include <inc/globals.h>
#include <inc/log.h>
#include <inc/fs/filesystem.h>
#include <stdint.h>

EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *simpleFs;
EFI_FILE_PROTOCOL *root;

EFI_STATUS initFsServices() {
    EFI_STATUS exitStatus;
    EFI_GUID simpleFsGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;

    EFI_HANDLE *handles = NULL;
    UINTN bufferSize = 0;

    exitStatus = sysT->BootServices->LocateHandle(
        ByProtocol,
        &simpleFsGuid, 
        NULL, 
        &bufferSize,
        handles);
    
    if (exitStatus == EFI_BUFFER_TOO_SMALL)
    {
        sysT->BootServices->AllocatePool(EfiLoaderData, bufferSize, (void**)&handles);
    } else if (exitStatus == EFI_NOT_FOUND) {
        bdebug(ERROR, "Simple Filesystem Protocol is not supported on this device!");
        return exitStatus;
    }

    exitStatus = sysT->BootServices->LocateHandle(
        ByProtocol,
        &simpleFsGuid, 
        NULL, 
        &bufferSize,
        handles);
    
    if (EFI_ERROR(exitStatus))
    {
        bdebug(ERROR, "Could not obtain Simple Filesystem Protocol handles!\r\n");
        return exitStatus;
    }

    for (UINTN i = 0; i < bufferSize / sizeof(EFI_HANDLE); i++) {
        bdebug(INFO, "File System found!\r\n");
    }

    exitStatus = sysT->BootServices->HandleProtocol(handles[0], &simpleFsGuid, (void **)&simpleFs);

    if (EFI_ERROR(exitStatus)) {
        bdebug(ERROR, "Error occured retrieving Simple Filesystem Protocol!\r\n");
        return exitStatus;
    }

    return exitStatus;
}

int openVolume() {
    EFI_STATUS exitStatus;

    exitStatus = simpleFs->OpenVolume(simpleFs, &root);

    if (EFI_ERROR(exitStatus)) {
        return -1;
    }

    return 0;
}

int openFile(const CHAR16 *filepath, uint64_t mode, uint64_t attributes) {
    EFI_STATUS exitStatus;
    EFI_FILE_PROTOCOL *openedFile;

    exitStatus = root->Open(root, &openedFile, filepath, mode, attributes);

    if (EFI_ERROR(exitStatus)) {
        return -1;
    }

    return 0;
}