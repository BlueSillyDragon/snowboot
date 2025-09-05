#pragma once

#include "efi.h"

VOID BlPrint(CHAR16 *Format, ...);
VOID BlPrintEfiStatus(EFI_STATUS Status);