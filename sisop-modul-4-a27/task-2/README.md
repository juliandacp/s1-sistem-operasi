# The Plagiarist's Trap

Alam dan Yumna sudah jadi teman kelas sejak semester pertama. Setiap praktikum mereka duduk berdampingan, laptop saling menghadap, matcha di tengah meja, debat makanan enak di sekitar Surabaya. Yumna lebih jago algoritma, Alam lebih ngerti sistem. Mereka saling melengkapi tanpa pernah bicara terlalu banyak tentangnya.

Tapi belakangan ada yang berubah. Alam mulai sadar dia selalu menghitung hari sampai praktikum berikutnya tiba. Yumna mulai sadar dia tidak pernah benar-benar fokus pada soal. Dia lebih sering memperhatikan cara Alam menggaruk kepala saat bingung, atau senyum kecilnya saat kode akhirnya jalan setelah dua jam debugging.

Modul minggu ini tentang FUSE. Soalnya, entah kebetulan atau tidak, bercerita tentang tiga karakter fiktif bernama **CodeCopier**, **AlgoAnna**, dan **BinaryBen** yang merancang sebuah filesystem jebakan. "Drama banget soalnya," bisik Yumna sambil tertawa kecil. "Kerjain bareng?" tanya Alam, mencoba terdengar santai. Yumna mengangguk terlalu cepat untuk bisa menyembunyikan senyumnya. Tetapi Alam ingin kamu, si sepuh sisop untuk mengerjakan soal nya terlebih dahulu biar dia tidak malu di depan Yumna.

Sebagai teman yang baik dan _problem solver_ andal, kamu diminta untuk **melengkapi dan memperbaiki** kode FUSE filesystem yang telah dirancang oleh AlgoAnna dan BinaryBen. Sayangnya, kode tersebut masih berantakan — ada bagian yang hilang, ada juga _bug_ yang harus kamu temukan. File yang harus kamu kerjakan adalah `troll_skeleton.c`. Mungkin malam ini, bukan cuma jebakan filesystem yang akan tertangkap, tetapi Alam juga bakal confess ke Yumna. *wink wink*

---

### A. Pembuatan User _(10 Points)_ --> done

Buatlah 3 user di sistem yang merepresentasikan aktor-aktor yang terlibat:

- **CodeCopier** — sang plagiator (target jebakan)
- **AlgoAnna** — otak di balik rencana
- **BinaryBen** — kaki tangan AlgoAnna

Gunakan `useradd` dan `passwd` untuk membuat akun lokal. Ketiga user inilah yang akan digunakan untuk menguji filesystem FUSE-mu.

---

### B. Pemasangan FUSE _(25 Points)_  --> done

Lengkapi kode di `troll_skeleton.c` agar filesystem FUSE dapat di-_mount_ di `/mnt/honeypot`. Di dalamnya **harus terlihat dua file** ketika diakses dengan `ls`:

- `secret_solution.txt` — umpan utama
- `submit.txt` — tempat CodeCopier akan memicu jebakannya sendiri

> **Catatan:** Pada `troll_skeleton.c`, fungsi `troll_readdir` masih bermasalah. Hanya satu file yang terlihat ketika user melakukan `ls`. Temukan dan perbaiki _bug_ tersebut agar kedua file muncul.

---

### C. Isi File yang Mengecoh _(25 Points)_

Setelah kedua file terlihat di filesystem, kamu harus melengkapi logika di fungsi `troll_read`. Apabila file `secret_solution.txt` dibaca menggunakan `cat`:

- Oleh user **selain `CodeCopier`** (misalnya `AlgoAnna` atau `BinaryBen`), maka isinya akan tampak seperti:

  ```
  CodeCopier's plagiarized assignments archive!!.txt --> done
  ```

- Oleh user **`CodeCopier`** sendiri, maka isinya akan tampak menggiurkan seperti:

  ```
  Algorithm walkthrough: Use dynamic programming with memoization on overlapping subproblems  --> done
  ```

Gunakan `fuse_get_context()` dan `getpwuid()` untuk mengetahui siapa user yang sedang mengakses file.

---

### D. Trap _(25 Points)_ --> done

Suatu hari, CodeCopier menemukan file `secret_solution.txt` di PC-nya. Dia kira ini adalah bocoran kunci jawaban tugas akhir, dan dia berniat mengirimnya ke grup belajarnya melalui sebuah "script upload" lama bernama `submit.txt` yang sudah biasa dia pakai dengan cara `echo "submit" > submit.txt`.

