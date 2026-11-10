# Pertunjukan Drama Raja Setan Manchester
Anton merupakan seseorang yang bekerja di FIFA di divisi IT-Dev. Ia sangat suka untuk bermain sepakbola dan menonton pertandingan. Tapi karena bobot pekerjaanya yang sangat berat, Anton dan teman-temanya tidak dapat mengikuti berita sepakbola terbaru. Sebuah ide terbesit di pikiran Anton, ia ingin untuk membuat sebuah script yang dapat menampilkan hasil pertandingan bola yang sedang berlangsung, beserta riwayat pertandingan sebelumnya. Bantulah Anton untuk membuatkan sistem yang dapat membantunya untuk melihat statistik, hasil, dan riwayat pertandingan sepakbola favoritnya. 

## A. Login dan Register ***(30 Points)***
Untuk memastikan tidak sembarang orang bisa menggunakan aplikasi ini, Anton memutuskan untuk membuat sistem login. Buat sebuah script bernama: `register.sh`. Script ini digunakan untuk mendaftarkan user baru ke dalam sistem. dengan format sebagai berikut :
```sh
Nama lengkap 
Tanggal lahir
Role
```
Contoh Input : 
```
Nama: Anton Gonzales
Tanggal lahir (yyyy-mm-dd): 1969-06-09
Role: Analyst
```
Akan terdapat sebanyak 3 role dengan rincian sebagai berikut : 
```sh
1. Coach
2. Analyst
3. Player
```

### Pembuatan Username & Password
Sistem akan membuat username dan password secara otomatis dengan aturan username: `Kata pertama dari nama lengkap`. Contoh: `Anton Gonzales --> Username: Anton`. Password: `Username + tahun lahir + role`,  contoh: `Anton09061969Analyst`. Data kemudian disimpan ke dalam `/storage/users.txt` dengan format:
```
namaLengkap,tanggal lahir,role,username,password
```
Contoh:
```
Lionel Messi,2003-04-12,Player,Lionel,Lionel2003Player
```

Setelah membuat script `register.sh`, buatlah script `login.sh` agar user dapat melakukan login dan mengakses aplikasi. 
```sh
Username
Password
```
Sistem akan memeriksa apakah data tersebut cocok dengan data pada `users.txt`. Semua aktivitas login/register akan dicatat dalam file `/storage/log/log.txt` dengan format seperti berikut:
- Jika pengguna mencoba register dengan username yang sudah terdaftar :
```log
[ERROR] DD/MM/YY | hh:mm:ss REGISTER: User {USERNAME} sudah terdaftar!
```

- Jika register berhasil
```log
[INFO] DD/MM/YY | hh:mm:ss REGISTER: User {USERNAME} berhasil didaftarkan!
```

- Jika login gagal
```log
[ERROR] DD/MM/YY | hh:mm:ss LOGIN: Gagal melakukan login pada user {USERNAME}!
```

- Jika login berhasil
```log
[INFO] DD/MM/YY | hh:mm:ss LOGIN: User {USERNAME} berhasil login!
```

- Jika logout berhasil
```log
[INFO] DD/MM/YY | hh:mm:ss LOGIN: User {USERNAME} berhasil logout!
```

*Catatan*: Sistem hanya mengizinkan satu pengguna login pada satu waktu. Jika sudah ada pengguna aktif berdasarkan log, login dari pengguna lain tidak diproses sampai sesi sebelumnya berakhir (User yang sedang login melakukan logout).

Setelah user berhasil melakukan login, script `login.sh` akan menampilkan 4 opsi seperti berikut:
``` sh
===== Pertunjukan Drama Raja Setan Manchester =====
1. Lihat statistik pertandingan
2. Hapus user
3. Ubah waktu pengarsipan
4. Keluar
```

## B. Penampil Statistik Pertandingan ***(30 Points)***
Setelah membantu Anton untuk membuat sistem login dan register, Anton kemudian meminta mu untuk membuatkan `script.sh` yang akan berisi template laporan seperti berikut:
```sh
====================================================
           LAPORAN STATISTIK PERTANDINGAN
====================================================

Tanggal Laporan : [YYYY MM DD]
Waktu Generate  : [ss:mm:hh]

----------------------------------------------------
STATISTIK UMUM
----------------------------------------------------

Total Pertandingan        : [TOTAL_MATCH]
Total Gol                 : [TOTAL_GOALS]
Rata-rata Shots           : [AVG_SHOTS]

----------------------------------------------------
PERTANDINGAN TERBARU {team1} VS {team2}
----------------------------------------------------

PEMENANG                 : [WINNER]
GOL {team1}              : [GOALWINNINGTEAM]
GOL {team2}              : [GOALLOSINGTEAM]
AVERAGE SHOTS {team1}    : [AVERAGESHOTTEAM1]
AVERAGE SHOTS {team2}    : [AVERAGESHOTTEAM1]

----------------------------------------------------
RIWAYAT PERTANDINGAN 
----------------------------------------------------
No | Home Team | Away Team | Score | Shots | Winner
----------------------------------------------------
1  | Barca     | Madrid    | 2-1   | 12-9  | Barca
2  | City      | Arsenal   | 1-1   | 10-10 | Draw
```
Riwayat pertandingan hanya akan menampilkan maksimal hasil dari 5 pertandingan sebelumnya. 

