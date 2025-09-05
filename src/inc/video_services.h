#pragma once
#include "inc/boot_protocol/snowboot.h"
#include <efi.h>

#define AQUABOOT_BG 0x2B60DE

SNOWBOOT_FRAMEBUFFER *BlInitVideoServices();
VOID BlPlotPixels (SNOWBOOT_FRAMEBUFFER *Framebuffer, INT32 X, INT32 Y, UINT32 Pixel);
VOID BlChangeBackgroundColor(SNOWBOOT_FRAMEBUFFER *Framebuffer, UINT32 BackgroundColor);