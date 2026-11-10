#!/bin/bash

mkdir -p storage
mkdir -p storage/log

USERS_FILE="storage/users.txt"
LOG_FILE="storage/log/log.txt"

write_log(){
local type=$1
local action=$2
local message=$3

local date=$(date '+%d/%m/%y')
local time=$(date '+%H:%M:%S')
echo "[$type] $date | $time  $action: $message">>"$LOG_FILE"
}

echo "===================================================="
echo "                REGISTRASI USER BARU             "
echo "===================================================="

read -p "Nama Lengkap              : " fullname
read -p "Tanggal lahir (yyyy-mm-dd): " birthdate
read -p "Role                      : " role

if [ "$role" != "Coach" ] && [ "$role" != "Analyst" ] && [ "$role" != "Player" ] && [ "$role" != "coach" ] && [ "$role" != "analyst" ] && [ "$role" != "player" ]; then
echo "ERROR: Role tidak vaid!"
write_log "ERROR" "REGISTER" "Role tidak valid!"
exit 1
fi

username=$(echo $fullname | cut -d' ' -f1)
tahun=$(echo $birthdate | cut -d'-' -f1)
password="${username}${tahun}${role}"

if [ -f "$USERS_FILE" ] && grep -q ",${username}," "$USERS_FILE"; then
echo "ERROR: Username '$username' sudah terdaftar!"
write_log "ERROR" "REGISTER" "User $username sudah terdaftar!"
exit 1
fi

echo "${fullname},${birthdate},${role},${username},${password}" >> "$USERS_FILE"
write_log "INFO" "REGISTER" "User $username berhasil didaftarkan"

echo "=================================================="
echo "              REGISTRASI BERHASIL                 "
echo "=================================================="
echo "Nama lengkap    : $fullname "
echo "Tanggal Lahir   : $birthdate"
echo "Role            : $role     "
echo "Username        : $username "
echo "Password        : $password "
echo "=================================================="