**Untuk menjaga keamanan aplikasinya, Anton menginginkan agar `script.sh` tidak dapat dibuka sembarangan dan hanya dapat dijalankan jika terpanggil pada `login.sh`.**

Setiap `script.sh` yang dipanggil untuk menampilkan statistik pertandingan akan dicatat juga pada `/storage/log/log.txt` dengan format sebagai berikut:
- Jika perbedaan skor lebih dari 2:
``` log
[INFO] DD/MM/YY | hh:mm:ss MATCH: HAHAHA, {timKalah} menjadi penghibur yang sangat lucu!
```

- Jika perbedaan skor kurang dari sama dengan 2:
```log
[INFO] DD/MM/YY | hh:mm:ss MATCH: GACOR, {timMenang} menang bolo!
```

- Jika pertandingan Draw
```log
[INFO] DD/MM/YY | hh:mm:ss MATCH: Kok malah DRAW, ga seru ah!
```

Selain mencatat log, hasil pertandingan yang di generate oleh `script.sh` juga akan disimpan pada file tersendiri di directory `storage/matchStatistic` dengan format nama file:
```
MATCH_YY-MM-DD_MM:HH.txt
```

*Catatan*: Silahkan gunakan crontab untuk menjalankan `generate.sh` setiap 2 menit . Data yang dihasilkan oleh `generate.sh` yang nantinya akan digunakan oleh `script.sh` untuk menampilkan statistik pertandingan.

## C. Penghapusan User ***(20 Points)***
Jika user memilih `2` maka akan menampilkan daftar akun yang ingin dan dapat dihapus. Syarat untuk menghapus akun adalah `loggedUserLevel > targetUserLevel`, dengan level user sebagai berikut:
```
Coach   = 3
Analyst = 2
Player  = 1
```
User yang sudah dihapus tidak dapat lagi digunakan untuk melakukan login dan user hanya dapat menghapus akun lain dengan `targetUserLevel` yang lebih rendah (tidak boleh setara atau lebih tinggi).

## D. Pengarsipan ***(20 Points)***
Setelah membuat program yang sangat kompleks, Anton juga ingin merapihkan penyimpanan sistem. Oleh karena itu, bantulah Anton kembali untuk mengarsipkan setiap hasil pertandingan ketika `script.sh` dipanggil yang telah disimpan pada directory `/storage/matchStatistic`. Setiap hasil file yang di generate akan dikumpulkan dan diarsipkan kedalam ZIP file dengan default waktu selama 2 jam menggunakan script `archive.sh`. Hasil ZIP file akan dimasukan kedalam directory `/storage/archive`.

Format penamaan ZIP file adalah seperti berikut:
```txt
MATCH_archive_YYYYMMDD_HHMM.zip
```

Bila user memilih opsi `3` pada script `login.sh`, maka user dapat mengubah interval pengarsipan dengan memasukan input dalam menit seperti berikut:
```txt
Masukan interval pengarsipan (dalam menit): 
```
*Catatan*: Jika user tidak memasukan angka, gunakan waktu default 2 jam.

---
# The Manchester Red Devil Drama Show

Anton is someone who works at FIFA in the IT-Dev division. He really enjoys playing football and watching matches. However, due to his heavy workload, Anton and his friends cannot keep up with the latest football news. An idea suddenly came to Anton’s mind—he wants to create a script that can display the results of ongoing football matches along with the history of previous matches. Help Anton build a system that allows him to view statistics, results, and match history of his favorite football games.

## A. Login and Register ***(30 Points)***

To ensure that not just anyone can use this application, Anton decided to implement a login system. Create a script named: `register.sh`. This script is used to register new users into the system with the following format:

```sh
Nama lengkap 
Tanggal lahir
Role
```

Example Input:

```
Nama: Anton Gonzales
Tanggal lahir (yyyy-mm-dd): 1969-06-09
Role: Analyst
```

There will be 3 roles with the following details:

```sh
1. Coach
2. Analyst
3. Player
```

### Username & Password Generation

The system will automatically generate a username and password with the following rules.  
Username: `First word of the full name`. Example: `Anton Gonzales --> Username: Anton`.  
Password: `Username + birth year + role`, example: `Anton09061969Analyst`.

The data is then stored in `/storage/users.txt` with the format:

```
namaLengkap,tanggal lahir,role,username,password
```

Example:

```
Lionel Messi,2003-04-12,Player,Lionel,Lionel2003Player
```

After creating the `register.sh` script, create another script `login.sh` so users can log in and access the application.

```sh
Username
Password
```

