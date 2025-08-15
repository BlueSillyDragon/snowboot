#pragma once
#include <efi.h>
#include <stdint.h>

EFI_STATUS initFsServices();

int openVolume();
int openFile(const CHAR16 *filepath, uint64_t mode, uint64_t attributes);