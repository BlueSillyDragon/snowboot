#include "inc/boot_protocol/snowboot.h"
#include "inc/globals.h"
#include "inc/print.h"
#include <inc/log.h>
#include "inc/memory_services.h"
#include "inc/video_services.h"
#include <stdint.h>

EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;
EFI_GUID GopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop;

SNOWBOOT_FRAMEBUFFER *BlInitVideoServices() {
    EFI_STATUS Status;
    SNOWBOOT_FRAMEBUFFER *Framebuffer;
    Status = BlGetSystemTable()->BootServices->LocateProtocol(&GopGuid, NULL, (VOID**)&Gop);

    if (EFI_ERROR(Status)) {
        BlPrint(u"Could not locate GOP, you may be using a pre-UEFI machine, try using a BIOS build of SnowBoot instead.\r\n");
        asm volatile ( "hlt" );
    }

    UINTN SizeOfInfo, NumberOfModes, NativeMode;

    Status = Gop->QueryMode(Gop, Gop->Mode == NULL ? 0:Gop->Mode->Mode, &SizeOfInfo, &Info);

    if (Status == EFI_NOT_STARTED) {
        Gop->SetMode(Gop, 0);
    }

    if (EFI_ERROR(Status)) {
        BlDebug(ERROR, "Unable to retrieve native mode!\n\r");
        asm volatile ( "hlt" );
    }

    else {
        NativeMode = Gop->Mode->Mode;
        NumberOfModes = Gop->Mode->MaxMode;
        BlDebug(INFO, "Video mode retrieved!\n\r");
    }

    // Set video mode and get frame buffer

    Status = Gop->SetMode(Gop, 10);

    if (EFI_ERROR(Status)) {
        BlDebug(ERROR, "Could not set video mode! Defaulting to native...\r\n");
        Status = Gop->SetMode(Gop, 0);
        if (EFI_ERROR(Status)) {
            BlDebug(ERROR, "Failure to set video mode! Halting...\r\n");
            asm volatile ( "hlt" );
        }
    }

    UINT64 FramebufferAddress;
    UefiAllocatePages(1, &FramebufferAddress, EfiReservedMemoryType);
    Framebuffer = (SNOWBOOT_FRAMEBUFFER *)FramebufferAddress;

    Framebuffer->base = Gop->Mode->FrameBufferBase;
    Framebuffer->size = Gop->Mode->FrameBufferSize;
    Framebuffer->horizontalRes = Gop->Mode->Info->HorizontalResolution;
    Framebuffer->verticalRes = Gop->Mode->Info->VerticalResolution;
    Framebuffer->pixelsPerScanline = Gop->Mode->Info->PixelsPerScanLine;
    Framebuffer->pitch = (Framebuffer->pixelsPerScanline * 4);

    return Framebuffer;
}

VOID BlPlotPixels (SNOWBOOT_FRAMEBUFFER *Framebuffer, INT32 X, INT32 Y, UINT32 Pixel) {
    volatile UINT32 *FramebufferAddress = (UINT32 *)Framebuffer->base;

    FramebufferAddress[X * (Framebuffer->pitch / 4) + Y] = Pixel;
}

VOID BlChangeBackgroundColor(SNOWBOOT_FRAMEBUFFER *Framebuffer, UINT32 BackgroundColor) {

    for (UINT32 i = 0; i < Framebuffer->verticalRes; i++) {
        for (UINT32 j = 0; j < Framebuffer->horizontalRes; j++) {
            BlPlotPixels(Framebuffer,i, j, BackgroundColor);
        }
    }

}