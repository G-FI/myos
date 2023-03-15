#扩展成匹配后面pattern的文件
C_SOURCES = ${wildcard kernel/*.c libc/*.c drivers/*.c cpu/*.c}
HEADERS = ${wildcard kernel/*.h drivers/*.h cpu/*.h libc/*.h}
#变量值替换（var: .c = .o）
OBJ = ${C_SOURCES:.c=.o boot/boot.o cpu/interrupt.o cpu/tables_flush.o kernel/process.o}

CFLAGS = -g -std=gnu99 -ffreestanding -Wall -Wextra

complier=i686-elf-gcc

myos.elf:linker.ld  ${OBJ}
	${complier} -T ./linker.ld -o isodir/boot/$@ -ffreestanding $^ -nostdlib 


##添加ramdisk之后，使用tutorial 提供的floopy.img 因此从软盘启动
run: myos.elf makefloppy
	qemu-system-i386 -fda ./floppy.img

debug: myos.elf makefloppy
	qemu-system-i386 -S -s -fda ./floppy.img
	#qemu-system-i386 -S -s -kernel isodir/boot/$<

makefloppy:
	/bin/bash ./update_image.sh

#编译成目标文件
%.o : %.c ${HEADERS}
	${complier} $(CFLAGS) -c $< -o $@

%.o: %.asm
	nasm $< -felf32 -o $@


clean:
	rm -rf  boot/*.o cpu/*.o drivers/*.o kernel/*.o libc/*.o
	rm isodir/boot/*.elf
