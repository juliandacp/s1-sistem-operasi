# SentinelOS - Task 2

Seorang mahasiswa bernama SON ditugaskan oleh dosennya untuk membuat sebuah sistem operasi sederhana bernama **SentinelOS**. Karena keterbatasannya, ia meminta bantuan Anda untuk mengerjakan tugas ini. Bantulah SON dengan menyelesaikan tahap-tahap pembuatan SentinelOS berikut ini.

---

## **A. Mempersiapkan Kernel Linux _(10 Points)_**

Tahap pertama adalah membangun kernel Linux minimal yang akan menjadi inti dari SentinelOS. Kernel ini harus dikonfigurasi dengan fitur-fitur penting seperti dukungan 64-bit, multiuser, dan script execution.

**Task:** Unduh dan kompilasi Linux kernel versi 6.1.1 dengan konfigurasi minimal untuk x86_64, pastikan fitur esensial seperti CONFIG_64BIT, CONFIG_MULTIUSER, CONFIG_BINFMT_SCRIPT, dan serial console aktif. Output akhir harus berupa file `bzImage`.

---

## **B. Membangun Sistem File Root Awal _(15 Points)_**

Setelah kernel siap, SON perlu membuat struktur direktori root filesystem (rootfs) sebagai dasar sistem operasi. Filesystem ini akan menjadi lingkungan di mana semua program berjalan saat sistem boot.

**Task:** Buat struktur direktori rootfs lengkap dengan folder-folder sistem: `/bin`, `/dev`, `/etc`, `/proc`, `/sys`, `/tmp`, `/vault`, `/watch`, `/log`, `/home/guardian`, `/home/observer`, `/root`. Copy utilitas BusyBox ke sistem dan buat file notifikasi `/log/NOTICE` (isi terserah).

---

## **C. Mengimplementasikan Sistem Multi-User dan Keamanan _(20 Points)_**

SON menginginkan SentinelOS untuk mendukung multiple user dengan sistem keamanan yang tepat. Setiap user harus memiliki password yang aman dan akses terbatas ke direktori tertentu sesuai dengan role mereka.

**Task:** Buat sistem authentication dengan 3 user yang dilindungi password terenkripsi MD5: root (password: SkyLord), guardian (password: Guard123), observer (password: EyeOpen). Setel permission direktori sesuai user (root bisa akses `/vault`, guardian hanya `/watch` dan `/home/guardian`, observer hanya `/home/observer`).

---

## **D. Uji Coba Boot dan Customization Terminal _(15 Points)_**

Sekarang saatnya menguji apakah SentinelOS dapat boot dengan benar dari kernel bzImage dan initramfs. SON ingin menambahkan init script yang proper dan customization pada terminal prompt agar lebih user-friendly dengan warna yang menarik.

**Task:** Buat init script yang mount filesystem multi-user. Set custom prompt dengan warna default (hijau dan biru). Jalankan QEMU untuk test boot langsung dari bzImage dan pastikan login prompt muncul dengan benar.


## **E. Membuat ISO Bootable dengan Multi-Mode GRUB _(20 Points)_**

Tahap terakhir adalah membuat SentinelOS menjadi ISO bootable yang dapat di-boot dari CDROM/DVDROM. SON ingin menambahkan fitur GRUB yang memungkinkan pengguna memilih mode boot berbeda, dan setiap mode akan mengubah warna prompt untuk memberikan visual feedback yang berbeda.

**Task:** Buat ISO bootable dengan 3 menu GRUB (Mode 1, Mode 2, Mode 3). Setiap menu akan pass parameter `mode` yang berbeda ke kernel. Implementasikan perubahan prompt color berdasarkan mode yang dipilih. 

---

## **F. Membuat System Info Script _(20 Points)_**
SentinelOS sudah berjalan dengan baik, namun SON ingin menambahkan utilitas diagnostik sederhana agar setiap user dapat melihat kondisi sistem secara cepat tanpa harus menjalankan banyak perintah satu per satu.

