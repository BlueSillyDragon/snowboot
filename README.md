# snowboot
snowboot is a 64-bit UEFI bootloader targeted for x86 machines. This is the custom bootloader for SnowOS

## How to build snowboot

First, be sure to run the get-deps script
```
$ ./get-deps
```
Then create a build directory and cd into it
```
$ mkdir build
$ cd build
```
Now all you need to do is run CMake and Ninja
```
$ cmake .. -GNinja -DCMAKE_C_COMPILER=clang
$ ninja
```
A weird error might show up when running ninja, idk how to make CMake not do that, so just manually fix it.
Find this line in the build.ninja file
```
dd if=/dev/zero of=snowboot.efi bs=4096 count=0 seek=$$ ( ( ( $$ ( wc -c < snowboot.efi ) + 4095 ) / 4096 ) ) 2>/dev/null
```
Replace everything after 'seek=' with '$$(( ($$(wc -c < snowboot.efi) + 4095) / 4096 )) 2>/dev/null' Now run ninja again, and it should build successfully. It'll put the .efi file in 'build/src/snowboot.efi' at which point it can now be copied to a .img file (or the ESP of a machine)

snowboot is under a LGPL license

Copyright (C) 2023, 2025 BlueSillyDragon