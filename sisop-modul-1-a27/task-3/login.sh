#!/bin/bash

USERS_FILE="storage/users.txt"
LOG_FILE="storage/log/log.txt"

cek_sesi_aktif() {
if [ ! -f "$LOG_FILE" ]; then
return 1
fi 

last_line=$(tail -n 1 "$LOG_FILE" 2>/dev/null)

if echo "$last_line" | grep -q "LOGIN: User" && ! echo "$last_line" | grep -q "LOGOUT"; then
return 0
fi
return 1
}

get_level() {
case $1 in
"Coach") echo 3 ;;
"Analyst") echo 2 ;;
"Player") echo 1 ;;
"coach") echo 3 ;;
"analyst") echo 2 ;;
"player") echo 1 ;;
*) echo 0 ;;
esac
}

write_log(){
local type=$1
local action=$2
local message=$3
local date=$(date '+%d/%m/%y')
local time=$(date '+%H:%M:%S')
echo "[$type] $date | $time $action: $message" >> "$LOG_FILE"
}

hapus_user() {
echo " "
echo "=========================================================="
echo "                     HAPUS USER                           "
echo "=========================================================="

read -p "Masukkan username yang akan dihapus: " user_to_delete
user_data=$(grep ",${username}" "$USERS_FILE")
target_data=$(grep ",${user_to_delete}," "$USERS_FILE")

if [ -z "$target_data" ]; then
echo "==========================================================="
echo "ERROR: User '$user_to_delete' tidak ditemukan!"
echo "==========================================================="
read -p "Tekan Enter untuk kembali..."
return 
fi

if [ "$user_to_delete" = "$username" ]; then
echo "==========================================================="
echo "ERROR: Anda tidak bisa menghapus akun sendiri! "
echo "==========================================================="
read -p "Tekan Enter untuk kembali..."
return 
fi


target_role=$(echo "$target_data" | cut -d',' -f3)
target_level=$(get_level "$target_role")
user_role=$(echo "$user_data" | cut -d',' -f3)
user_level=$(get_level "$user_role")

if [ $user_level -le $target_level ]; then
echo "==========================================================="
echo "ERROR: Tidak bisa menghapus user ini!"
echo "-----------------------------------------------------------"
echo "Level Anda           : $user_role ($user_level)"
echo "Level target         : $target_role ($target_level)"
echo "-----------------------------------------------------------"
echo "Anda hanya bisa menghapus user dengan level LEBIH RENDAH!"
echo "==========================================================="
write_log "ERROR" "MENU" "User $username gagal hapus $user_to_delete"
read -p "Tekan Entar untuk kembali..."
return 
fi

echo ""
echo "User yang akan dihapus"
echo " Username   : $user_to_delete"
echo " Role       : $target_role"
echo " Level      : $target_level"
read -p "Yakin ingin menghapus? (y/n): " confirm

if [ "$confirm" = "y" ] || [ "$confirm" = "Y" ]; then
sed -i "/,$user_to_delete,/d" "$USERS_FILE"
echo "==========================================================="
echo "User '$user_to_delete' berhasil dihapus"
echo "==========================================================="
write_log "INFO" "MENU" "User $username ($user_role) menghapus user $user_to_delete ($target_role)"
else
echo "Penghapusan dibatalkan"

fi

read -p "Tekan Enter untuk kembali..."
}
lihat_statistik(){
echo ""
if [ -f "./script.sh" ]; then
./script.sh
else
echo "==============================================="
echo "          ERROR: Belum ada laporan!            "
echo "==============================================="
fi
read -p "Tekan Enter untuk kembali..."
}

ubah_waktu() {
echo "" 
echo "==============================================="
echo "             UBAH WAKTU PENGARSIPAN            "
echo "==============================================="
echo "Waktu default: 120 menit (2 jam)               "
read -p "Masukkan interval pengarsipan (dalam menit): " interval
if [ -z "$interval" ]; then
interval=120
echo "Menggunakan waktu default: 120 menit"
elif ! [[ "$interval" =~ ^[0-9]+$ ]]; then
echo "=============================================="
echo "ERROR : Input harus berupa angka!"
echo "=============================================="
write_log "ERROR" "MENU" "User $username gagal mengubah waktu pengarsipan (input bukan angka)"
read -p "Tekan Enter untuk kembali..."
return
fi

echo "$interval" > storage/archive_interval.txt
echo "=================================================="
echo "Waktu pengarsipan diubah menjadi: $interval menit"
echo "=================================================="
write_log "INFO" "MENU" "$username mengubah waktu pengarsipan menjadi $interval menit"
read -p "Tekan ENter untuk kembali..."
}

echo "=================================================="
echo "                    LOGIN SYSTEM                  "
echo "=================================================="

active_session=$(cek_sesi_aktif)
if [ $? -eq 0 ]; then
echo "=================================================="
echo "ERROR: Sedang ada user yang aktif!                "
echo "User aktif: $active_session"
echo "Tunggu hingga user tersebut logout                "
echo "=================================================="
exit 1
fi

read -p "Username: " username
read -p "Password: " password

if [ ! -f "$USERS_FILE" ]; then
echo "=================================================="
echo "ERROR: Belum ada user terdaftar                   "
echo "=================================================="
exit 1
fi

user_data=$(grep ",${username}," "$USERS_FILE")

if [ -z "$user_data" ]; then
echo "=================================================="
echo "ERROR: Username tidak ditemukan !"
echo "=================================================="
write_log "ERROR" "LOGIN" "Gagal melakukan login pada user $username"
exit 1
fi

stored_password=$(echo "$user_data" | cut -d',' -f5)
fullname=$(echo "$user_data" | cut -d',' -f1)
role=$(echo "$user_data" | cut -d',' -f3)

if [ "$password" != "$stored_password" ]; then
echo "=================================================="
echo "ERROR: Password salah!"
echo "=================================================="
write_log "ERROR" "LOGIN" "Gagal melakukan login pada user $username"
exit 1
fi

write_log "INFO" "LOGIN" "User $username berhasil login"

echo ""
echo "===================================================="
echo "                LOGIN BERHASIL                    "
echo "===================================================="
echo "Selamat datang, $fullname!"
echo "Username        : $username"
echo "Role            : $role "
echo "Level           : $(get_level "$role")"
echo "==================================================="

while true; do
echo ""
echo "===================================================="
echo "      Pertunjukan Drama Raja Setan MAnchester      "
echo "===================================================="
echo "1. Lihat statistik pertandingan"
echo "2. Hapus user "
echo "3. Ubah waktu pengarsipan"
echo "4. Keluar"
echo "===================================================="
read -p "Pilih menu (1-4): " menu

case $menu in
1) 
lihat_statistik
;;
2)
hapus_user
;;
3)
ubah_waktu
;;
4)
write_log "INFO" "LOGIN" "User $username berhasil logout!"
echo "===================================================="
echo "                 LOGOUT BERHASIL                   "
echo "===================================================="
echo "Terima kasih, $fullname!"
echo "===================================================="
break
;;
*)
echo "===================================================="
echo "ERROR: Pilihan tidak valid!"
echo "Silahkan pilih 1,2,3,4"
echo "===================================================="
;;

esac
done