The system will verify whether the credentials match the data in `users.txt`. All login/register activities must be recorded in `/storage/log/log.txt` with the following format:

- If a user tries to register with an already registered username:

```log
[ERROR] DD/MM/YY | hh:mm:ss REGISTER: User {USERNAME} sudah terdaftar!
```

- If registration is successful

```log
[INFO] DD/MM/YY | hh:mm:ss REGISTER: User {USERNAME} berhasil didaftarkan!
```

- If login fails

```log
[ERROR] DD/MM/YY | hh:mm:ss LOGIN: Gagal melakukan login pada user {USERNAME}!
```

- If login succeeds

```log
[INFO] DD/MM/YY | hh:mm:ss LOGIN: User {USERNAME} berhasil login!
```

- If logout succeeds

```log
[INFO] DD/MM/YY | hh:mm:ss LOGIN: User {USERNAME} berhasil logout!
```

*Note*: The system only allows one user to be logged in at a time. If there is already an active user based on the log file, another user cannot log in until the current session ends (the logged-in user performs a logout).

After a user successfully logs in, the script `login.sh` will display the following 4 options:

```sh
===== Pertunjukan Drama Raja Setan Manchester =====
1. Lihat statistik pertandingan
2. Hapus user
3. Ubah waktu pengarsipan
4. Keluar
```

## B. Match Statistics Viewer ***(30 Points)***

After helping Anton create the login and registration system, Anton then asks you to create `script.sh` which will contain a report template like the following:

```sh
====================================================
           LAPORAN STATISTIK PERTANDINGAN
====================================================

Tanggal Laporan : [YYYY MM DD]
Waktu Generate  : [ss:mm:hh]

----------------------------------------------------
STATISTIK UMUM
----------------------------------------------------

Total Pertandingan        : [TOTAL_MATCH]
Total Gol                 : [TOTAL_GOALS]
Rata-rata Shots           : [AVG_SHOTS]

----------------------------------------------------
PERTANDINGAN TERBARU {team1} VS {team2}
----------------------------------------------------

PEMENANG                 : [WINNER]
GOL {team1}              : [GOALWINNINGTEAM]
GOL {team2}              : [GOALLOSINGTEAM]
AVERAGE SHOTS {team1}    : [AVERAGESHOTTEAM1]
AVERAGE SHOTS {team2}    : [AVERAGESHOTTEAM1]

----------------------------------------------------
RIWAYAT PERTANDINGAN 
----------------------------------------------------
No | Home Team | Away Team | Score | Shots | Winner
----------------------------------------------------
1  | Barca     | Madrid    | 2-1   | 12-9  | Barca
2  | City      | Arsenal   | 1-1   | 10-10 | Draw
```

The match history should display a maximum of the 5 most recent matches.

**To maintain application security, Anton wants `script.sh` to not be accessible directly and only be executable when called from `login.sh`.**

Every time `script.sh` is called to display match statistics, the activity must also be recorded in `/storage/log/log.txt` with the following format:

- If the score difference is greater than 2:

```log
[INFO] DD/MM/YY | hh:mm:ss MATCH: HAHAHA, {timKalah} menjadi penghibur yang sangat lucu!
```

- If the score difference is less than or equal to 2:

```log
[INFO] DD/MM/YY | hh:mm:ss MATCH: GACOR, {timMenang} menang bolo!
```

- If the match ends in a draw:

```log
[INFO] DD/MM/YY | hh:mm:ss MATCH: Kok malah DRAW, ga seru ah!
```

In addition to logging, the generated match results from `script.sh` must also be stored in a separate file in the directory `storage/matchStatistic` with the filename format:

```
MATCH_YY-MM-DD_MM:HH.txt
```

*Note*: Use `crontab` to run `generate.sh` every 2 minutes. The data produced by `generate.sh` will later be used by `script.sh` to display match statistics.

## C. User Deletion ***(20 Points)***

If the user selects option `2`, the system will display a list of accounts that can be deleted.  
The requirement for deleting an account is `loggedUserLevel > targetUserLevel`, with the following user levels:

```
Coach   = 3
Analyst = 2
Player  = 1
```

Users that have been deleted can no longer be used to log in. A user can only delete another account if the target account has a lower level (it cannot be equal or higher).

## D. Archiving ***(20 Points)***

After creating a very complex program, Anton also wants to organize the system storage. Therefore, help Anton again by archiving each match result whenever `script.sh` is called and saved in the directory `/storage/matchStatistic`. Every generated result file will be collected and archived into a ZIP file with a default interval of 2 hours using the script `archive.sh`. The ZIP files will be stored in the directory `/storage/archive`.

The ZIP filename format is as follows:

```txt
MATCH_archive_YYYYMMDD_HHMM.zip
```

If the user selects option `3` in the `login.sh` script, the user can change the archiving interval by entering the interval in minutes as follows:

```txt
Masukan interval pengarsipan (dalam menit): 
```

*Note*: If the user does not enter a number, use the default time of 2 hours.