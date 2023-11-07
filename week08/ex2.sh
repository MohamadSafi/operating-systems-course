gcc mmu.c -o mmu
gcc pager.c -o pager

mkdir -p /tmp/ex2

./pager 4 2 &
PAGER_PID=$!

./mmu 4 R0 R1 W1 R0 R2 W2 R0 R3 W2 $PAGER_PID