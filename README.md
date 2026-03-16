# shitOS

### запуск этой шняги

**кланируем реп**

```bash
git clone https://github.com/xHak2215/shitOS
cd shitOS
```

**устанавливаем всякое нужное**
```bash
sudo apt update
sudo apt install nasm gcc gcc-multilib qemu-system-x86 make gcc-multilib grub-pc-bin xorriso mtools qemu-system-x86 grub-efi-amd64-bin
```

**делаем скрипд копмеляции и запуска исплняемым и запускаем его**
```bash
chmod +x build.sh

./build.sh
```

### функции и бибиотеки:
`multi_lib.c`:
- `strlen` считывает синуолы в стоке
- `int_to_str` числа в стоки

