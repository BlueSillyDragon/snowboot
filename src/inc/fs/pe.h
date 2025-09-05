#pragma once

#include <efi.h>

#define DOS_PE_HEADER_OFFSET 0x3c
#define PE_SIGNATURE 0x4550
#define IMAGE_FILE_MACHINE_AMD64 0x8664

// DOS Header struct taken from Limine, thank you Mint XD

#define DOS_SIGNATURE 0x5a4d

typedef struct _IMAGE_FILE_DOS_HEADER
{
    UINT16 e_magic;
    UINT16 e_cblp;
    UINT16 e_cp;
    UINT16 e_crlc;
    UINT16 e_cparhdr;
    UINT16 e_minalloc;
    UINT16 e_maxalloc;
    UINT16 e_ss;
    UINT16 e_sp;
    UINT16 e_csum;
    UINT16 e_ip;
    UINT16 e_cs;
    UINT16 e_lfarlc;
    UINT16 e_ovno;
    UINT16 e_res[4];
    UINT16 e_oemid;
    UINT16 e_oeminfo;
    UINT16 e_res2[10];
    uint32_t e_lfanew;

} __attribute__((packed)) IMAGE_FILE_DOS_HEADER;

typedef struct _IMAGE_FILE_PE_HEADER
{
    UINT16 Machine;
    UINT16 NumberOfSections;
    UINT16 TimeDateStamp;
    UINT32 PointerToSymbolTable;
    UINT32 NumberOfSymbols;
    UINT16 SizeOfOptionalHeader;
    UINT16 Characteristics;
} __attribute__((packed)) IMAGE_FILE_PE_HEADER;

typedef struct _NT_HEADERS_64
{
    UINT32 Signature;
    IMAGE_FILE_PE_HEADER PeHeader;
} __attribute__((packed)) NT_HEADERS_64;

BOOLEAN BlValidatePeHeader(CHAR8 *Pe);