#pragma once

#include <efi.h>

enum DEBUG_TYPE {INFO, WARNING, ERROR, NONE};

void BlInitSerialServices();

void BlSerialPutchar(CHAR8 C);

void BlDebug(enum DEBUG_TYPE Type, const char *String, ...);