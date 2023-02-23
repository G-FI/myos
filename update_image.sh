#!/bin/bash

sudo losetup /dev/loop0 floppy.img
sudo mount /dev/loop0 ./mnt
sudo cp isodir/boot/myos.elf ./mnt/kernel
sudo cp initrd.img ./mnt/initrd
sudo umount /dev/loop0
sudo losetup -d /dev/loop0
