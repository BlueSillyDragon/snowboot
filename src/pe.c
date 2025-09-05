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

    BlDebug(NONE, "PE Header Info\r\n\r\n");
    BlDebug(NONE, "Machine: 0x%x\r\n", NtHeaders->PeHeader.Machine);
    BlDebug(NONE, "Number of Sections: 0x%x\r\n", NtHeaders->PeHeader.NumberOfSections);
    BlDebug(NONE, "Time & Date Stamp: 0x%x\r\n", NtHeaders->PeHeader.TimeDateStamp);
    BlDebug(NONE, "Pointer to Symbol Table: 0x%x\r\n", NtHeaders->PeHeader.PointerToSymbolTable);
    BlDebug(NONE, "Number of Symbols: 0x%x\r\n", NtHeaders->PeHeader.NumberOfSymbols);
    BlDebug(NONE, "Size of Optional Header: 0x%x\r\n", NtHeaders->PeHeader.SizeOfOptionalHeader);
    BlDebug(NONE, "Characteristics: 0x%x\r\n", NtHeaders->PeHeader.Characteristics);

    if (NtHeaders->PeHeader.Machine != IMAGE_FILE_MACHINE_AMD64)
    {
        BlDebug(ERROR, "Not an x64 PE!\r\n");
        return FALSE;
    }

    return TRUE;
}