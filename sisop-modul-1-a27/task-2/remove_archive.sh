#!/bin/bash

for file in /home/julian/sisop-modul-1-a27/task-2/backup_log_*.zip
do
    nama_file=$(basename "$file")
    
    tanggal=$(echo "$nama_file" | grep -oP '[0-9]{8}')
    
    hari=$(echo "$tanggal" | cut -c1-2)
    bulan=$(echo "$tanggal" | cut -c3-4)
    tahun=$(echo "$tanggal" | cut -c5-8)
    
    tanggal_file=$(date -d "$tahun-$bulan-$hari" +%s)
    now=$(date +%s)
    diff=$(( (now - tanggal_file) / 86400 ))

    if [[ "$diff" -gt 7 ]]
    then
        rm "$file"
    fi
done