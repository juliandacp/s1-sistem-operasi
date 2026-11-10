#!/bin/bash

BASE="/home/ubuntu/sisop-modul-1-a27/task-3"
DATA_FILE="$BASE/storage/matchResult.csv"
LOG_FILE="$BASE/storage/log/log.txt"
MATCH_DIR="$BASE/storage/matchStatistic"

mkdir -p "$BASE/storage"
mkdir -p "$MATCH_DIR"

write_log() {
    local type=$1
    local action=$2
    local message=$3
    local date=$(date '+%d/%m/%y')
    local time=$(date '+%H:%M:%S')
    echo "[$type] $date | $time $action: $message" >> "$LOG_FILE"
}

hitung_statistik() {
    total_match=0
    total_goal=0
    total_shots_all=0
 
    while IFS=',' read -r home away score shots winner; do
        if [ "$home" != "Home Team" ]; then
            total_match=$((total_match + 1))
 
            home_goal=$(echo "$score" | cut -d'-' -f1)
            away_goal=$(echo "$score" | cut -d'-' -f2)
            total_goal=$((total_goal + home_goal + away_goal))

            home_shot=$(echo "$shots" | cut -d'-' -f1)
            away_shot=$(echo "$shots" | cut -d'-' -f2)
            total_shots_all=$((total_shots_all + home_shot + away_shot))
        fi
    done < "$DATA_FILE"

    if [ $total_match -gt 0 ]; then
        avg_shots=$((total_shots_all / total_match))
    else
        avg_shots=0
    fi
 
    echo "$total_match|$total_goal|$avg_shots|$total_shots_all"
}


tampilkan_riwayat() {
    echo ""
    echo "-------------------------------------"
    echo "RIWAYAT PERTANDINGAN"
    echo "-------------------------------------"
    echo "No | Home Team | Away Team | Score | Shots | Winner"
    echo "-------------------------------------"
    
    nomor=1
    while IFS=',' read -r home away score shots winner; do
        if [ "$home" != "Home Team" ]; then
            echo "$nomor | $home | $away | $score | $shots | $winner"
            nomor=$((nomor + 1))
        fi
    done < "$DATA_FILE"
    
    echo "-------------------------------------"
}


if [ ! -f "$DATA_FILE" ]; then
    echo "====================================="
    echo "Belum ada data pertandingan!"
    echo "====================================="
    exit 1
fi


data_count=$(tail -n +2 "$DATA_FILE" | wc -l)

if [ $data_count -eq 0 ]; then
    echo "====================================="
    echo "Belum ada data pertandingan!"
    echo "====================================="
    exit 1
fi


last_match=$(tail -n 1 "$DATA_FILE")
home_team=$(echo "$last_match" | cut -d',' -f1)
away_team=$(echo "$last_match" | cut -d',' -f2)
score=$(echo "$last_match" | cut -d',' -f3)
shots=$(echo "$last_match" | cut -d',' -f4)
winner=$(echo "$last_match" | cut -d',' -f5)


home_goal=$(echo "$score" | cut -d'-' -f1)
away_goal=$(echo "$score" | cut -d'-' -f2)


home_shot=$(echo "$shots" | cut -d'-' -f1)
away_shot=$(echo "$shots" | cut -d'-' -f2)


stats=$(hitung_statistik)
total_match=$(echo "$stats" | cut -d'|' -f1)
total_goal=$(echo "$stats" | cut -d'|' -f2)
avg_shots=$(echo "$stats" | cut -d'|' -f4)


echo ""
echo "======================================================="
echo "LAPORAN STATISTIK PERTANDINGAN"
echo "======================================================="
echo ""
echo "Tanggal Laporan : $(date '+%Y %m %d')"
echo "Waktu Generate  : $(date '+%H:%M:%S')"
echo ""
echo "--------------------------------------------------------"
echo "STATISTIK UMUM"
echo "--------------------------------------------------------"
echo "Total Pertandingan : $total_match"
echo "Total Gol          : $total_goal"
echo "Average Shots      : $avg_shots"
echo ""
echo "--------------------------------------------------------"
echo "PERTANDINGAN TERBARU $home_team VS $away_team"
echo "--------------------------------------------------------"
echo "PEMENANG            : $winner"
echo "GOL $home_team      : $home_goal"
echo "GOL $away_team      : $away_goal"
echo "AVERAGE SHOTS $home_team : $home_shot"
echo "AVERAGE SHOTS $away_team : $away_shot"


tampilkan_riwayat

echo "========================================================"


diff=$((home_goal - away_goal))
if [ $diff -lt 0 ]; then
    diff=$((diff * -1))
fi


if [ "$winner" = "Draw" ]; then
    write_log "INFO" "MATCH" "Kok malah DRAW, ga seru ah!"
elif [ $diff -gt 2 ]; then
    if [ "$winner" = "$home_team" ]; then
        write_log "INFO" "MATCH" "HAHAHAHA, $away_team menjadi penghibur yang sangat lucu!"
    else
        write_log "INFO" "MATCH" "HAHAHAHA, $home_team menjadi penghibur yang sangat lucu!"
    fi
else
    write_log "INFO" "MATCH" "GACOR, $winner menang bolo!"
fi


filename="MATCH_$(date '+%y-%m-%d_%H:%M').txt"
cat > "$MATCH_DIR/$filename" << EOF
LAPORAN STATISTIK PERTANDINGAN
=====================================================
Tanggal Laporan : $(date '+%Y %m %d')
Waktu Generate  : $(date '+%H:%M:%S')

STATISTIK UMUM
Total Pertandingan : $total_match
Total Gol          : $total_goal
Rata-rata Shots    : $avg_shots

PERTANDINGAN TERBARU $home_team VS $away_team
PEMENANG            : $winner
GOL $home_team      : $home_goal
GOL $away_team      : $away_goal
AVERAGE SHOTS $home_team : $home_shot
AVERAGE SHOTS $away_team : $away_shot

RIWAYAT PERTANDINGAN
No |   Home Team   |   Away Team   |   Score  |  Shots  |  Winner  |
EOF

nomor=1
while IFS=',' read -r home away score shots winner;do
if [ "$home" != "Home Team" ]; then
echo "$nomor | $home | $away | $score | $shots | $winner" >> "$MATCH_DIR/$filename"
nomor=$((nomor+1))
fi
done < "$DATA_FILE"

