#include "inc/print.h"
#include "inc/globals.h"
#include <stdarg.h>

VOID BlPrint(CHAR16 *Format, ...) {
    CHAR16 C[2];
    INT64 IntToPrint;
    INT64 Number[100];
    INT64 j;
    va_list args;

    C[0] = u'\0', C[1] = u'\0';

    va_start(args, Format);

    for (int i = 0; Format[i] != u'\0'; i++) {
        if (Format[i] == u'%') {
            i++; // Get character after %

            switch (Format[i]) {
                case 'c':
                    C[0] = va_arg(args, int);
                    BlGetSystemTable()->ConOut->OutputString(BlGetSystemTable()->ConOut, C);
                    break;
                case 'd':
                    IntToPrint = va_arg(args, int);
                    j = 0;
                    do {
                        Number[j] = (IntToPrint % 10);
                        IntToPrint = (IntToPrint - IntToPrint % 10) / 10;
                        j++;
                    }
                    while(IntToPrint > 0);

                    j--;

                    for (; j>=0; j--) {
                        C[0] = Number[j] + '0';
                        BlGetSystemTable()->ConOut->OutputString(BlGetSystemTable()->ConOut, C);
                    }

                    continue;
                default:
                    break;
            }
        }

        else {
            C[0] = Format[i];
            BlGetSystemTable()->ConOut->OutputString(BlGetSystemTable()->ConOut, C);
        }
    }

    va_end(args);

}


VOID BlPrintEfiStatus(EFI_STATUS Status) {
    switch (Status) {
        case EFI_SUCCESS:
            BlPrint(u"EFI_SUCCESS\r\n");
            break;
        case EFI_INVALID_PARAMETER:
            BlPrint(u"EFI_INVALID_PARAMETER\r\n");
            break;
        case EFI_NOT_FOUND:
            BlPrint(u"EFI_NOT_FOUND\r\n");
            break;
        default:
            BlPrint(u"Not implemented!");
            break;
    }   
}