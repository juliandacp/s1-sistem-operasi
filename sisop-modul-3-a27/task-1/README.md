# NgeBOOT - Task 1

Kamu adalah seorang mahasiswa Teknik Informatika yang sedang rebahan di kasur, menikmati hari sepulang kuliah. Saat asik doomscrolling, tiba-tiba notifikasi muncul. **"PRAKTIKUM MODUL 3 SISOP TELAH DIMULAI, SELAMAT MENGERJAKAN!"**. Demi bisa balik doomscrolling dengan tenang, kamu bertekad menyelesaikan praktikum ini secepat mungkin.

---

## Struktur Direktori

```
task-1/
├── .config           # Konfigurasi kernel Linux
├── kernel.sh         # Script kompilasi kernel
├── single.sh         # Script pembuatan single-user filesystem
├── multi.sh          # Script pembuatan multi-user filesystem
├── iso.sh            # Script pembuatan ISO bootable
├── qemu.sh           # Script menjalankan QEMU
├── backup.sh         # Script backup file hasil build
└── osboot/           # Direktori output hasil build
```

---

## A. Persiapkan Kernelmu _(15 Points)_

Lengkapi script `kernel.sh` untuk otomasi proses unduh, ekstrak, dan kompilasi kernel Linux, lalu simpan hasilnya di direktori `osboot/`. hapus seluruh direktori dan file sisa kompilasi, hanya sisakan `bzImage`.

> Gunakan file `.config` saat kompilasi

**Run:** `bash kernel.sh`

**Output:** `osboot/bzImage`

---

## B. Single User Filesystem _(15 Points)_

Lengkapi script `single.sh` untuk membangun dan mengompresi filesystem **single-user** menggunakan BusyBox sebagai shell environment, lalu simpan hasilnya di direktori `osboot/`. hapus seluruh direktori dan file sisa build, hanya sisakan `single.gz`.

**Spesifikasi Filesystem:**
- User: hanya `root`
- Direktori: `bin/`, `dev/`, `proc/`, `sys/`
- Root memiliki akses penuh terhadap seluruh direktori

**Run:** `sudo bash single.sh`

**Output:** `osboot/single.gz`

---

## C. Multi User Filesystem _(30 Points)_

Lengkapi script `multi.sh` untuk membangun dan mengompresi filesystem **multi-user** menggunakan BusyBox sebagai shell environment, lalu simpan hasilnya di direktori `osboot/`. hapus seluruh direktori dan file sisa build, hanya sisakan `multi.gz`.

**Spesifikasi Filesystem:**
- User: `root`, `blue`, `green`, `red`, `pink`
- Direktori: `bin/`, `dev/`, `proc/`, `sys/`, `etc/`, `root/`, `home/{blue,green,red,pink}/`
- Password:
  
  | User  | Password  |
  |-------|-----------|
  | root  | root123   |
  | blue  | blue123   |
  | green | green123  |
  | red   | red123    |
  | pink  | pink123   |

- Akses:
  - tiap user memiliki akses penuh terhadap direktori pribadinya
  - root memiliki akses penuh terhadap semua direktori
  - blue (tidak bisa akses /root)
  - green (tidak bisa akses /root, /home/blue)
  - red (tidak bisa akses /root, /home/{blue, green}) 
  - pink (tidak bisa akses /root, /home/{blue,green,red})
  - selain itu hanya bisa read dan execute
- Saat user berhasil login, terminal dibersihkan dan muncul welcome message **"HI [USER]"** dalam format ASCII art

  > [USER] diganti dengan user yang sedang login
  
  _Hint: gunakan text to ASCII Art Generator_

**Run:** `sudo bash multi.sh`

**Output:** `osboot/multi.gz`

---

## D. myISO _(15 Points)_

Lengkapi script `iso.sh` untuk membuat ISO bootable yang memuat kedua filesystem (single dan multi user) dengan GRUB sebagai bootloader, lalu simpan hasilnya di direktori `osboot/`.

**Spesifikasi ISO:**
- Bisa memilih antara single atau multi user pada menu GRUB

**Run: `bash iso.sh`**

**Output:** `osboot/mylinux.iso`

---

## E. Easy QEMU _(15 Points)_

Lengkapi script `qemu.sh` untuk menjalankan QEMU dengan opsi mode boot yang dipilih melalui argumen CLI.

**Penggunaan:**
```
bash qemu.sh --single   # Boot single-user filesystem langsung via kernel
bash qemu.sh --multi    # Boot multi-user filesystem langsung via kernel
bash qemu.sh --all      # Boot dari ISO (tampilkan menu GRUB)
```

