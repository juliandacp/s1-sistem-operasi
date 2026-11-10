# MirrorFS

Bagas adalah mahasiswa tingkat akhir yang sedang mengerjakan skripsi. Di samping kesibukannya tersebut, terdapat permasalahan lain yang cukup mengganggu produktivitasnya: laptop yang ia gunakan merupakan perangkat bersama yang dipakai oleh seluruh anggota keluarga, termasuk adik-adiknya.

Suatu malam, Bagas telah selesai merapikan puluhan file laporan penelitiannya. Namun keesokan harinya, ia mendapati beberapa file namanya berubah. Hal tersebut terjadi karena adiknya yang masih duduk di bangku sekolah dasar sedang belajar menggunakan komputer.

Sebagai mahasiswa Sistem Operasi, Bagas memutuskan untuk menyelesaikan permasalahan ini secara teknis. Ia merancang **MirrorFS**, sebuah FUSE filesystem yang mencerminkan folder laporannya dengan tampilan yang seragam, sekaligus mencegah terjadinya modifikasi oleh pihak manapun.

Buatlah program FUSE bernama `mirrorfs.c` yang di-mount pada `/mnt/mirror` dan membaca dari source directory `/home/bagas/reports`. Filesystem ini harus menerapkan beberapa perilaku khusus.

> **Catatan:** Setup awal sudah disiapkan dalam `setup.sh`. Cukup jalankan sekali sebelum mengerjakan.

### A. Mirroring Source Directory _(20 Points)_

FUSE filesystem harus menampilkan isi dari `/home/bagas/reports` secara langsung. Ketika user menjalankan `ls /mnt/mirror`, harus menampilkan file yang sama dengan yang ada di source directory.

```bash
$ ls /home/bagas/reports
laporan_jan.txt  laporan_feb.txt  laporan_mar.txt  data.csv
```

### B. Read-Only Filesystem _(20 Points)_

Seluruh filesystem bersifat **read-only**. Tidak ada pihak yang diizinkan untuk membuat, mengubah, maupun menghapus file di dalam `/mnt/mirror`. Semua operasi write harus gagal dan mengembalikan error `EROFS`.

```bash
$ touch /mnt/mirror/file_baru.txt
touch: cannot touch '/mnt/mirror/file_baru.txt': Read-only file system

$ echo "test" > /mnt/mirror/laporan_jan.txt
bash: /mnt/mirror/laporan_jan.txt: Read-only file system
```

### C. Prefix Nama File _(25 Points)_

Seluruh nama file yang ditampilkan di `/mnt/mirror` harus memiliki prefix `LAPORAN_` guna menyeragamkan penamaan. Namun demikian, ketika file tersebut dibaca, kontennya harus tetap merujuk pada file asli yang terdapat di source directory.

```bash
$ ls /mnt/mirror
LAPORAN_laporan_jan.txt  LAPORAN_laporan_feb.txt  LAPORAN_data.csv

$ cat /mnt/mirror/LAPORAN_laporan_jan.txt
Laporan Bulan Januari     ← isi dari file asli laporan_jan.txt
```

### D. Filter Ekstensi _(35 Points)_

MirrorFS hanya menampilkan file berekstensi `.txt`. File dengan ekstensi lain tidak boleh ditampilkan maupun dapat diakses melalui mount point.

```bash
$ ls /home/bagas/reports
laporan_jan.txt  laporan_feb.txt  data.csv  summary.pdf

$ ls /mnt/mirror
LAPORAN_laporan_jan.txt  LAPORAN_laporan_feb.txt
# data.csv dan summary.pdf tidak muncul
```

## Contoh Skenario Lengkap

```bash
$ ls /home/bagas/reports
laporan_jan.txt  laporan_feb.txt  laporan_mar.txt  data.csv  summary.pdf

$ ls /mnt/mirror
LAPORAN_laporan_jan.txt  LAPORAN_laporan_feb.txt  LAPORAN_laporan_mar.txt

$ cat /mnt/mirror/LAPORAN_laporan_jan.txt
Laporan Bulan Januari

$ touch /mnt/mirror/test.txt
touch: cannot touch '/mnt/mirror/test.txt': Read-only file system

$ cat /mnt/mirror/LAPORAN_data.csv
cat: /mnt/mirror/LAPORAN_data.csv: No such file or directory
```

