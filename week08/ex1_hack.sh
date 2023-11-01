PID=$(cat /tmp/ex1.pid)

if ! ps -p $PID > /dev/null; then
    echo "ex1.c is not running."
    exit 1
fi

search_memory_segment() {
    START="$1"
    END="$2"
    
    gdb --batch --pid $PID -ex "dump memory /tmp/dump.bin $START $END" &>/dev/null

PASSWORD=$(strings /tmp/dump.bin | grep -o "pass:......")

    if [[ ! -z "$PASSWORD" ]]; then
        echo "Password found: $PASSWORD"
        sudo kill -9 $PID
        rm /tmp/dump.bin
        exit 0
    fi

    rm /tmp/dump.bin
}

while read -r line; do
    START=$(echo $line | awk '{print $1}' | cut -d'-' -f1)
    END=$(echo $line | awk '{print $1}' | cut -d'-' -f2)
    
    PERMS=$(echo $line | awk '{print $2}')
    if [[ $PERMS == r* ]]; then
        search_memory_segment "0x$START" "0x$END"
    fi
done < "/proc/$PID/maps"

echo "Password not found."
