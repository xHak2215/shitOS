#!/bin/bash

echo "=== Сборка Simple OS в ISO ==="

# Создаем директории
mkdir -p iso/boot/grub

# Компилируем загрузчик (ELF формат для GRUB)
echo "Компиляция boot.asm..."
nasm -f elf32 boot.asm -o boot.o

# Компилируем ядро
echo "Компиляция kernel.c..."
gcc -fno-stack-protector -m32 -c kernel.c -o kernel.o

# Линкуем загрузчик и ядро (ELF формат для GRUB)
echo "Линковка ядра..."
ld -m elf_i386 -T link.ld -o kernel boot.o kernel.o

# Создаем образ для загрузчика
dd if=/dev/zero of=floppy.img bs=512 count=2880
dd if=boot.o of=floppy.img conv=notrunc 

# Создание ISO-образа
genisoimage -o govnvOS.iso -b floppy.img -no-emul-boot -boot-load-size 4 -boot-info-table iso/

#echo "ISO создан: simple-os.iso"
#echo "Запуск: qemu-system-i386 -cdrom simple-os.iso"

# Запуск
qemu-system-i386 -kernel kernel 