## Catatan

- Source directory: `/home/bagas/reports`
- Mount point: `/mnt/mirror`
- Jalankan `setup.sh` terlebih dahulu

---

# MirrorFS

Bagas is a final-year student currently working on his undergraduate thesis. Alongside his academic workload, he faces an additional problem that significantly disrupts his productivity: the laptop he uses is a shared family device, also used by his younger siblings.

One night, Bagas finished organizing dozens of his research report files. The following morning, he found that several files had been renamed. The incident was caused by his younger sibling, who is in elementary school and was in the process of learning how to use a computer.

As a student of Operating Systems, Bagas decided to address the problem technically. He designed **MirrorFS**, a FUSE filesystem that mirrors his reports folder with a uniform naming convention, while preventing any modification by any party.

Create a FUSE program named `mirrorfs.c` that is mounted at `/mnt/mirror` and reads from the source directory `/home/bagas/reports`. The filesystem must implement several specific behaviors.

> **Note:** The initial setup has been provided in `setup.sh`. Run it once before starting the task.

### A. Mirroring Source Directory _(20 Points)_

The FUSE filesystem must directly display the contents of `/home/bagas/reports`. When a user runs `ls /mnt/mirror`, it must show the same files as those in the source directory.

```bash
$ ls /home/bagas/reports
laporan_jan.txt  laporan_feb.txt  laporan_mar.txt  data.csv
```

### B. Read-Only Filesystem _(20 Points)_

The entire filesystem must be **read-only**. No user is permitted to create, modify, or delete any file inside `/mnt/mirror`. All write operations must fail and return an `EROFS` error.

```bash
$ touch /mnt/mirror/file_baru.txt
touch: cannot touch '/mnt/mirror/file_baru.txt': Read-only file system

$ echo "test" > /mnt/mirror/laporan_jan.txt
bash: /mnt/mirror/laporan_jan.txt: Read-only file system
```

### C. File Name Prefix _(25 Points)_

All files displayed inside `/mnt/mirror` must have the prefix `LAPORAN_` to enforce a uniform naming convention. However, when a file is read, its content must still refer to the original file in the source directory.

```bash
$ ls /mnt/mirror
LAPORAN_laporan_jan.txt  LAPORAN_laporan_feb.txt  LAPORAN_data.csv

$ cat /mnt/mirror/LAPORAN_laporan_jan.txt
Laporan Bulan Januari     ← content from the original laporan_jan.txt
```

### D. Extension Filter _(35 Points)_

MirrorFS must only display files with the `.txt` extension. Files with any other extension must neither appear in the directory listing nor be accessible through the mount point.

```bash
$ ls /home/bagas/reports
laporan_jan.txt  laporan_feb.txt  data.csv  summary.pdf

$ ls /mnt/mirror
LAPORAN_laporan_jan.txt  LAPORAN_laporan_feb.txt
# data.csv and summary.pdf do not appear
```

## Complete Scenario Example

```bash
$ ls /home/bagas/reports
laporan_jan.txt  laporan_feb.txt  laporan_mar.txt  data.csv  summary.pdf

$ ls /mnt/mirror
LAPORAN_laporan_jan.txt  LAPORAN_laporan_feb.txt  LAPORAN_laporan_mar.txt

$ cat /mnt/mirror/LAPORAN_laporan_jan.txt
Laporan Bulan Januari

$ touch /mnt/mirror/test.txt
touch: cannot touch '/mnt/mirror/test.txt': Read-only file system

$ cat /mnt/mirror/LAPORAN_data.csv
cat: /mnt/mirror/LAPORAN_data.csv: No such file or directory
```

## Notes
- Source directory: `/home/bagas/reports`
- Mount point: `/mnt/mirror`
- Run `setup.sh` before starting
