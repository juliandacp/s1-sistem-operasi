#!/bin/bash

mkdir -p storage

FILE="/home/ubuntu/sisop-modul-1-a27/task-3/storage/matchResult.csv"

if [ ! -f "$FILE" ] || [ ! -s "$FILE" ]; then
echo "Home Team,Away Team,Score,Shots,Winner" > "$FILE"
fi

teams=("Barca" "Madrid" "City" "Arsenal" "Bayern" "PSG" "Liverpool" "MU" "Juventus" "Inter")

home_team=${teams[$RANDOM % ${#teams[@]}]}
away_team=${teams[$RANDOM % ${#teams[@]}]}

while [ "$home_team" = "$away_team" ]; do
away_team=${teams[$RANDOM % ${#teams[@]}]}
done

home_scores=$((RANDOM % 5))
away_scores=$((RANDOM % 5))

home_shots=$((RANDOM % 20 + 5))
away_shots=$((RANDOM % 20 + 5))

if [ $home_scores -gt $away_scores ]; then
winner="$home_team"
elif [ $away_scores -gt $home_scores ]; then
winner="$away_team"
else
winner="Draw"
fi

new_data="${home_team},${away_team},${home_scores}-${away_scores},${home_shots}-${away_shots},${winner}"

data_count=$(tail -n +2 "$FILE" | wc -l)

if [ $data_count -lt 5 ]; then
echo "$new_data" >> "$FILE"
else
tail -n +2 "$FILE" > /tmp/data.txt

echo "$new_data" >> /tmp/data.txt

tail -n 5 /tmp/data.txt > /tmp/last5.txt

echo "Home Team,Away Team,Score,Shots,Winner" > "$FILE"
cat /tmp/last5.txt >> "$FILE"
rm -f /tmp/data.txt /tmp/last5.txt
fi

echo "Data generated: $home_team $home_scores- $away_scores $away_team" >> /home/ubuntu/latihan-github/generate.log
