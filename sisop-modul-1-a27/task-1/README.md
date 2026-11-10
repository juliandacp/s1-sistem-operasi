# GAME ASSET MANAGER

Studio game **Gamelove** sedang mengembangkan beberapa game baru. Seiring berkembangnya proyek, jumlah aset game seperti gambar, suara, dan model 3D di folder `assets/` meningkat hingga ratusan file.

Masalah mulai muncul.

Beberapa file ada di folder tetapi tidak tercatat di database, sementara sebagian metadata masih tersimpan meskipun file fisiknya sudah hilang. Akibatnya tim developer kesulitan melacak aset yang sebenarnya digunakan dalam proyek.

Untuk mengatasi masalah ini, kamu ditunjuk sebagai Tools Engineer untuk membuat sebuah Command Line Interface (CLI) bernama:

```
script.sh
```

Tool ini akan berfungsi sebagai Game Asset Manager, yang menjaga sinkronisasi antara folder aset fisik dan database metadata `metadata.csv`.

## A. Sinkronisasi (SYNC) _(30 Points)_

Perintah `sync` bertugas memastikan folder aset dan database metadata selalu konsisten.

### 1. Detect New Files

Jika terdapat file baru di folder `assets/` yang belum tercatat di `metadata.csv`, maka sistem harus otomatis menambahkan metadata baru dengan format:

```
filename,size,extension,created_at
```

contoh:

```
vector.svg,482,svg,2026-03-01_04:28:40
```

### 2. Clean Up

Jika terdapat entri pada `metadata.csv` tetapi file fisiknya sudah tidak ada di folder `assets/`, maka entri tersebut harus dihapus dari database.

### 3. Automation

Jalankan perintah `sync` setiap 5 menit menggunakan `crontab`. Tuliskan konfigurasi cronjob kalian pada file `crontabs`.

## B. List _(20 Points)_

Perintah `list` menampilkan isi database dalam bentuk tabel.

Fitur tambahan:

- `--sort=name` → mengurutkan berdasarkan nama file
- `--sort=size` → mengurutkan berdasarkan ukuran file
- `--ext=<extension>` → menampilkan hanya file dengan ekstensi tertentu

Contoh:

```
./script.sh list --sort=size
./script.sh list --ext=png
./script.sh list --ext=png --sort=size
```

## C. Statistics _(20 Points)_

Perintah `stats` menampilkan ringkasan statistik aset yang tersimpan di database, meliputi:

- Total jumlah aset
- Total ukuran seluruh aset (bytes)
- Rata-rata ukuran aset
- Aset terbesar
- Aset terkecil
- Jumlah file berdasarkan ekstensi

## D. Manajemen Aset (CREATE & DELETE) _(20 Points)_

Selain monitoring, tool ini juga memungkinkan pengelolaan aset secara langsung dari CLI.

### 1. Mock Asset Creation

Perintah:

```
./script.sh create <filename> <size>
```

Fungsi:

- Membuat file dummy pada folder `assets/`
- Ukuran file mengikuti parameter `<size>` dalam bytes
- Secara otomatis menambahkan metadata ke `metadata.csv`

### 2. Asset Removal

Perintah:

```
./script.sh delete <filename>
```

Fungsi:

- Menghapus file fisik dari folder `assets/`
- Menghapus metadata file tersebut dari database

## E. Sistem Logging (LOG) _(10 Points)_

Seluruh aktivitas sistem harus dicatat dalam file:

```
activity.log
```

### Format Log

```
[YYYY-MM-DD HH:MM:SS] [LEVEL] [COMMAND] message
```

#### 1. SYSTEM

Jika komponen penting tidak ditemukan:

```
assets/
metadata.csv
activity.log
```

Log yang dicatat:

```
[YYYY-MM-DD HH:MM:SS] [ERROR] [SYSTEM] Missing components (assets/, metadata.csv, or activity.log)
```

#### 2. SYNC

File baru ditemukan

```
[YYYY-MM-DD HH:MM:SS] [INFO] [SYNC] Added new file <filename>
```

File hilang dibersihkan dari database

```
[YYYY-MM-DD HH:MM:SS] [INFO] [SYNC] Removed missing file <filename> from database
```

Automation dijalankan

```
[YYYY-MM-DD HH:MM:SS] [INFO] [AUTOSYNC] Installed autosync every 5 minutes
```

Cronjob sudah ada

```
[YYYY-MM-DD HH:MM:SS] [INFO] [AUTOSYNC] Autosync already installed
```

#### 3. LIST

Berhasil menampilkan database

```
[YYYY-MM-DD HH:MM:SS] [INFO] [LIST] Displayed database content
```

Hasil filter kosong

```
[YYYY-MM-DD HH:MM:SS] [INFO] [LIST] Query returned empty result
```

#### 4. STATS

Statistik berhasil dibuat

```
[YYYY-MM-DD HH:MM:SS] [INFO] [STATS] Generated statistics
```

Database kosong

```
[YYYY-MM-DD HH:MM:SS] [INFO] [STATS] Accessed empty database
```

#### 5. CREATE

Argumen kurang

```
[YYYY-MM-DD HH:MM:SS] [ERROR] [CREATE] Missing arguments
```

Size tidak valid

```
[YYYY-MM-DD HH:MM:SS] [ERROR] [CREATE] Invalid size: <size>
```

File sudah ada

```
[YYYY-MM-DD HH:MM:SS] [ERROR] [CREATE] File <filename> already exists
```

File berhasil dibuat

```
[YYYY-MM-DD HH:MM:SS] [INFO] [CREATE] Created file <filename> (<size> bytes)
```

