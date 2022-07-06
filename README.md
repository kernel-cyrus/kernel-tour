Kernel Tour
=============================

An aarch64 qemu based kernel running and debug environment for both linux and android common kernel.

All the stuff are intergrated and configured ready to run.

The envionment include:
- prebuilt aarch64 qemu excutables
- prebuilt aarch64 gdb with tui enabled
- prebuilt aarch64 initramfs image
- support running with linux kernel
- support running with android common kernel
- support debug with gdb for aarch64
- support debug with ddd for aarch64

You can start running and debugging the kernel with gdb or ddd just by the `start-kernel.sh` script.

Host Environment: x86 PC + Ubuntu 20.04

Download Lightbox
-----------------------------
```
git clone https://github.com/kernel-cyrus/lightbox.git
```
Command Format
-----------------------------
```
./start-kernel.sh
    --kernel=<kernel source root path>
    --initrd=<initrd file path>
    --with=<gdb or ddd>
```
Start Linux Kernel
-----------------------------

**Download linux kernel**
```
git clone https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git
```
**Apply a patch to enable debug info**
```
cd linux
git apply .../lightbox/patches/linux-mainline.patch
```
**Build linux kernel**
```
ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- make lightbox_defconfig
ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- make Image scripts_gdb
```
**Run kernel with gdb**
```
./start-kernel --kernel=<kernel source path> --with=gdb
```
**Run kernel with ddd**
```
sudo apt install ddd
./start-kernel --kernel=<kernel source path> --with=ddd
```
**Run with your own initramfs image**
```
./start-kernel --kernel=<kernel source path> --initrd=<initramfs image path>
```
Start Android Common Kernel
-----------------------------

**Download android common kernel**
```
repo init -u https://android.googlesource.com/kernel/manifest -b common-android-mainline
repo sync
```
**Apply a patch to enable debug info**
```
cd common
git apply .../lightbox/patches/android-mainline.patch
```
**Build kernel**
```
BUILD_CONFIG=common/build.config.lightbox build/build.sh
```
**Run kernel with gdb**
```
./start-kernel --kernel=<kernel repo path> --with=gdb
```
**Run kernel with ddd**
```
sudo apt install ddd
./start-kernel --kernel=<kernel repo path> --with=ddd
```
**Run with your own initramfs image**
```
./start-kernel --kernel=<kernel repo path> --initrd=<initramfs image path>
```
