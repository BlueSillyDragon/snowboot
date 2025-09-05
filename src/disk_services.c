#include "inc/globals.h"
#include "inc/disk_services.h"
#include "inc/disk_services.h"
#include "inc/log.h"
#include <stdint.h>

static DISK Disks[10];
static DISK Parts[10];

INT32 DiskCount = 0;
INT32 PartCount = 0;

void BlDetectDisks()
{

    EFI_STATUS Status;

    EFI_GUID BlockIoGuid = EFI_BLOCK_IO_PROTOCOL_GUID;
    EFI_GUID DiskIoGuid = EFI_DISK_IO_PROTOCOL_GUID;

    EFI_HANDLE *Handles = NULL;
    UINTN BufferSize = 0;

    Status = BlGetSystemTable()->BootServices->LocateHandle(                       // First we call LocateProtocol with a buffer size of 0
        ByProtocol,                                                         // to figure out how much space we need to allocate for it
        &BlockIoGuid, 
        NULL, 
        &BufferSize,
        Handles);
    
    if (Status == EFI_BUFFER_TOO_SMALL)
    {
        BlGetSystemTable()->BootServices->AllocatePool(EfiLoaderData, BufferSize, (void**)&Handles);
    }

    Status = BlGetSystemTable()->BootServices->LocateHandle(
        ByProtocol,
        &BlockIoGuid, 
        NULL, 
        &BufferSize,
        Handles);

    if (EFI_ERROR(Status))
    {
        BlDebug(ERROR, "Could not obtain Block IO Handles!\r\n");
        for (;;);
    }

    for(INT32 i = 0; (UINTN)i < BufferSize / sizeof(EFI_HANDLE); ++i)
    {
        EFI_BLOCK_IO_PROTOCOL *BlockIo = NULL;
        EFI_DISK_IO_PROTOCOL *DiskIo = NULL;
        PDISK Disk;
        PDISK Partition;
        Status = BlGetSystemTable()->BootServices->HandleProtocol(Handles[i], &BlockIoGuid, (void**)&BlockIo);

        // Save the partition so we can probe it later
        if (BlockIo->Media->LogicalPartition)
        {
            Status = BlGetSystemTable()->BootServices->HandleProtocol(Handles[i], &DiskIoGuid, (void**)&DiskIo);

            if (EFI_ERROR(Status))
            {
                BlDebug(WARNING, "Partition %d does not support Disk I/O\r\n", PartCount);
            }

            Partition = &Parts[PartCount];
            Partition->BlockIo = BlockIo;
            Partition->DiskIo = DiskIo;
            Partition->Sectors = BlockIo->Media->LastBlock + 1;
            Partition->BlockSize = BlockIo->Media->BlockSize;
            Partition->Id = i;
            PartCount++;
            continue;
        }

        else if (!BlockIo->Media->MediaPresent)
        {
            continue;
        }

        else if (EFI_ERROR(Status))
        {
            BlDebug(ERROR, "Error Obtaining Block I/O Protocol!\r\n");
            continue;
        }

        Status = BlGetSystemTable()->BootServices->HandleProtocol(Handles[i], &DiskIoGuid, (void**)&DiskIo);

        if (EFI_ERROR(Status))
        {
            BlDebug(ERROR, "Error Obtaining Disk I/O Protocol!\r\n");
            continue;
        }

        Disk = &Disks[i];
        Disk->BlockIo = BlockIo;
        Disk->DiskIo = DiskIo;
        Disk->Sectors = BlockIo->Media->LastBlock + 1;
        Disk->BlockSize = BlockIo->Media->BlockSize;
        Disk->Id = i;
        DiskCount++;
    }

    BlDebug(INFO, "Number of disks is: %d\r\n", DiskCount);
}

int BlGetDiskCount()
{
    return DiskCount;
}

int BlGetPartCount()
{
    return PartCount;
}

VOID BlReadDisk(INT32 Index, UINT64 Offset, INT32 Bytes, VOID *Buffer)
{
    EFI_STATUS Status;
    PDISK Disk;

    Disk = &Disks[Index];

    Status = Disk->DiskIo->ReadDisk(Disk->DiskIo, Disk->BlockIo->Media->MediaId, Offset, Bytes, &Buffer);

    if (EFI_ERROR(Status))
    {
        BlDebug(ERROR, "Could not read from disk: %d\r\n", Index);
        Buffer = NULL;
    }
}

void BlReadPart(INT32 Index, UINT64 Offset, INT32 Bytes, VOID *Buffer)
{
    EFI_STATUS Status;
    PDISK Partition;

    Partition = &Parts[Index];

    BlDebug(INFO, "offset: %d\r\n", Offset);
    Status = Partition->DiskIo->ReadDisk(Partition->DiskIo, Partition->BlockIo->Media->MediaId, Offset, Bytes, Buffer);

    if (EFI_ERROR(Status))
    {
        BlDebug(ERROR, "Could not read from partition: %d\r\n", Index);
    }
}

void BlInitDiskServices(void)
{
    BlDetectDisks();
}