**Task:** Buat script /bin/sysinfo yang dapat dijalankan oleh semua user. Script harus menampilkan informasi berikut secara terformat: hostname sistem, tanggal dan waktu saat ini, uptime sistem, user yang sedang login beserta direktori home-nya, jumlah proses yang sedang berjalan, serta sisa memori yang tersedia. Pastikan script dapat dieksekusi langsung dengan mengetik sysinfo dari direktori manapun.

---

### **Catatan: untuk pengumpulan bisa push seluruh folder osboot**
<br>

# SentinelOS - Task 2 

A university student named SON has been assigned by his lecturer to create a simple operating system called **SentinelOS**. Due to his limitations, he is asking for your help to complete this assignment. Help SON by completing the following stages of creating SentinelOS.

---

## **A. Preparing the Linux Kernel _(10 Points)_**

The first stage is to build a minimal Linux kernel that will serve as the core of SentinelOS. This kernel must be configured with essential features such as 64-bit support, multi-user support, and script execution.

**Task:** Download and compile Linux kernel version 6.1.1 with a minimal configuration for x86_64, ensuring essential features like CONFIG_64BIT, CONFIG_MULTIUSER, CONFIG_BINFMT_SCRIPT, and serial console are enabled. The final output must be a `bzImage` file.

---

## **B. Building the Initial Root Filesystem _(15 Points)_**

Once the kernel is ready, SON needs to create the directory structure for the root filesystem (rootfs) as the foundation of the operating system. This filesystem will act as the environment where all programs execute when the system boots.

**Task:** Create a complete rootfs directory structure with the following system folders: `/bin`, `/dev`, `/etc`, `/proc`, `/sys`, `/tmp`, `/vault`, `/watch`, `/log`, `/home/guardian`, `/home/observer`, and `/root`. Copy the BusyBox utility into the system and create a notification file at `/log/NOTICE` (content is up to you).

---

## **C. Implementing a Multi-User System and Security _(20 Points)_**

SON wants SentinelOS to support multiple users with an appropriate security system. Each user must have a secure password and restricted access to specific directories based on their role.

**Task:** Create an authentication system with 3 users protected by MD5-encrypted passwords: root (password: SkyLord), guardian (password: Guard123), and observer (password: EyeOpen). Set directory permissions according to the user (root can access `/vault`, guardian only for `/watch` and `/home/guardian`, observer only for `/home/observer`).

---

## **D. Boot Testing and Terminal Customization _(15 Points)_**

Now it is time to test whether SentinelOS can boot properly from the bzImage kernel and initramfs. SON wants to add a proper init script and customize the terminal prompt to make it more user-friendly with attractive colors.

**Task:** Create an init script that mounts the multi-user filesystems. Set a custom prompt with default colors (green and blue). Run QEMU to test booting directly from the bzImage and ensure the login prompt appears correctly.

---

## **E. Creating a Bootable ISO with Multi-Mode GRUB _(20 Points)_**

The final stage is to package SentinelOS into a bootable ISO that can be booted from a CDROM/DVDROM. SON wants to add a GRUB feature that allows users to choose different boot modes, where each mode will change the prompt color to provide distinct visual feedback.

**Task:** Create a bootable ISO with 3 GRUB menus (Mode 1, Mode 2, Mode 3). Each menu must pass a different `mode` parameter to the kernel. Implement prompt color changes based on the selected mode. 

---

## **F. Creating a System Info Script _(20 Points)_**

SentinelOS is running well, but SON wants to add a simple diagnostic utility so that every user can quickly view the system's condition without having to run multiple commands one by one.

**Task:** Create a /bin/sysinfo script that can be executed by all users. The script must display the following information in a formatted manner: system hostname, current date and time, system uptime, the currently logged-in user along with their home directory, the number of running processes, and available memory. Ensure the script can be executed directly by typing sysinfo from any directory.

---

### **Note: for submission you can push the entire osboot folder**
