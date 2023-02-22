#扩展成匹配后面pattern的文件
C_SOURCES = ${wildcard kernel/*.c libc/*.c drivers/*.c cpu/*.c}
HEADERS = ${wildcard kernel/*.h drivers/*.h cpu/*.h libc/*.h}
#变量值替换（var: .c = .o）
OBJ = ${C_SOURCES:.c=.o boot/boot.o cpu/interrupt.o cpu/tables_flush.o}

CFLAGS = -g -std=gnu99 -ffreestanding -Wall -Wextra

complier=i686-elf-gcc

##直接从内核文件启动，而不需要启动介质cdrom
run: myos.elf
	qemu-system-i386 -kernel isodir/boot/$<

debug: myos.elf
	qemu-system-i386 -S -s -kernel isodir/boot/$<



myos.elf:linker.ld  ${OBJ}
	${complier} -T ./linker.ld -o isodir/boot/$@ -ffreestanding $^ -nostdlib 


#编译成目标文件
%.o : %.c ${HEADERS}
	${complier} $(CFLAGS) -c $< -o $@

%.o: %.asm
	nasm $< -felf32 -o $@


clean:
	rm -rf  boot/*.o cpu/*.o drivers/*.o kernel/*.o libc/*.o
	rm isodir/boot/*.elf
	rm *.iso
