gcc ex3.c -o ex3

./ex3 3 &

sleep 20

pstree -p

./ex3 5 &

sleep 30

pstree -p
