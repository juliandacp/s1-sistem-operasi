#!/bin/bash
LOG_FILE="activity.log"
METADATA_FILE="metadata.csv"
ASSETS_DIRECTORY="assets"

log_msg(){
    local level=$1 cmd=$2 msg=$3
    echo "[$(date +'%Y-%m-%d %H:%M:%S')] [$level] [$cmd] $msg" >> "$LOG_FILE"
}

do_detect(){
    for file in "$ASSETS_DIRECTORY"/*; do
        [ -e "$file" ] || continue
        filename=$(basename "$file")
        if ! grep -q "^$filename," "$METADATA_FILE" 2>/dev/null; then
            size=$(stat -c%s "$file")
            ext="${filename##*.}"
            [ "$ext" == "$filename" ] && ext=""
            created_at=$(date -r "$file" +"%Y-%m-%d_%H:%M:%S")
            echo "$filename,$size,$ext,$created_at" >> "$METADATA_FILE"
            log_msg "INFO" "SYNC" "Added new file $filename"
        fi
    done
}

do_clean(){
    if [ -s "$METADATA_FILE" ]; then
        temp=$(mktemp)
        cp "$METADATA_FILE" "$temp"
        while IFS=, read -r filename size ext created_at; do
            if [ -n "$filename" ] && [ ! -f "$ASSETS_DIRECTORY/$filename" ]; then
                sed -i "/^$filename,/d" "$METADATA_FILE"
                log_msg "INFO" "SYNC" "Removed missing file $filename from database"
            fi
        done < "$temp"
        rm "$temp"
    fi
}

do_sync(){
    do_detect;
    do_clean;
}

do_autosync(){
    local cron_cmd="*/5 * * * * cd $(pwd) && ./script.sh sync"
    if crontab -l 2>/dev/null | grep -qF "./script.sh sync"; then
        log_msg "INFO" "AUTOSYNC" "Autosync already installed"
        echo "Autosync exist."
    else
        (crontab -l 2>/dev/null; echo "$cron_cmd") | crontab -
        echo "$cron_cmd" > crontabs
        log_msg "INFO" "AUTOSYNC" "Installed autosync every 5 minutes"
        echo "Autosync installed."
    fi
}

do_list(){
    local sort_by="" ext_filter="" tmp_list=$(mktemp)

    for arg in "$@"; do
        case $arg in
            --sort=name) sort_by="name" ;;
            --sort=size) sort_by="size" ;;
            --ext=*) ext_filter="${arg#*=}" ;;
        esac
    done
    
    if [ -n "$ext_filter" ]; then
        awk -F, -v ext="$ext_filter" '$3 == ext {print $0}' "$METADATA_FILE" > "$tmp_list"
    else
        cat "$METADATA_FILE" > "$tmp_list"
    fi

    if [ ! -s "$tmp_list" ]; then
        log_msg "INFO" "LIST" "Query returned empty result"
        rm "$tmp_list"
        return
    fi

    if [ "$sort_by" == "name" ]; then
        sort -t, -k1,1 "$tmp_list" -o "$tmp_list"
    elif [ "$sort_by" == "size" ]; then
        sort -t, -k2,2n "$tmp_list" -o "$tmp_list"
    fi

    echo -e "FILENAME, SIZE, EXTENSION, CREATED_AT" | cat - "$tmp_list" | column -s, -t
    log_msg "INFO" "LIST" "Displayed database content"
    rm "$tmp_list"
}

do_stats(){
    if [ ! -s "$METADATA_FILE" ]; then
        log_msg "INFO" "STATS" "Accessed empty database"
        echo "Database empty."
        return
    fi

    local total_asset=$(wc -l < "$METADATA_FILE")
    local total_size=$(awk -F, '{sum+=$2} END {print sum}' "$METADATA_FILE")
    local avg_size=$(awk -F, '{sum+=$2} END {print sum/NR}' "$METADATA_FILE")
    local maximum=$(awk -F, 'BEGIN {max=-1; name=""} {if($2>max) {max=$2; name=$1}} END {print name " (" max " bytes)"}' "$METADATA_FILE")
    local minimum=$(awk -F, 'BEGIN {min=99999999999; name=""} {if($2<min) {min=$2; name=$1}} END {print name " (" min " bytes)"}' "$METADATA_FILE")

    echo "Total Assets     : $total_asset"
    echo "Total Size       : $total_size bytes"
    echo "Average Size     : $avg_size bytes"
    echo "Maximum Asset    : $maximum"
    echo "Minimum Asset    : $minimum"
    echo "Files by Ext     :"
    awk -F, '{print $3}' "$METADATA_FILE" | sort | uniq -c | awk '{print "  - " $2 ": " $1}'

    log_msg "INFO" "STATS" "Generated statistics"
}

do_create(){
    local filename=$1 size=$2

    if [[ -z "$filename" || -z "$size" ]]; then
        log_msg "ERROR" "CREATE" "Missing arguments"
        echo "Error: missing arguments."
        return
    fi

    if ! [[ "$size" =~ ^[0-9]+$ ]]; then
        log_msg "ERROR" "CREATE" "Invalid size: $size"
        echo "Error: invalid size."
        return
    fi

    if [ -f "$ASSETS_DIRECTORY/$filename" ]; then
        log_msg "ERROR" "CREATE" "File $filename already exists"
        echo "Error: file already exists."
        return
    fi

    dd if=/dev/zero of="$ASSETS_DIRECTORY/$filename" bs=1 count="$size" status=none
    local ext="${filename##*.}" [ "$ext" == "$filename" ] && ext=""
    local created_at=$(date +"%Y-%m-%d_%H:%M:%S")
    echo "$filename,$size,$ext,$created_at" >> "$METADATA_FILE"

    log_msg "INFO" "CREATE" "Created file $filename ($size bytes)"
    echo "Created file $filename ($size bytes)"
}

do_delete() {
    local filename=$1

    if [ -z "$filename" ]; then
        log_msg "ERROR" "DELETE" "No filename provided"
        echo "Error: no filename."
        return
    fi

    if [ ! -f "$ASSETS_DIRECTORY/$filename" ]; then
        log_msg "ERROR" "DELETE" "$filename not found"
        echo "Error: file not found."
        return
    fi

    rm "$ASSETS_DIRECTORY/$filename"
    sed -i "/^$filename,/d" "$METADATA_FILE"

    log_msg "INFO" "DELETE" "Deleted $filename"
    echo "deleted $filename"
}

case "$1" in
    sync)
        do_sync;
        do_autosync
        ;;
    list)
        shift
        do_list "$@"
        ;;
    stats)
        do_stats
        ;;
    create)
        do_create "$2" "$3"
        ;;
    delete)
        do_delete "$2"
        ;;
    *)
        echo "command list: ./script.sh sync | list | stats | create | delete"
        ;;
esac