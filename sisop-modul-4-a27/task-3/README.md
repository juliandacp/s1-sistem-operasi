# SumbulOS — The Piping Gauntlet

Lilhab baru saja menyelesaikan seminggu yang melelahkan mempelajari filesystem ketika Trabowo, rivalnya yang abadi, menempelkan secarik kertas di mejanya.

> **"Kamu mengaku penggemar OS? Buktikan. Buat aku sebuah shell — bare metal, dari nol — yang bisa menjalankan `echo`, `grep`, dan `wc` dengan piping yang benar. Kamu punya waktu sampai deadline."**

Lilhab menatap kertas itu. *Bare metal. Tanpa libc. Tanpa OS di bawahnya. Hanya interrupt BIOS dan memori mentah.*
Ia membuka laptopnya, membuat folder bernama `SumbulOS`, dan mulai bekerja.

Di tengah pengerjaan, kodenya menolak untuk dikompilasi. Dalam momen kelemahan, ia membuka chatbot AI dan mengetik: *"Bisakah kamu membuatkan aku kernel bare-metal dengan piping?"*

Responnya adalah: **"lol no."**

Dengan rendah hati, Lilhab menutup tab tersebut. Ia menemukan [modul](https://github.com/arsitektur-jaringan-komputer/Modul-Sisop/tree/master/Modul4), mengambil napas dalam, dan memutuskan untuk meminta bantuan dengan cara yang benar — dari *kamu*.

**Bantu Lilhab menyelesaikan kernelnya sebelum deadline!**

---

### Struktur Direktori

```
task/
├── bin/                  ← output kompilasi (dibuat oleh make prepare)
├── include/
│   ├── kernel.h
│   ├── std_lib.h
│   └── std_type.h
├── src/
│   ├── bootloader.asm    ← disediakan, jangan dimodifikasi
│   ├── kernel.asm        ← disediakan, jangan dimodifikasi
│   ├── kernel.c          ← KAMU LENGKAPI INI
│   └── std_lib.c         ← KAMU LENGKAPI INI
├── bochsrc.txt           ← disediakan, jangan dimodifikasi
└── makefile              ← KAMU LENGKAPI INI
```

---

### Latar Belakang

#### Cara Kerja `kernel.asm`

Dua fungsi telah diimplementasikan dalam assembly dan diekspor ke C:

```c
// Menulis satu byte ke alamat memori segment:offset
void putInMemory(int segment, int address, char character);

// Memicu interrupt BIOS dengan nilai register yang diberikan
int interrupt(int number, int AX, int BX, int CX, int DX);
```

Karena `AX` adalah register 16-bit yang terdiri dari `AH` (byte tinggi) dan `AL` (byte rendah), kamu membangunnya sebagai:
```c
AX = AH * 256 + AL   // atau secara ekuivalen: AX = (AH << 8) | AL
```

#### Pustaka Standar yang Tersedia (`std_lib.h` / `std_lib.c`)

```c
int div(int a, int b);
int mod(int a, int b);
void memcpy(byte* src, byte* dst, unsigned int size);
unsigned int strlen(char* str);
bool strcmp(char* str1, char* str2);   // mengembalikan true jika sama
void strcpy(char* src, char* dst);
void clear(byte* buf, unsigned int size);
```

#### Referensi Perintah

| Tool    | Penggunaan |
|---------|-----------|
| `dd`    | `dd if=<in> of=<out> bs=<block_size> count=<n> seek=<n> conv=<conv>` |
| `nasm`  | `nasm -f <format> <input> -o <output>` |
| `bcc`   | `bcc -ansi -c <input> -o <output>` |
| `ld86`  | `ld86 -o <output> -d <file1.o> [file2.o ...]` |
| `bochs` | `bochs -f <config> -q` |

---

### Tugas

#### A — Screen I/O (`src/kernel.c`) _(25 Points)_

Implementasikan tiga fungsi I/O dasar di `kernel.c`.

**`void printString(char* str)`**
- Cetak setiap karakter dari string yang diakhiri null ke layar.
- Gunakan `interrupt(0x10, AX, 0, 0, 0)` dengan `AH = 0x0E` (BIOS teletype output).

**`void readString(char* buf)`**
- Baca input keyboard karakter per karakter sampai Enter (`\r`) ditekan.
- Gunakan `interrupt(0x16, 0x0000, 0, 0, 0)` untuk membaca keystroke (`AH = 0x00`).
- Echo setiap karakter ke layar saat diketik.
- Tangani Backspace (`\b`): hapus karakter terakhir dari buffer dan layar.
  Urutan backspace-erase standar adalah: cetak `\b`, cetak `' '`, cetak `\b`.

**`void clearScreen()`**
- Scroll seluruh layar (bersihkan) menggunakan `interrupt(0x10, ...)` dengan `AH = 0x06`:
  - `AL = 0` (bersihkan semua baris), `BH = 0x07` (atribut white-on-black)
  - `CH = 0, CL = 0` (sudut kiri atas), `DH = 24, DL = 79` (sudut kanan bawah)
- Pindahkan kursor ke posisi (0, 0) menggunakan `AH = 0x02`.
- Isi semua 80×25 sel memori video di segmen `0xB800` dengan karakter spasi dan byte atribut `0x07` menggunakan `putInMemory()`.

---

#### B — Pustaka Standar (`src/std_lib.c`) _(20 Points)_

Implementasikan ketujuh fungsi utilitas yang dideklarasikan di `std_lib.h`.

- **`div(a, b)`** — Pembagian integer. **Jangan gunakan `/`**; gunakan hanya `+`, `-`, `*`, `<<`, `>>`. Tangani operand negatif dengan benar.
- **`mod(a, b)`** — Modulo integer. **Jangan gunakan `%`**. Kamu boleh menggunakan `div()`.
- **`memcpy(src, dst, size)`** — Salin `size` byte dari `src` ke `dst`.
- **`strlen(str)`** — Kembalikan jumlah karakter sebelum null terminator.
- **`strcmp(str1, str2)`** — Kembalikan `true` jika kedua string identik, `false` jika tidak.
- **`strcpy(src, dst)`** — Salin `src` ke `dst`, termasuk null terminator.
- **`clear(buf, size)`** — Zeroing `size` byte dari `buf`.

---

#### C — Perintah `echo` dan `grep` (`src/kernel.c`) _(25 Points)_

Shell menerima perintah dari user melalui `readString`. Dispatcher pipe (`handleCommand`) sudah disediakan — kamu hanya perlu mengimplementasikan handler-nya.

**`void handleEcho(char* args, char* output)`**
- Salin `args` langsung ke `output`.
- Contoh: `echo hello world` → buffer output berisi `hello world`.

**`void handleGrep(char* pattern, char* input, char* output)`**
- Cari `pattern` sebagai **substring** di manapun di dalam `input`.
- Jika ditemukan: salin `input` ke `output`.
- Jika tidak ditemukan: set `output` ke kosong (`output[0] = '\0'`), yang akan tercetak sebagai `NULL`.

Perilaku shell yang diharapkan:
```
$> echo hello world
hello world

$> echo hello world | grep hello
hello world

$> echo hello world | grep bye
NULL
```

---

#### D — Perintah `wc` (`src/kernel.c`) _(20 Points)_

**`void handleWc(char* input, char* output)`**
- Jika `input` kosong, output `"0 0 0"`.
- Jika tidak, hitung:
  - **Baris** — jumlah karakter `'\n'` + 1 (string non-kosong memiliki setidaknya 1 baris).
  - **Kata** — jumlah token yang dipisahkan spasi.
  - **Karakter** — total jumlah karakter (tidak termasuk null terminator).
- Tulis hasil ke `output` dalam format `"lines words chars"`.
- Gunakan helper `intToStr()` yang disediakan untuk mengonversi integer ke string.

Perilaku shell yang diharapkan:
```
$> echo hello world | wc
1 2 11

$> echo hello world | grep hello | wc
1 2 11

$> echo hello world | grep bye | wc
NULL
```

> **Catatan:** `wc` setelah `grep` yang gagal menghasilkan `NULL` karena inputnya kosong.

---

#### E — Makefile (`makefile`) _(10 Points)_

Lengkapi setiap target di `makefile` sehingga `make build` mengompilasi seluruh OS ke `bin/floppy.img` dan `make run` menjalankannya di Bochs.

| Target       | Yang harus dilakukan |
|--------------|----------------------|
| `prepare`    | Buat image floppy 1.44 MB kosong (`bin/floppy.img`) yang diisi nol. Gunakan blok 512 byte, total 2880 blok. |
| `bootloader` | Assemble `src/bootloader.asm` → `bin/bootloader.bin` (flat binary). Tulis ke sektor 0 dari `bin/floppy.img` (1 blok, `conv=notrunc`). |
| `stdlib`     | Kompilasi `src/std_lib.c` → `bin/std_lib.o`. |
| `kernel`     | Kompilasi `src/kernel.c` → `bin/kernel.o`. Assemble `src/kernel.asm` → `bin/kernel_asm.o` (format as86). |
| `link`       | Link `bin/kernel.o`, `bin/kernel_asm.o`, `bin/std_lib.o` → `bin/kernel.bin`. Tulis `bin/kernel.bin` ke `bin/floppy.img` mulai dari sektor 1 (`seek=1`, `conv=notrunc`). |
| `build`      | Jalankan `prepare`, `bootloader`, `stdlib`, `kernel`, `link` secara berurutan. **(sudah disediakan)** |
| `run`        | Jalankan Bochs dengan `bochsrc.txt` dalam mode senyap. |

> **Tips:** Direktori `bin/` harus ada sebelum `dd` dan `bcc` menulis ke dalamnya — target `prepare` adalah tempat yang tepat untuk membuatnya.

---

### Output Akhir yang Diharapkan

Ketika kamu menjalankan `make build && make run`, Bochs seharusnya boot dan menampilkan:

```
SumbulOS - [KODE TIM KAMU]
$> _
```

Shell harus menangani dengan benar semua kombinasi `echo`, `grep`, dan `wc` dengan hingga dua pipe `|`.

---

# SumbulOS — The Piping Gauntlet

Lilhab had just finished a grueling week of studying filesystems when Trabowo, his perpetual rival,
slammed a sticky note on his desk.

> **"You call yourself an OS enthusiast? Prove it. Build me a shell — bare metal, from scratch —
> that can run `echo`, `grep`, and `wc` with proper piping. You have until the deadline."**

Lilhab stared at the note. *Bare metal. No libc. No OS beneath him. Just BIOS interrupts and raw memory.*
He opened his laptop, created a folder called `SumbulOS`, and got to work.

Halfway through, his code refused to compile. In a moment of weakness, he opened an AI chatbot
and typed: *"Can you build a bare-metal kernel shell with piping for me?"*

The response was: **"lol no."**

Humbled, Lilhab closed the tab. He found the [module](https://github.com/arsitektur-jaringan-komputer/Modul-Sisop/tree/master/Modul4),
took a deep breath, and decided to ask for help the right way — from *you*.

**Help Lilhab finish his kernel before the deadline!**

---

### Directory Structure

```
task/
├── bin/                  ← compiled output (created by make prepare)
├── include/
│   ├── kernel.h
│   ├── std_lib.h
│   └── std_type.h
├── src/
│   ├── bootloader.asm    ← provided, do not modify
│   ├── kernel.asm        ← provided, do not modify
│   ├── kernel.c          ← YOU COMPLETE THIS
│   └── std_lib.c         ← YOU COMPLETE THIS
├── bochsrc.txt           ← provided, do not modify
└── makefile              ← YOU COMPLETE THIS
```

---

### Background

#### How `kernel.asm` Works

Two functions are pre-implemented in assembly and exported to C:

```c
// Write a single byte to a memory segment:offset address
void putInMemory(int segment, int address, char character);

// Trigger a BIOS interrupt with the given register values
int interrupt(int number, int AX, int BX, int CX, int DX);
```

Because `AX` is a 16-bit register composed of `AH` (high byte) and `AL` (low byte), you construct it as:
```c
AX = AH * 256 + AL   // or equivalently: AX = (AH << 8) | AL
```

#### Available Standard Library (`std_lib.h` / `std_lib.c`)

```c
int div(int a, int b);
int mod(int a, int b);
void memcpy(byte* src, byte* dst, unsigned int size);
unsigned int strlen(char* str);
bool strcmp(char* str1, char* str2);   // returns true if equal
void strcpy(char* src, char* dst);
void clear(byte* buf, unsigned int size);
```

#### Command Reference

| Tool    | Usage |
|---------|-------|
| `dd`    | `dd if=<in> of=<out> bs=<block_size> count=<n> seek=<n> conv=<conv>` |
| `nasm`  | `nasm -f <format> <input> -o <output>` |
| `bcc`   | `bcc -ansi -c <input> -o <output>` |
| `ld86`  | `ld86 -o <output> -d <file1.o> [file2.o ...]` |
| `bochs` | `bochs -f <config> -q` |

---

### Tasks

#### A — Screen I/O (`src/kernel.c`) _(25 Points)_

Implement the three fundamental I/O functions in `kernel.c`.

**`void printString(char* str)`**
- Print each character of the null-terminated string to the screen.
- Use `interrupt(0x10, AX, 0, 0, 0)` with `AH = 0x0E` (BIOS teletype output).

**`void readString(char* buf)`**
- Read keyboard input character by character until Enter (`\r`) is pressed.
- Use `interrupt(0x16, 0x0000, 0, 0, 0)` to read a keystroke (`AH = 0x00`).
- Echo each character to the screen as it is typed.
- Handle Backspace (`\b`): erase the last character from both the buffer and the screen.
  The standard backspace-erase sequence is: print `\b`, print `' '`, print `\b`.

**`void clearScreen()`**
- Scroll the entire screen (clear it) using `interrupt(0x10, ...)` with `AH = 0x06`:
  - `AL = 0` (clear all lines), `BH = 0x07` (white-on-black attribute)
  - `CH = 0, CL = 0` (top-left corner), `DH = 24, DL = 79` (bottom-right corner)
- Move the cursor to position (0, 0) using `AH = 0x02`.
- Fill all 80×25 cells of video memory at segment `0xB800` with a space character
  and attribute byte `0x07` using `putInMemory()`.

---

#### B — Standard Library (`src/std_lib.c`) _(20 Points)_

Implement all seven utility functions declared in `std_lib.h`.

- **`div(a, b)`** — Integer division. **Do not use `/`**; use only `+`, `-`, `*`, `<<`, `>>`.
  Handle negative operands correctly.
- **`mod(a, b)`** — Integer modulo. **Do not use `%`**. You may use your `div()`.
- **`memcpy(src, dst, size)`** — Copy `size` bytes from `src` to `dst`.
- **`strlen(str)`** — Return the number of characters before the null terminator.
- **`strcmp(str1, str2)`** — Return `true` if both strings are identical, `false` otherwise.
- **`strcpy(src, dst)`** — Copy `src` into `dst`, including the null terminator.
- **`clear(buf, size)`** — Zero out `size` bytes of `buf`.

---

#### C — `echo` and `grep` Commands (`src/kernel.c`) _(25 Points)_

The shell accepts commands from the user via `readString`. A pipe dispatcher
(`handleCommand`) is already provided — you only need to implement the handlers.

**`void handleEcho(char* args, char* output)`**
- Copy `args` directly into `output`.
- Example: `echo hello world` → output buffer contains `hello world`.

**`void handleGrep(char* pattern, char* input, char* output)`**
- Search for `pattern` as a **substring** anywhere inside `input`.
- If found: copy `input` into `output`.
- If not found: set `output` to empty (`output[0] = '\0'`), which will print as `NULL`.

Expected shell behaviour:
```
$> echo hello world
hello world

$> echo hello world | grep hello
hello world

$> echo hello world | grep bye
NULL
```

---

#### D — `wc` Command (`src/kernel.c`) _(20 Points)_

**`void handleWc(char* input, char* output)`**
- If `input` is empty, output `"0 0 0"`.
- Otherwise count:
  - **Lines** — number of `'\n'` characters + 1 (a non-empty string has at least 1 line).
  - **Words** — number of whitespace-delimited tokens.
  - **Characters** — total number of characters (excluding the null terminator).
- Write the result to `output` in the format `"lines words chars"`.
- Use the provided `intToStr()` helper to convert integers to strings.

Expected shell behaviour:
```
$> echo hello world | wc
1 2 11

$> echo hello world | grep hello | wc
1 2 11

$> echo hello world | grep bye | wc
NULL
```

> **Note:** `wc` after a failed `grep` outputs `NULL` because its input is empty.

---

#### E — Makefile (`makefile`) _(10 Points)_

Complete every target in the `makefile` so that `make build` compiles the entire
OS into `bin/floppy.img` and `make run` launches it in Bochs.

| Target       | What it must do |
|--------------|-----------------|
| `prepare`    | Create a blank 1.44 MB floppy image (`bin/floppy.img`) filled with zeros. Use 512-byte blocks, 2880 blocks total. |
| `bootloader` | Assemble `src/bootloader.asm` → `bin/bootloader.bin` (flat binary). Write it to sector 0 of `bin/floppy.img` (1 block, `conv=notrunc`). |
| `stdlib`     | Compile `src/std_lib.c` → `bin/std_lib.o`. |
| `kernel`     | Compile `src/kernel.c` → `bin/kernel.o`. Assemble `src/kernel.asm` → `bin/kernel_asm.o` (as86 format). |
| `link`       | Link `bin/kernel.o`, `bin/kernel_asm.o`, `bin/std_lib.o` → `bin/kernel.bin`. Write `bin/kernel.bin` to `bin/floppy.img` starting at sector 1 (`seek=1`, `conv=notrunc`). |
| `build`      | Run `prepare`, `bootloader`, `stdlib`, `kernel`, `link` in order. **(already provided)** |
| `run`        | Launch Bochs with `bochsrc.txt` in quiet mode. |

> **Tip:** The `bin/` directory must exist before `dd` and `bcc` write into it — the `prepare` target is the right place to create it.

---

### Expected Final Output

When you run `make build && make run`, Bochs should boot and display:

```
SumbulOS - [YOUR TEAM CODE]
$> _
```

The shell should correctly handle all combinations of `echo`, `grep`, and `wc`
with up to two `|` pipes.
