date
sleep 3
mkdir ./folder1

date
sleep 3
mkdir ./folder2

date
sleep 3
ls -ltr / > "folder1/root.txt" 

date
sleep 3
ls -ltr ~ > "folder2/home.txt" 


cat folder1/root.txt

cat folder2/home.txt

ls -ltr folder1

ls -ltr folder2