Lengkapi `troll_write` dan `troll_read` sehingga:

1. Ketika ada user yang menulis kata `submit` ke `submit.txt` (misalnya lewat `echo "submit" > /mnt/honeypot/submit.txt`), maka jebakan akan **aktif**.
2. Setelah jebakan aktif, **semua file** ber-ekstensi `.txt` di dalam `/mnt/honeypot` (baik `secret_solution.txt` maupun `submit.txt`) akan menampilkan ASCII Art (sudah disediakan di kode) yang bertuliskan:

   ```
   GOTCHA!! Caught red-handed!
   ```

3. **Status jebakan harus persist** — tetap aktif walaupun FUSE di-_unmount_, mesin di-_restart_, atau program dimatikan dan dijalankan ulang. _Hint:_ gunakan file penanda (_flag file_) di lokasi di luar FUSE filesystem, misalnya di `/var/tmp/`.

---

### E. Audit Log _(15 Points)_ --> done

AlgoAnna dan BinaryBen ingin bukti tertulis untuk ditunjukkan kepada CodeCopier nantinya. Mereka memintamu untuk **menambahkan mekanisme logging** ke filesystem FUSE-mu.

Implementasikan fungsi `log_operation` yang masih kosong di `troll_skeleton.c`, lalu **panggil fungsi tersebut** di dalam `troll_read` dan `troll_write`. Setiap operasi `read` dan `write` pada `secret_solution.txt` maupun `submit.txt` harus dicatat ke dalam file log di `/var/log/honeypot.log` dengan format:

```
[YYYY-MM-DD HH:MM:SS] <username> <OPERATION> <filename>
```

Contoh entry log yang valid:

```
[2026-05-19 14:32:01] CodeCopier READ secret_solution.txt
[2026-05-19 14:32:18] CodeCopier WRITE submit.txt
[2026-05-19 14:33:05] AlgoAnna READ secret_solution.txt
```

Ketentuan:

- `OPERATION` hanya berupa string `READ` atau `WRITE` (huruf kapital).
- Format waktu mengikuti `strftime` dengan pola `%Y-%m-%d %H:%M:%S`.
- File log harus **persist** (di luar mount point FUSE), sama seperti _flag file_ jebakan.
- Setiap entry adalah satu baris (`\n` di akhir).

---

## Catatan

- `troll_skeleton.c` adalah _starter code_. Bagian yang harus kamu kerjakan ditandai dengan komentar `// TODO: PART <X>` dan/atau `// BUG`.
- `submit.txt` hanya berupa file kosong yang berfungsi sebagai _trigger_ — narasi di soal hanyalah _storytelling_.
- Trigger jebakan **dan** isi log harus persist walaupun FUSE di-_unmount_ atau mesin di-_restart_.
- Pastikan FUSE di-_mount_ dengan opsi `allow_other` supaya user lain (selain yang menjalankan binary) bisa mengakses filesystem.
- Untuk reset jebakan saat testing: hapus file `/var/tmp/honeypot_trigger.flag`.

---

# The Plagiarist's Trap

Alam and Yumna have been classmates since their first semester. Every practicum they sit side by side - laptops facing each other, matcha in the middle of the table, debating where to find the best food around Surabaya. Yumna is sharper at algorithms, Alam knows systems better. They complement each other without ever really talking about it.

But lately something has shifted. Alam has started to realize he's been counting down the days until the next practicum. Yumna has started to realize she can never quite focus on the problems anymore. She catches herself watching the way Alam scratches his head when he's stuck, or that small smile he gives when his code finally works after two hours of debugging.

This week's module is on FUSE. The problem, coincidentally or not, happens to be about three fictional characters named **CodeCopier**, **AlgoAnna**, and **BinaryBen** who design a booby-trapped filesystem. "This is so dramatic and weird" Yumna whispers, laughing softly. "haha yeah, work on it together?" Alam asks, trying to sound casual. Yumna nods a little too quickly to hide her smile. But Alam wants you, the OS veteran, to solve the problem first so he doesn't embarrass himself in front of Yumna.

