#!/bin/bash

echo -n "Nama : "
read nama

echo -n "Aktivitas : "
read aktivitas

echo -n "Durasi : "
read durasi

echo -n "Waktu mulai : "
read waktu

format_aktivitas=$(echo "$aktivitas" | tr ' ' '_')

tanggal=$(echo "$waktu" | cut -d' ' -f1)

hari=$(echo "$tanggal" | cut -d'-' -f1)
bulan=$(echo "$tanggal" | cut -d'-' -f2)
tahun=$(echo "$tanggal" | cut -d'-' -f3)

format_tanggal="${hari}${bulan}${tahun}"

log_file="${nama}_${format_tanggal}.log"

if [[ "$durasi" -gt 21600 ]]
then
    status="ALERT"
else
    status="ACCEPTED"
fi

echo "[$status] $waktu : $nama $aktivitas selama $durasi detik" >> "$log_file"
echo "${nama},${format_aktivitas},${durasi},${waktu}" >> losiento.csv