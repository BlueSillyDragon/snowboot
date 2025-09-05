#include <efi.h>
#include <stdint.h>
#include "efi/efiapi.h"
#include "efi/efidef.h"
#include "inc/globals.h"
#include "inc/log.h"
#include "inc/acpi.h"
#include "inc/memory_services.h"

EFI_GUID Acpi = ACPI_TABLE_GUID;
EFI_GUID Acpi2 = ACPI_20_TABLE_GUID;

VOID BlRetrieveAcpiTables()
{
    // Check the ACPI version
    const VOID *PAcpi = &Acpi;
    const VOID *PAcpi2 = &Acpi2;
    VOID *vendorGuid = &BlGetSystemTable()->ConfigurationTable[9].VendorGuid;

    BlDebug(INFO, "Number of table entries %d\r\n", BlGetSystemTable()->NumberOfTableEntries);

    for (UINT64 i = 0; i < BlGetSystemTable()->NumberOfTableEntries; i++)
    {
        if (memcmp(vendorGuid, PAcpi, 128) == 0)
        {
            BlDebug(INFO, "Found ACPI 1.0!\r\n");
        }

        else if (memcmp(vendorGuid, PAcpi2, 128) == 0)
        {
            BlDebug(INFO, "Found ACPI 2.0!\r\n");
        }

        BlDebug(INFO, "Vendor Guid 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\r\n",
            BlGetSystemTable()->ConfigurationTable[i].VendorGuid.Data1,
            BlGetSystemTable()->ConfigurationTable[i].VendorGuid.Data2,
            BlGetSystemTable()->ConfigurationTable[i].VendorGuid.Data3,
            BlGetSystemTable()->ConfigurationTable[i].VendorGuid.Data4[0],
            BlGetSystemTable()->ConfigurationTable[i].VendorGuid.Data4[1],
            BlGetSystemTable()->ConfigurationTable[i].VendorGuid.Data4[2],
            BlGetSystemTable()->ConfigurationTable[i].VendorGuid.Data4[3],
            BlGetSystemTable()->ConfigurationTable[i].VendorGuid.Data4[4],
            BlGetSystemTable()->ConfigurationTable[i].VendorGuid.Data4[5],
            BlGetSystemTable()->ConfigurationTable[i].VendorGuid.Data4[6],
            BlGetSystemTable()->ConfigurationTable[i].VendorGuid.Data4[7]);
    }
}