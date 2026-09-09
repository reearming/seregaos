#!/usr/bin/env bash

set -e

PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"

KERNEL_DIR="$PROJECT_DIR/kernel"
ISO_ROOT="$PROJECT_DIR/iso_root"
ISO="$PROJECT_DIR/serega.iso"

CC="gcc"
LD="ld"

echo "==> Cleaning..."

rm -rf "$ISO_ROOT"
rm -f "$ISO"

echo "==> Compiling kernel..."

"$CC" \
    -ffreestanding \
    -fno-stack-protector \
    -fno-pie \
    -fno-pic \
    -m64 \
    -mno-red-zone \
    -fno-asynchronous-unwind-tables \
    -fno-unwind-tables \
    -c "$KERNEL_DIR/main.c" \
    -o "$KERNEL_DIR/main.o"

echo "==> Linking kernel..."

"$LD" \
    -nostdlib \
    -T "$PROJECT_DIR/linker.ld" \
    -o "$PROJECT_DIR/kernel.elf" \
    "$KERNEL_DIR/main.o"

echo "==> Preparing ISO..."

mkdir -p "$ISO_ROOT/boot"
mkdir -p "$ISO_ROOT/EFI/BOOT"

cp "$PROJECT_DIR/kernel.elf" \
   "$ISO_ROOT/boot/kernel.elf"

cp /usr/share/limine/limine-bios-cd.bin \
   "$ISO_ROOT/boot/limine-bios-cd.bin"

cp /usr/share/limine/limine-uefi-cd.bin \
   "$ISO_ROOT/boot/limine-uefi-cd.bin"

cp /usr/share/limine/limine-bios.sys \
   "$ISO_ROOT/boot/limine-bios.sys"

cp /usr/share/limine/BOOTX64.EFI \
   "$ISO_ROOT/EFI/BOOT/BOOTX64.EFI"

cat > "$ISO_ROOT/limine.conf" <<'EOF'
timeout: 0

/MyOS
    protocol: limine
    path: boot():/boot/kernel.elf
EOF

echo "==> Creating ISO..."

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
echo "    ISO:    $ISO"

if [[ "$1" == "run" ]]; then
    echo
    echo "==> Starting QEMU..."

    cp /usr/share/edk2/x64/OVMF_VARS.4m.fd \
       /tmp/myos-vars.fd

    qemu-system-x86_64 \
        -M q35 \
        -m 256M \
        -drive if=pflash,format=raw,readonly=on,file=/usr/share/edk2/x64/OVMF_CODE.4m.fd \
        -drive if=pflash,format=raw,file=/tmp/myos-vars.fd \
        -cdrom "$ISO"
fi


if [[ "$1" == "clean" ]]; then
    rm -rf "$ISO_ROOT"
    rm -f "$ISO" "$PROJECT_DIR/kernel.elf" "$KERNEL_DIR/main.o"
    echo "Clean complete."
    exit 0
fi
