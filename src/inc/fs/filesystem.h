#pragma once
#include <efi.h>
#include <stdint.h>

EFI_STATUS initFsServices();

int openVolume();
EFI_FILE_PROTOCOL *openFile(const CHAR16 *filepath, uint64_t mode, uint64_t attributes);
int readFile(EFI_FILE_PROTOCOL *file, void *readBuffer);