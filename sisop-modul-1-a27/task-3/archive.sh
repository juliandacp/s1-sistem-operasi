#!/bin/bash

BASE="/home/ubuntu/sisop-modul-1-a27/task-3"
MATCH_DIR="$BASE/storage/matchStatistic"
ARCHIVE_DIR="$BASE/storage/archive"
CONFIG_FILE="$BASE/storage/archive_interval.txt"

mkdir -p "$MATCH_DIR"
mkdir -p "$ARCHIVE_DIR"

if [ -f "$CONFIG_FILE" ]; then
interval=$(cat "$CONFIG_FILE")
else
interval=120
fi

echo "=================================================================="
echo "                 ARCHIVE STATISTIK PERTANDINGAN                   "
echo "=================================================================="
echo "Interval pengarsipan: $interval menit"
echo ""

batas_waktu=$((interval * 60))
arsip_count=0

for file in "$MATCH_DIR"/*.txt; do
if [ -f "$file" ]; then
filename=$(basename "$file")
date_part=$(echo "$filename" | grep -oP '\d{2}-\d{2}-\d{2}_\d{2}:\d{2}')
if [ -n "$date_part" ]; then
file_date=$(echo "$date_part" | cut -d'_' -f1)
file_time=$(echo "$date_part" | cut -d'_' -f2)

file_timestamp=$(date -d "20${file_date:6:2}-${file_date:3:2}-${file_date:0:2} $file_time" +%s 2>/dev/null)
current_timestamp=$(date +%s)

if [ -n "$file_timestamp" ]; then
diff=$((current_timestamp - file_timestamp))

if [ $diff -ge $batas_waktu ]; then
zip_name="MATCH_archive_$(date '+%Y%m%d_%H%M').zip"
zip -j "$ARCHIVE_DIR/$zip_name" "$file"

if [ $? -eq 0 ]; then
rm -f "$file"
echo "Arsip: $filename -> $zip_name"
arsip_count=$((arsip_count + 1))
fi
fi
fi
fi
fi
done
echo ""
echo "========================================================================="
echo "Selesai! $arsip_count file diarsipkan"
echo "========================================================================"

