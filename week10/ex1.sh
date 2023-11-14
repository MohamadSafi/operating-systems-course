
DIR_PATH="$1"


gcc monitor.c -o monitor


gcc ex1.c -o ex1


./monitor "$DIR_PATH" &
MONITOR_PID=$!

sleep 2

./ex1 "$DIR_PATH"

kill $MONITOR_PID

