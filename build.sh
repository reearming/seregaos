
#!/bin/bash

set -e

PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"

KERNEL_DIR="$PROJECT_DIR/kernel"
ISO_ROOT="$PROJECT_DIR/iso_root"
ISO="$PROJECT_DIR/serega.iso"

CC="gcc"
LD="ld"

echo "==> Compiling kernel..."

"$CC" \
    -ffreestanding \
    -fno-stack-protector \
    -fno-pie \
    -fno-pic \
    -m64 \
    -mcmodel=kernel \
    -mno-red-zone \
    -fno-asynchronous-unwind-tables \
    -fno-unwind-tables \
    -c "$KERNEL_DIR/main.c" \
    -o "$KERNEL_DIR/main.o"

"$CC" \
    -ffreestanding \
    -fno-stack-protector \
    -fno-pie \
    -fno-pic \
    -m64 \
    -mcmodel=kernel \
    -mno-red-zone \
    -fno-asynchronous-unwind-tables \
    -fno-unwind-tables \
    -c "$KERNEL_DIR/fbuffer.c" \
    -o "$KERNEL_DIR/fbuffer.o"

echo "==> Linking kernel..."

"$LD" \
    -nostdlib \
    -T "$PROJECT_DIR/linker.ld" \
    -o "$PROJECT_DIR/kernel.elf" \
    "$KERNEL_DIR/main.o" \
    "$KERNEL_DIR/fbuffer.o"

echo "==> Preparing ISO..."

mkdir -p "$ISO_ROOT/boot"
mkdir -p "$ISO_ROOT/EFI/BOOT"

cp "$PROJECT_DIR/kernel.elf" \
   "$ISO_ROOT/boot/kernel.elf"

cp -n /usr/share/limine/limine-bios-cd.bin \
   "$ISO_ROOT/boot/limine-bios-cd.bin"

cp -n /usr/share/limine/limine-uefi-cd.bin \
   "$ISO_ROOT/boot/limine-uefi-cd.bin"

cp -n /usr/share/limine/limine-bios.sys \
   "$ISO_ROOT/boot/limine-bios.sys"

cp -n /usr/share/limine/BOOTX64.EFI \
   "$ISO_ROOT/EFI/BOOT/BOOTX64.EFI"

if [ ! -f "$ISO_ROOT/limine.conf" ]; then
    cat > "$ISO_ROOT/limine.conf" <<'EOF'
timeout: 0

/MyOS
    protocol: limine
    path: boot():/boot/kernel.elf
EOF
fi

echo "==> Creating ISO..."

rm -f "$ISO"

xorriso -as mkisofs \
    -R -r -J \
    -b boot/limine-bios-cd.bin \
    -no-emul-boot \
    -boot-load-size 4 \
    -boot-info-table \
    -hfsplus \
    -apm-block-size 2048 \
    --efi-boot boot/limine-uefi-cd.bin \
    -efi-boot-part \
    --efi-boot-image \
    --protective-msdos-label \
    "$ISO_ROOT" \
    -o "$ISO"

echo "==> Installing Limine BIOS..."

limine bios-install "$ISO"

echo
echo "==> Build complete!"
echo "    Kernel: $PROJECT_DIR/kernel.elf"
echo "    ISO: $ISO"
