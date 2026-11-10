#!/bin/bash

awk -F ',' '
{
    nama = $1
    aksi = $2
    durasi = $3
    waktu = $4

    split(waktu, t, " ")
    split(t[1], d, "-")

    if(d[1]=="07" && d[2]=="03" && d[3]=="2026" && durasi > 7200){
        gsub("_", " ", aksi)
        tanggal = d[1] d[2] d[3]
        print "[" nama "] " aksi " selama " durasi " detik pada " tanggal
    }
}
' losiento.csv
