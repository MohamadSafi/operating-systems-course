
gcc pager.c -o pager
gcc mmu.c -o mmu

./pager 4 2 &
PAGER_PID=$!

sleep 2

./mmu 4 R0 R1 W1 R0 R2 W2 R0 R3 W2 $PAGER_PID

kill $PAGER_PID



