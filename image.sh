dd if=/dev/zero of=myOS.img bs=1048576 count=256
sgdisk myOS.img -o --new=1 --typecode=1:ef00 -w
sudo losetup --offset 1048576 --sizelimit 67108864 /dev/loop0 myOS.img
sudo mkfs.fat -F 32 /dev/loop0
sudo mount /dev/loop0 /mnt
sudo mkdir /mnt/EFI
sudo mkdir /mnt/EFI/BOOT
sudo cp build/src/snowboot.efi /mnt/EFI/BOOT/BOOTX64.EFI
sudo cp snowboot.conf /mnt/EFI/BOOT/
sudo umount /mnt
sudo losetup -d /dev/loop0
echo "Finished Generating Image!"