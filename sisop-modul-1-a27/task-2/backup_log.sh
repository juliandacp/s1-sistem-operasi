#!/bin/bash

cd /home/julian/sisop-modul-1-a27/task-2

file=$(ls *.log 2>/dev/null)

if [[ -z "$file" ]]
then
    echo "Tidak ada file log."
    exit 1
fi

list_tanggal=$(echo "$file" | grep -oE '[0-9]{8}' | sort -u)

for tanggal in $list_tanggal
do
    file_zip="backup_log_${tanggal}.zip"
    zip "$file_zip" *_${tanggal}.log > /dev/null
    echo "Log berhasil diarsipkan."
    echo "Isi arsip:"
    unzip -Z1 "$file_zip"
    echo ""
done