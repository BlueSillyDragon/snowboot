#pragma once
#include <efi.h>
#include <inc/types.h>

EFI_STATUS BlInitFsServices();

int BlOpenVolume();
EFI_FILE_PROTOCOL *BlOpenFile(const CHAR16 *Filepath, UINT64 Mode, UINT64 Attributes);
int BlReadFile(EFI_FILE_PROTOCOL *File, void *ReadBuffer);