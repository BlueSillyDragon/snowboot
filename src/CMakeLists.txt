set(SOURCES
"../nyu-efi/x86_64/reloc.c"
"../nyu-efi/x86_64/entry.S"
"snowboot.c"
"video_services.c"
"disk_services.c"
"print.c"
"filesystem.c"
"log.c"
"memory_services.c"
"virtual_memory.c"
"acpi.c"
"elf.c"
"pe.c")
add_executable(snowboot ${SOURCES})

target_include_directories(snowboot
PRIVATE ${CMAKE_SOURCE_DIR}/nyu-efi/inc)

target_compile_options(snowboot
PRIVATE $<$<COMPILE_LANGUAGE:C>:-g>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-target x86_64-unknown-none>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-pipe>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-Wall>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-Wextra>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-std=gnu11>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-nostdinc>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-ffreestanding>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-fno-stack-protector>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-fno-stack-check>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-fshort-wchar>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-fno-lto>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-fPIE>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-ffunction-sections>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-fdata-sections>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-I ${CMAKE_SOURCE_DIR}/src>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-isystem ${CMAKE_SOURCE_DIR}/freestnd-c-hdrs>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-m64>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-march=x86-64>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-mno-80387>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-mno-mmx>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-mno-sse>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-mno-sse2>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-mno-red-zone>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-MMD>
PRIVATE $<$<COMPILE_LANGUAGE:C>:-MP>)

target_link_options(snowboot
PRIVATE -fuse-ld=lld
PRIVATE -Wl,-m,elf_x86_64
PRIVATE -nostdlib
PRIVATE -pie
PRIVATE -z text
PRIVATE -Wl,-z,max-page-size=0x1000
PRIVATE -Wl,-gc-sections
PRIVATE -Wl,-T,${CMAKE_SOURCE_DIR}/nyu-efi/x86_64/link_script.lds)

add_custom_target(snowboot.efi ALL
COMMAND llvm-objcopy -O binary snowboot snowboot.efi
COMMAND dd if=/dev/zero of=snowboot.efi bs=4096 count=0 seek=$$(( ($$(wc -c < snowboot.efi) + 4095) / 4096 )) 2>/dev/null
DEPENDS snowboot)