#### 6. DELETE

Tidak ada filename

```
[YYYY-MM-DD HH:MM:SS] [ERROR] [DELETE] No filename provided
```

File tidak ditemukan

```
[YYYY-MM-DD HH:MM:SS] [ERROR] [DELETE] <filename> not found
```

File berhasil dihapus

```
[YYYY-MM-DD HH:MM:SS] [INFO] [DELETE] Deleted <filename>
```

<br>
<br>
Good Luck All 😆

---
# GAME ASSET MANAGER

Game studio **Gamelove** is currently developing several new games. As the projects grow, the number of game assets such as images, sounds, and 3D models in the `assets/` folder has increased to hundreds of files.

Problems have started to appear.

Some files exist in the folder but are not recorded in the database, while some metadata entries still remain even though the physical files have already been deleted. As a result, the development team is having difficulty tracking which assets are actually used in the project.

To solve this problem, you are assigned as a **Tools Engineer** to create a Command Line Interface (CLI) tool named:

```
script.sh
```

This tool will function as a **Game Asset Manager**, responsible for maintaining synchronization between the physical asset folder and the metadata database `metadata.csv`.

## A. Synchronization (SYNC) *(30 Points)*

The `sync` command ensures that the asset folder and the metadata database remain consistent.

### 1. Detect New Files

If a new file exists in the `assets/` folder but is not recorded in `metadata.csv`, the system must automatically add a new metadata entry with the format:

```
filename,size,extension,created_at
```

example:

```
vector.svg,482,svg,2026-03-01_04:28:40
```

### 2. Clean Up

If an entry exists in `metadata.csv` but the physical file no longer exists in the `assets/` folder, the entry must be removed from the database.

### 3. Automation

Run the `sync` command every 5 minutes using `crontab`. Write your cronjob configuration in the `crontabs` file.

## B. List *(20 Points)*

The `list` command displays the contents of the database in a table format.

Additional features:

* `--sort=name` → sort by filename
* `--sort=size` → sort by file size
* `--ext=<extension>` → display only files with a specific extension

Example:

```
./script.sh list --sort=size
./script.sh list --ext=png
./script.sh list --ext=png --sort=size
```

## C. Statistics *(20 Points)*

The `stats` command displays summary statistics of assets stored in the database, including:

* Total number of assets
* Total size of all assets (bytes)
* Average asset size
* Largest asset
* Smallest asset
* Number of files based on extension

## D. Asset Management (CREATE & DELETE) *(20 Points)*

In addition to monitoring, this tool also allows asset management directly from the CLI.

### 1. Mock Asset Creation

Command:

```
./script.sh create <filename> <size>
```

Function:

* Creates a dummy file in the `assets/` folder
* The file size follows the `<size>` parameter in bytes
* Automatically adds metadata to `metadata.csv`

### 2. Asset Removal

Command:

```
./script.sh delete <filename>
```

Function:

* Deletes the physical file from the `assets/` folder
* Removes the file metadata from the database

## E. Logging System (LOG) *(10 Points)*

All system activities must be recorded in the file:

```
activity.log
```

### Log Format

```
[YYYY-MM-DD HH:MM:SS] [LEVEL] [COMMAND] message
```

#### 1. SYSTEM

If important components are not found:

```
assets/
metadata.csv
activity.log
```

The log that must be recorded:

```
[YYYY-MM-DD HH:MM:SS] [ERROR] [SYSTEM] Missing components (assets/, metadata.csv, or activity.log)
```

#### 2. SYNC

New file detected

```
[YYYY-MM-DD HH:MM:SS] [INFO] [SYNC] Added new file <filename>
```

Missing file cleaned from the database

```
[YYYY-MM-DD HH:MM:SS] [INFO] [SYNC] Removed missing file <filename> from database
```

Automation executed

```
[YYYY-MM-DD HH:MM:SS] [INFO] [AUTOSYNC] Installed autosync every 5 minutes
```

Cronjob already exists

```
[YYYY-MM-DD HH:MM:SS] [INFO] [AUTOSYNC] Autosync already installed
```

#### 3. LIST

Database successfully displayed

```
[YYYY-MM-DD HH:MM:SS] [INFO] [LIST] Displayed database content
```

Empty filter result

```
[YYYY-MM-DD HH:MM:SS] [INFO] [LIST] Query returned empty result
```

#### 4. STATS

Statistics successfully generated

```
[YYYY-MM-DD HH:MM:SS] [INFO] [STATS] Generated statistics
```

Database is empty

```
[YYYY-MM-DD HH:MM:SS] [INFO] [STATS] Accessed empty database
```

#### 5. CREATE

Missing arguments

```
[YYYY-MM-DD HH:MM:SS] [ERROR] [CREATE] Missing arguments
```

Invalid size

```
[YYYY-MM-DD HH:MM:SS] [ERROR] [CREATE] Invalid size: <size>
```

File already exists

```
[YYYY-MM-DD HH:MM:SS] [ERROR] [CREATE] File <filename> already exists
```

File successfully created

```
[YYYY-MM-DD HH:MM:SS] [INFO] [CREATE] Created file <filename> (<size> bytes)
```

#### 6. DELETE

No filename provided

```
[YYYY-MM-DD HH:MM:SS] [ERROR] [DELETE] No filename provided
```

File not found

```
[YYYY-MM-DD HH:MM:SS] [ERROR] [DELETE] <filename> not found
```

File successfully deleted

```
[YYYY-MM-DD HH:MM:SS] [INFO] [DELETE] Deleted <filename>
```

<br>
<br>

Good Luck All 😆