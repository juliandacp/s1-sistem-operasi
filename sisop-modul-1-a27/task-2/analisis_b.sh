#!/bin/bash

awk -F',' '

BEGIN{
    count_punch = 0
    count_wilson = 0
}

{
    nama = $1
    aksi = $2
    durasi = $3
    waktu = $4

    split(waktu, t, " ")
    split(t[1], d, "-")
    split(t[2], jam, ":")

    hari = d[1] + 0
    bulan = d[2]
    tahun = d[3]
    jam_mulai = jam[1] + 0

    if(bulan=="03" && tahun=="2026" && hari >= 1 && hari <= 5){
        if(nama=="Punch" && aksi ~ /tidur/ && jam_mulai > 12) count_punch++
        if(nama=="Wilson" && aksi ~ /makan/ && jam_mulai >= 9 && jam_mulai <= 19) count_wilson++
    }
}

END{
    print "[Punch] tidur sebanyak " count_punch " kali di atas pukul 12.00 siang."
    print "[Wilson] makan sebanyak " count_wilson " kali dari pukul 09.00 sampai 19.00."
}
' losiento.csv