---

## F. Jangan Lupa Backup _(10 Points)_

Lengkapi script `backup.sh` untuk membuat arsip ZIP dari file `bzImage`, `single.gz`, `multi.gz`, dan `mylinux.iso`, lalu simpan hasilnya di direktori `osboot/`.

**Format penamaan:**
```
backup_[DDMMYYYY]_[HHMMSS].zip
```

**Contoh output:**
```
backup_03052026_225837.zip
```

---

# NgeBOOT - Task 1

You are a Computer Science student lying in bed, enjoying your day after lectures. While doomscrolling, a notification suddenly pops up. **"PRAKTIKUM MODUL 3 SISOP TELAH DIMULAI, SELAMAT MENGERJAKAN!"**. Determined to get back to doomscrolling in peace, you commit to finishing this practicum as fast as possible.

---

## Directory Structure

```
task-1/
├── .config           # Linux kernel configuration
├── kernel.sh         # Kernel compilation script
├── single.sh         # Single-user filesystem build script
├── multi.sh          # Multi-user filesystem build script
├── iso.sh            # Bootable ISO creation script
├── qemu.sh           # QEMU runner script
├── backup.sh         # Build output backup script
└── osboot/           # Build output directory
```

---

## A. Prepare Your Kernel _(15 Points)_

Complete `kernel.sh` to automate the process of downloading, extracting, and compiling the Linux kernel, then save the result in the `osboot/` directory. Remove all leftover compilation files and directories, keeping only `bzImage`.

> Use the provided `.config` file during compilation

**Run:** `bash kernel.sh`

**Output:** `osboot/bzImage`

---

## B. Single User Filesystem _(15 Points)_

Complete `single.sh` to build and compress a **single-user** filesystem using BusyBox as the shell environment, then save the result in the `osboot/` directory. Remove all leftover build files and directories, keeping only `single.gz`.

**Filesystem Specifications:**
- User: `root` only
- Directories: `bin/`, `dev/`, `proc/`, `sys/`
- Root has full access to all directories

**Run:** `sudo bash single.sh`

**Output:** `osboot/single.gz`

---

## C. Multi User Filesystem _(30 Points)_

Complete `multi.sh` to build and compress a **multi-user** filesystem using BusyBox as the shell environment, then save the result in the `osboot/` directory. Remove all leftover build files and directories, keeping only `multi.gz`.

**Filesystem Specifications:**
- Users: `root`, `blue`, `green`, `red`, `pink`
- Directories: `bin/`, `dev/`, `proc/`, `sys/`, `etc/`, `root/`, `home/{blue,green,red,pink}/`
- Passwords:

  | User  | Password  |
  |-------|-----------|
  | root  | root123   |
  | blue  | blue123   |
  | green | green123  |
  | red   | red123    |
  | pink  | pink123   |

- Access permissions:
  - Each user has full access to their own home directory
  - Root has full access to all directories
  - blue cannot access `/root`
  - green cannot access `/root`, `/home/blue`
  - red cannot access `/root`, `/home/{blue, green}`
  - pink cannot access `/root`, `/home/{blue,green,red}`
  - All other directories are read and execute only
- Upon successful login, the terminal is cleared and a **"HI [USER]"** welcome message is displayed in ASCII art format

  > [USER] is replaced with the currently logged-in user's name

  _Hint: use a text to ASCII Art Generator_

**Run:** `sudo bash multi.sh`

**Output:** `osboot/multi.gz`

---

## D. myISO _(15 Points)_

Complete `iso.sh` to create a bootable ISO containing both filesystems (single and multi user) with GRUB as the bootloader, then save the result in the `osboot/` directory.

**ISO Specifications:**
- Users can select between single or multi user mode from the GRUB boot menu

**Run:** `bash iso.sh`

**Output:** `osboot/mylinux.iso`

---

## E. Easy QEMU _(15 Points)_

Complete `qemu.sh` to run QEMU with a boot mode selected via a CLI argument.

**Usage:**
```
bash qemu.sh --single   # Boot single-user filesystem directly via kernel
bash qemu.sh --multi    # Boot multi-user filesystem directly via kernel
bash qemu.sh --all      # Boot from ISO (display GRUB menu)
```

---

## F. Don't Forget to Backup _(10 Points)_

Complete `backup.sh` to create a ZIP archive of `bzImage`, `single.gz`, `multi.gz`, and `mylinux.iso`, then save it in the `osboot/` directory.

**Naming format:**
```
backup_[DDMMYYYY]_[HHMMSS].zip
```

**Example output:**
```
backup_03052026_225837.zip
```
