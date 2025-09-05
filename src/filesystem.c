#include <efi.h>
#include <inc/globals.h>
#include <inc/log.h>
#include <inc/memory_services.h>
#include <inc/fs/filesystem.h>
#include <stddef.h>

EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFs;
EFI_FILE_PROTOCOL *Root;

EFI_STATUS BlInitFsServices() {
    EFI_STATUS ExitStatus;
    EFI_GUID SimpleFsGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;

    EFI_HANDLE *Handles = NULL;
    UINTN BufferSize = 0;

    ExitStatus = BlGetSystemTable()->BootServices->LocateHandle(
        ByProtocol,
        &SimpleFsGuid, 
        NULL, 
        &BufferSize,
        Handles);
    
    if (ExitStatus == EFI_BUFFER_TOO_SMALL)
    {
        BlGetSystemTable()->BootServices->AllocatePool(EfiLoaderData, BufferSize, (void**)&Handles);
    } else if (ExitStatus == EFI_NOT_FOUND) {
        BlDebug(ERROR, "Simple Filesystem Protocol is not supported on this device!");
        return ExitStatus;
    }

    ExitStatus = BlGetSystemTable()->BootServices->LocateHandle(
        ByProtocol,
        &SimpleFsGuid, 
        NULL, 
        &BufferSize,
        Handles);
    
    if (EFI_ERROR(ExitStatus))
    {
        BlDebug(ERROR, "Could not obtain Simple Filesystem Protocol Handles!\r\n");
        return ExitStatus;
    }

    for (UINTN i = 0; i < BufferSize / sizeof(EFI_HANDLE); i++) {
        BlDebug(INFO, "File System found!\r\n");
    }

    ExitStatus = BlGetSystemTable()->BootServices->HandleProtocol(Handles[0], &SimpleFsGuid, (void **)&SimpleFs);

    if (EFI_ERROR(ExitStatus)) {
        BlDebug(ERROR, "Error occured retrieving Simple Filesystem Protocol!\r\n");
        return ExitStatus;
    }

    return ExitStatus;
}

INT32 BlOpenVolume() {
    EFI_STATUS ExitStatus;

    ExitStatus = SimpleFs->OpenVolume(SimpleFs, &Root);

    if (EFI_ERROR(ExitStatus)) {
        return -1;
    }

    return 0;
}

EFI_FILE_PROTOCOL *BlOpenFile(const CHAR16 *Filepath, UINT64 Mode, UINT64 Attributes) {
    EFI_STATUS ExitStatus;
    EFI_FILE_PROTOCOL *OpenedFile;
    
    UefiAllocatePool(sizeof(EFI_FILE_PROTOCOL), (VOID *)OpenedFile);

    ExitStatus = Root->Open(Root, &OpenedFile, (CHAR16 *)Filepath, Mode, Attributes);

    if (EFI_ERROR(ExitStatus)) {
        return NULL;
    }

    return OpenedFile;
}

INT32 BlReadFile(EFI_FILE_PROTOCOL *File, void *ReadBuffer) {
    EFI_STATUS ExitStatus;
    UINTN BufferSize = 0;

    // Get the size of the file

    VOID *Buffer = NULL;

    EFI_FILE_INFO *FileInfo;
    EFI_GUID FileInfoId = EFI_FILE_INFO_ID;

    ExitStatus = File->GetInfo(File, &FileInfoId, &BufferSize, Buffer);

    if (EFI_ERROR(ExitStatus) && ExitStatus != EFI_BUFFER_TOO_SMALL) {
        BlDebug(ERROR, "Error getting file info!\r\n");
        return -1;
    }

    UefiAllocatePool(BufferSize, Buffer);

    ExitStatus = File->GetInfo(File, &FileInfoId, &BufferSize, Buffer);

    if (EFI_ERROR(ExitStatus)) {
        BlDebug(ERROR, "Could not get File Info!\r\n");
        return -1;
    }

    FileInfo = Buffer;

    BlDebug(INFO, "Size of file is: %d bytes\r\n", FileInfo->FileSize);

    // Read the file into ReadBuffer

    UefiAllocatePool(FileInfo->FileSize, ReadBuffer);
    ExitStatus = File->Read(File, &FileInfo->FileSize, ReadBuffer);

    if (EFI_ERROR(ExitStatus)) {
        BlDebug(ERROR, "Could not read file!\r\n");
        return -1;
    }

    return 0;
}