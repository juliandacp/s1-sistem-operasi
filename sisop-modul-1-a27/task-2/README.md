# Monyet Punch dan Ayam Wilson

Seekor monyet bernama Punch dan ayam jantan bernama Wilson menjadi terkenal di internet karena perjuangan hidupnya yang tidak mudah. Untuk mengenal Punch dan Wilson, para penggemar membuat sebuah sistem otomatis yang mencatat semua aktivitas mereka sepanjang hari. Semua aktivitas tersebut disimpan dalam file CSV [losiento.csv](https://github.com/oceanite/soalmodul1-sisop26/blob/main/task-2/losiento.csv)

Contoh isi file :

```

    nama_hewan,aksi,durasi_detik,waktu_mulai
    Punch,tidur_siang,7200,20-03-2026 08:10:56
    Punch,makan_siang,650,20-03-2026 12:10:34
    Wilson,bermain,530,20-03-2026 12:15:32

```

Karena data aktivitas Punch dan Wilson semakin banyak dan sulit dianalisis secara manual, penggemar membuat program analisis otomatis menggunakan Shell Script dan AWK. 

## A. Aktivitas Lama ***(20 Points)***
Penggemar ingin mengetahui aktivitas yang berlangsung lebih dari 2 jam oleh kedua hewan di tanggal 7 Maret 2026. Script untuk analisis ini disimpan dalam file [analisis_a.sh](https://github.com/oceanite/soalmodul1-sisop26/blob/main/task-2/analisis_a.sh). Tampilkan nama hewan, nama aksi dan durasi aktivitas. Format output: `[(Nama_Hewan)] (aktivitas) selama (waktu) detik pada (ddmmyyyy)`. Contoh: `[Punch] tidur siang selama 300 detik pada 07032026`.
Note: karakter _ pada kolom aktivitas diganti dengan spasi.

## B. Analisis Frekuensi ***(25 Points)***
Karena khawatir akan kesehatan Punch dan Wilson yang menurun pada rentang tanggal 1 Maret 2026 sampai 5 Maret 2026, penggemar ingin mengetahui seberapa sering Punch tidur di atas pukul 12.00 siang dan Wilson makan antara pukul 09.00 sampai 19.00. Script untuk analisis ini disimpan dalam file [analisis_b.sh](https://github.com/oceanite/soalmodul1-sisop26/blob/main/task-2/analisis_b.sh) dengan format output sebagai berikut:

`[Punch] tidur sebanyak n kali di atas pukul 12.00 siang.` 

atau

`[Wilson] makan sebanyak n kali dari pukul 08.00 sampai 19.00` 

Note: n merupakan frekuensi dilakukannya aktivitas.

## C. Pencatatan Aktivitas ***(25 Points)***
Sekarang kamu bertugas untuk mengamati dan mencatat aktivitas yang dilakukan kedua hewan pada file [losiento.csv](https://github.com/oceanite/soalmodul1-sisop26/blob/main/task-2/losiento.csv) di atas. Input dilakukan dengan menjalankan program [input_aktivitas.sh](https://github.com/oceanite/soalmodul1-sisop26/blob/main/task-2/input_aktivitas.sh) dengan ketentuan sebagai berikut:
Format input:
```
Nama :
Aktivitas :
Durasi : 
Waktu mulai :
```
Kemudian kamu harus mencatat riwayat input pada file log yang terpisah berdasarkan nama dan tanggal aktivitas. 
Format penamaan file log adalah sebagai berikut : `(namahewan)_ddmmyyyy.log`. 

File log mencatat aktivitas dengan format sebagai berikut: `[ACCEPTED] (timestamp) : (nama_hewan) (aktivitas) selama (durasi) detik`

Jika input aktivitas yang dimasukkan memiliki durasi lebih besar dari 21600 detik, maka log akan mencatat dengan format sebagai berikut: `[ALERT] (timestamp) : (nama_hewan) (aktivitas) selama (durasi) detik`

## D. Arsip Log ***(20 Points)***
Setelah aktivitas Punch dan Wilson dicatat pada file log, buatlah script [backup_log.sh](https://github.com/oceanite/soalmodul1-sisop26/blob/main/task-2/backup_log.sh) yang:

- Mengarsipkan semua file .log yang memiliki tanggal yang sama
- Menyimpannya dengan format: `backup_log_ddmmyyyy.zip`, misalnya: `backup_log_08032026.zip`
- Menampilkan isi file zip tersebut.

Contoh output setelah script dijalankan:
```
Log berhasil diarsipkan.
Isi arsip:
Punch_07032026.log
Wilson_07032026.log
```

## E. Penjadwalan Cron ***(10 Points)***
Penggemar ingin agar proses pengarsipan dijalankan secara otomatis setiap pukul 23:59 menggunakan [backup_log.sh](https://github.com/oceanite/soalmodul1-sisop26/blob/main/task-2/backup_log.sh). Karena jumlah arsip semakin banyak, maka setiap pukul 02:34 sistem akan menghapus arsip yang sudah berusia lebih dari 7 hari menggunakan script [remove_archive.sh](https://github.com/oceanite/soalmodul1-sisop26/blob/main/task-2/remove_archive.sh). Simpan konfigurasi penjadwalan tersebut pada file [crontab](https://github.com/oceanite/soalmodul1-sisop26/blob/main/task-2/crontab)

---
# Punch the Monkey and Wilson the Rooster

A monkey named Punch and a rooster named Wilson became famous on the internet because of their difficult life journey. To learn more about Punch and Wilson, fans created an automated system that records all of their activities throughout the day. All of these activities are stored in the CSV file [losiento.csv](https://github.com/oceanite/soalmodul1-sisop26/blob/main/task-2/losiento.csv)

Example file content :

```

    nama_hewan,aksi,durasi_detik,waktu_mulai
    Punch,tidur_siang,7200,20-03-2026 08:10:56
    Punch,makan_siang,650,20-03-2026 12:10:34
    Wilson,bermain,530,20-03-2026 12:15:32

```

Because the activity data of Punch and Wilson keeps increasing and becomes difficult to analyze manually, fans created an automated analysis program using Shell Script and AWK.

## A. Long Activities ***(20 Points)***
Fans want to find activities that lasted more than 2 hours for both animals on March 7, 2026. The script for this analysis is stored in the file [analisis_a.sh](https://github.com/oceanite/soalmodul1-sisop26/blob/main/task-2/analisis_a.sh). Display the animal name, activity name, and activity duration. Output format: `[(Nama_Hewan)] (aktivitas) selama (waktu) detik pada (ddmmyyyy)`. Example: `[Punch] tidur siang selama 300 detik pada 07032026`.

Note: the `_` character in the activity column should be replaced with a space.

## B. Frequency Analysis ***(25 Points)***
Because fans are concerned about the declining health of Punch and Wilson between March 1, 2026 and March 5, 2026, they want to know how often Punch sleeps after 12:00 PM and how often Wilson eats between 09:00 and 19:00. The script for this analysis is stored in the file [analisis_b.sh](https://github.com/oceanite/soalmodul1-sisop26/blob/main/task-2/analisis_b.sh) with the following output format:

`[Punch] tidur sebanyak n kali di atas pukul 12.00 siang.` 

or

`[Wilson] makan sebanyak n kali dari pukul 08.00 sampai 19.00` 

Note: n represents the frequency of the activity.

## C. Activity Logging ***(25 Points)***
Now you are responsible for observing and recording the activities performed by both animals in the file [losiento.csv](https://github.com/oceanite/soalmodul1-sisop26/blob/main/task-2/losiento.csv) above. Input is performed by running the program [input_aktivitas.sh](https://github.com/oceanite/soalmodul1-sisop26/blob/main/task-2/input_aktivitas.sh) with the following conditions:

Input format:
```
Nama :
Aktivitas :
Durasi : 
Waktu mulai :
```

After that, you must record the input history in a separate log file based on the animal's name and the activity date.  
The log filename format is as follows: `(namahewan)_ddmmyyyy.log`.

The log file records activities with the following format:  
`[ACCEPTED] (timestamp) : (nama_hewan) (aktivitas) selama (durasi) detik`

If the entered activity has a duration greater than 21600 seconds, the log will record it with the following format:  
`[ALERT] (timestamp) : (nama_hewan) (aktivitas) selama (durasi) detik`

## D. Log Archive ***(20 Points)***
After the activities of Punch and Wilson are recorded in the log files, create a script [backup_log.sh](https://github.com/oceanite/soalmodul1-sisop26/blob/main/task-2/backup_log.sh) that:

- Archives all `.log` files that have the same date
- Saves them using the format: `backup_log_ddmmyyyy.zip`, for example: `backup_log_08032026.zip`
- Displays the contents of the zip file.

Example output after the script is executed:
```
Log berhasil diarsipkan.
Isi arsip:
Punch_07032026.log
Wilson_07032026.log
```

## E. Cron Scheduling ***(10 Points)***
Fans want the archiving process to run automatically every day at 23:59 using [backup_log.sh](https://github.com/oceanite/soalmodul1-sisop26/blob/main/task-2/backup_log.sh). Because the number of archives keeps increasing, the system will delete archives that are older than 7 days every day at 02:34 using the script [remove_archive.sh](https://github.com/oceanite/soalmodul1-sisop26/blob/main/task-2/remove_archive.sh). Save the scheduling configuration in the file [crontab](https://github.com/oceanite/soalmodul1-sisop26/blob/main/task-2/crontab)