As a good friend and a reliable _problem solver_, you've been asked to **complete and fix** the FUSE filesystem code drafted by AlgoAnna and BinaryBen. Unfortunately, the code is still a mess. There are missing pieces, and there are _bugs_ you'll have to track down. The file you'll be working on is `troll_skeleton.c`. Maybe tonight, it won't just be the filesystem trap that gets caught — Alam might finally confess to Yumna too. *wink wink*

---

### A. User Creation _(10 Points)_

Create 3 users on the system representing the actors involved:

- **CodeCopier** — the plagiarist (trap target)
- **AlgoAnna** — the mastermind behind the plan
- **BinaryBen** — AlgoAnna's accomplice

Use `useradd` and `passwd` to create local accounts. These three users will be used to test your FUSE filesystem.

---

### B. FUSE Mounting _(25 Points)_

Complete the code in `troll_skeleton.c` so the FUSE filesystem can be mounted at `/mnt/honeypot`. Inside it, **two files must be visible** when accessed with `ls`:

- `secret_solution.txt` — the main bait
- `submit.txt` — where CodeCopier will trigger his own trap

> **Note:** In `troll_skeleton.c`, the `troll_readdir` function is broken. Only one file appears when the user runs `ls`. Find and fix the _bug_ so both files appear.

---

### C. Deceptive File Content _(25 Points)_

After both files are visible in the filesystem, you must complete the logic in the `troll_read` function. When `secret_solution.txt` is read using `cat`:

- By any user **other than `CodeCopier`** (e.g., `AlgoAnna` or `BinaryBen`), the content will appear as:

  ```
  CodeCopier's plagiarized assignments archive!!.txt
  ```

- By **`CodeCopier`** himself, the content will appear tempting:

  ```
  Algorithm walkthrough: Use dynamic programming with memoization on overlapping subproblems
  ```

Use `fuse_get_context()` and `getpwuid()` to identify who is accessing the file.

---

### D. Trap _(25 Points)_

One day, CodeCopier finds `secret_solution.txt` on his PC. He thinks this is a leaked answer key for the final assignment, and he intends to send it to his study group through an old "upload script" named `submit.txt` that he commonly uses with `echo "submit" > submit.txt`.

Complete `troll_write` and `troll_read` so that:

1. When any user writes the word `submit` to `submit.txt` (e.g., via `echo "submit" > /mnt/honeypot/submit.txt`), the trap **activates**.
2. After the trap activates, **all files** with the `.txt` extension inside `/mnt/honeypot` (both `secret_solution.txt` and `submit.txt`) will display ASCII Art (already provided in the code) that reads:

   ```
   GOTCHA!! Caught red-handed!
   ```

3. **Trap status must persist** — stays active even if FUSE is unmounted, the machine is restarted, or the program is killed and restarted. _Hint:_ use a flag file at a location outside the FUSE filesystem, e.g., in `/var/tmp/`.

---

### E. Audit Log _(15 Points)_

AlgoAnna and BinaryBen want written evidence to show CodeCopier later. They ask you to **add a logging mechanism** to your FUSE filesystem.

Implement the `log_operation` function (currently empty in `troll_skeleton.c`), then **call this function** inside `troll_read` and `troll_write`. Every `read` and `write` operation on `secret_solution.txt` and `submit.txt` must be recorded to a log file at `/var/log/honeypot.log` with the format:

```
[YYYY-MM-DD HH:MM:SS] <username> <OPERATION> <filename>
```

Example of valid log entries:

```
[2026-05-19 14:32:01] CodeCopier READ secret_solution.txt
[2026-05-19 14:32:18] CodeCopier WRITE submit.txt
[2026-05-19 14:33:05] AlgoAnna READ secret_solution.txt
```

Requirements:

- `OPERATION` is only the string `READ` or `WRITE` (uppercase).
- Time format follows `strftime` with the pattern `%Y-%m-%d %H:%M:%S`.
- The log file must **persist** (outside the FUSE mount point), just like the trap flag file.
- Each entry is one line (`\n` at the end).

---

## Notes

- `troll_skeleton.c` is _starter code_. Parts you need to complete are marked with comments `// TODO: PART <X>` and/or `// BUG`.
- `submit.txt` is just an empty file that serves as a _trigger_ — the story in the problem is just _storytelling_.
- Trap trigger **and** log content must persist even if FUSE is unmounted or the machine is restarted.
- Ensure FUSE is mounted with the `allow_other` option so other users (besides the one running the binary) can access the filesystem.
- To reset the trap during testing: delete the file `/var/tmp/honeypot_trigger.flag`.
