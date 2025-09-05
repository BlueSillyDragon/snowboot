#include <inc/globals.h>
#include <inc/print.h>
#include <inc/log.h>
#include <stdarg.h>
#include <stdint.h>

EFI_SERIAL_IO_PROTOCOL *Serial;

UINT8 SerialSupported;

VOID BlInitSerialServices()
{
    EFI_STATUS ExitStatus;
    EFI_GUID SerialGuid = EFI_SERIAL_IO_PROTOCOL_GUID;

    ExitStatus = BlGetSystemTable()->BootServices->LocateProtocol(&SerialGuid, NULL, (void**)&Serial);

    if (EFI_ERROR(ExitStatus)) {
        BlPrint(u"Unable to locate Serial I/O Protocol!\r\n");
        SerialSupported = 0;
    } else SerialSupported = 1;
}

VOID BlSerialPutchar(CHAR8 C)
{
    UINTN BufferSize = 1;
    char Buffer[1];

    Buffer[0] = C;

    Serial->Write(Serial, &BufferSize, (void *)Buffer);
}

VOID BlDebug(enum DEBUG_TYPE Type, const char* String, ...)
{
    if (!SerialSupported)
    {
        return;
    } 

    UINTN bufSize;

    switch (Type)
    {
        case INFO:
            bufSize = 19;
            Serial->Write(Serial, &bufSize, "[ \033[94mBoot\033[0m ] ");
            break;
        case WARNING:
            bufSize = 22;
            Serial->Write(Serial, &bufSize, "[ \033[33mWarning\033[0m ] ");
            break;
        case ERROR:
            bufSize = 20;
            Serial->Write(Serial, &bufSize, "[ \033[31mError\033[0m ] ");
            break;
        case NONE:
            break;
    }

    uint64_t IntToPrint;
    uint64_t Number[256];
    int J;
    char C;
    va_list Args;

    va_start(Args, String);

    for (int i = 0; String[i] != u'\0'; i++) {
        if (String[i] == u'%') {
            i++; // Get character after %

            switch (String[i]) {
                case 'c':
                    C = va_arg(Args, int);
                    BlSerialPutchar(C);
                    break;
                case 'd':
                    IntToPrint = va_arg(Args, uint64_t);
                    J = 0;
                    do {
                        Number[J] = (IntToPrint % 10);
                        IntToPrint = (IntToPrint - IntToPrint % 10) / 10;
                        J++;
                    }
                    while(IntToPrint > 0);

                    J--;

                    for (; J>=0; J--) {
                        C = Number[J] + '0';
                        BlSerialPutchar(C);
                    }

                    continue;
                case 'x':
                    IntToPrint = va_arg(Args, uint64_t);
                    J = 0;
                    do {
                        Number[J] = (IntToPrint % 16);
                        IntToPrint = (IntToPrint - IntToPrint % 16) / 16;
                        J++;
                    }
                    while(IntToPrint > 0);

                    J--;

                    for (; J>=0; J--) {
                        if (Number[J] > 0x9)
                        {
                            C = Number[J] + ('0' + 7);
                        }

                        else
                        {
                            C = Number[J] + '0';
                        }
                        BlSerialPutchar(C);
                    }

                    continue;
                default:
                    break;
            }
        }

        else {
            BlSerialPutchar(String[i]);
        }
    }

    va_end(Args);
}