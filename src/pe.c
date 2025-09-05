#include <efi.h>
#include <inc/log.h>
#include <inc/globals.h>
#include <inc/fs/pe.h>

BOOLEAN BlValidatePeHeader(CHAR8 *Pe)
{
    IMAGE_FILE_DOS_HEADER *DosHeader = (IMAGE_FILE_DOS_HEADER *)Pe;

    if (DosHeader->e_magic != DOS_SIGNATURE)
    {
        BlDebug(ERROR, "Could not verify DOS Header!\r\n");
        return FALSE;
    }

    NT_HEADERS_64 *NtHeaders = (NT_HEADERS_64 *)(Pe + DosHeader->e_lfanew);

    if (NtHeaders->Signature != PE_SIGNATURE)
    {
        BlDebug(ERROR, "Could not verify PE Header!\r\n");
        return FALSE;
    }

    BlDebug(NONE, "=== PE Header Info ===\r\n\r\n");
    BlDebug(NONE, "Machine: 0x%x\r\n", NtHeaders->PeHeader.Machine);
    BlDebug(NONE, "Number of Sections: 0x%x\r\n", NtHeaders->PeHeader.NumberOfSections);
    BlDebug(NONE, "Time & Date Stamp: 0x%x\r\n", NtHeaders->PeHeader.TimeDateStamp);
    BlDebug(NONE, "Pointer to Symbol Table: 0x%x\r\n", NtHeaders->PeHeader.PointerToSymbolTable);
    BlDebug(NONE, "Number of Symbols: 0x%x\r\n", NtHeaders->PeHeader.NumberOfSymbols);
    BlDebug(NONE, "Size of Optional Header: 0x%x\r\n", NtHeaders->PeHeader.SizeOfOptionalHeader);
    BlDebug(NONE, "Characteristics: 0x%x\r\n\r\n", NtHeaders->PeHeader.Characteristics);

    if (NtHeaders->PeHeader.Machine != IMAGE_FILE_MACHINE_AMD64)
    {
        BlDebug(ERROR, "Not an x64 PE!\r\n");
        return FALSE;
    }

    if (NtHeaders->OptionalHeaders.Magic != OPTIONAL_HEADER_MAGIC)
    {
        BlDebug(ERROR, "Invalid Optional Headers!\r\n");
        return FALSE;
    }

    BlDebug(NONE, "=== Optional Header (PE32+) Info ===\r\n\r\n");
    BlDebug(NONE, "Magic: 0x%x\r\n", NtHeaders->OptionalHeaders.Magic);
    BlDebug(NONE, "Linker Version: %d.%d\r\n", NtHeaders->OptionalHeaders.MajorLinkerVersion, NtHeaders->OptionalHeaders.MinorLinkerVersion);
    BlDebug(NONE, "SizeOfCode: 0x%x\r\n", NtHeaders->OptionalHeaders.SizeOfCode);
    BlDebug(NONE, "SizeOfInitializedData: 0x%x\r\n", NtHeaders->OptionalHeaders.SizeOfInitializedData);
    BlDebug(NONE, "SizeOfUninitializedData: 0x%x\r\n", NtHeaders->OptionalHeaders.SizeOfUninitializedData);
    BlDebug(NONE, "AddressOfEntryPoint: 0x%x\r\n", NtHeaders->OptionalHeaders.AddressOfEntryPoint);
    BlDebug(NONE, "BaseOfCode: 0x%x\r\n", NtHeaders->OptionalHeaders.BaseOfCode);
    BlDebug(NONE, "ImageBase: 0x%x\r\n", NtHeaders->OptionalHeaders.ImageBase);
    BlDebug(NONE, "SectionAlignment: 0x%x\r\n", NtHeaders->OptionalHeaders.SectionAlignment);
    BlDebug(NONE, "FileAlignment: 0x%x\r\n", NtHeaders->OptionalHeaders.FileAlignment);
    BlDebug(NONE, "OS Version: %d.%d\r\n", NtHeaders->OptionalHeaders.MajorOperatingSystemVersion, NtHeaders->OptionalHeaders.MinorOperatingSystemVersion);
    BlDebug(NONE, "Subsystem Version: %d.%d\r\n", NtHeaders->OptionalHeaders.MajorSubsystemVersion, NtHeaders->OptionalHeaders.MinorSubsystemVersion);
    BlDebug(NONE, "SizeOfImage: 0x%x\r\n", NtHeaders->OptionalHeaders.SizeOfImage);
    BlDebug(NONE, "SizeOfHeaders: 0x%x\r\n", NtHeaders->OptionalHeaders.SizeOfHeaders);
    BlDebug(NONE, "Subsystem: 0x%x\r\n", NtHeaders->OptionalHeaders.Subsystem);
    BlDebug(NONE, "DLL Characteristics: 0x%x\r\n", NtHeaders->OptionalHeaders.DllCharacteristics);
    BlDebug(NONE, "SizeOfStackReserve: 0x%x\r\n", NtHeaders->OptionalHeaders.SizeOfStackReserve);
    BlDebug(NONE, "SizeOfStackCommit: 0x%x\r\n", NtHeaders->OptionalHeaders.SizeOfStackCommit);
    BlDebug(NONE, "SizeOfHeapReserve: 0x%x\r\n", NtHeaders->OptionalHeaders.SizeOfHeapReserve);
    BlDebug(NONE, "SizeOfHeapCommit: 0x%x\r\n", NtHeaders->OptionalHeaders.SizeOfHeapCommit);
    BlDebug(NONE, "NumberOfRvaAndSizes: 0x%x\r\n", NtHeaders->OptionalHeaders.NumberOfRvaAndSizes);

    return TRUE;
}