#扩展成匹配后面pattern的文件
C_SOURCES = ${wildcard kernel/*.c libc/*.c drivers/*.c cpu/*.c}
HEADERS = ${wildcard kernel/*.h drivers/*.h cpu/*.h libc/*.h}
#变量值替换（var: .c = .o）
OBJ = ${C_SOURCES:.c=.o boot/boot.o cpu/interrupt.o cpu/tables_flush.o}

CFLAGS = -g -std=gnu99 -ffreestanding -Wall -Wextra

myos.iso:isodir myos.elf
	grub-mkrescue -o $@ $<

run: myos.iso
	qemu-system-i386  -nographic -curses -cdrom  $<

debug: myos.iso
	qemu-system-i386 -S -s -curses -cdrom $<



myos.elf:linker.ld  ${OBJ}
	i386-elf-gcc -T ./linker.ld -o isodir/boot/$@ -ffreestanding $^ -nostdlib 




#编译成目标文件
%.o : %.c ${HEADERS}
	i386-elf-gcc $(CFLAGS) -c $< -o $@

%.o: %.asm
	nasm $< -felf32 -o $@


clean:
	rm -rf  boot/*.o cpu/*.o drivers/*.o kernel/*.o libc/*.o
	rm isodir/boot/*.elf
	rm *.iso
