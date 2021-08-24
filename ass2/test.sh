#chmod +x ./test.sh
# ./test.sh

clear

make clean

printf "make\n"
make

printf "\n./create R simc 10000 6 1000\n"
./create R simc 10000 6 1000

printf "\n./gendata 10000 6 1234567 13 > R.in\n"
./gendata 10000 6 1234567 13 > R.in

printf "\n./insert R < R.in\n"
./insert R < R.in

printf "\n./stats R\n"
./stats R

printf "\n./select R '?,?,?,?,?,?' x | tail -6\n"
./select R '?,?,?,?,?,?' x | tail -6

printf "\n./select R '1234999,?,?,?,?,?' x\n"
./select R '1234999,?,?,?,?,?' x

printf "\n./select R '1234999,?,?,?,?,?' t\n"
./select R '1234999,?,?,?,?,?' t

printf "\n./select R '1234999,?,?,?,?,?' p\n"
./select R '1234999,?,?,?,?,?' p

printf "\n./select R '1234999,?,?,?,?,?' b\n"
./select R '1234999,?,?,?,?,?' b

printf "\ngrep 'a3-241,a4-158,a5-407' R.in\n"
grep 'a3-241,a4-158,a5-407' R.in

printf "\n./select R '?,?,a3-241,a4-158,a5-407,?' x\n"
./select R '?,?,a3-241,a4-158,a5-407,?' x

printf "\n./select R '?,?,a3-241,a4-158,a5-407,?' t\n"
./select R '?,?,a3-241,a4-158,a5-407,?' t

printf "\n./select R '?,?,a3-241,a4-158,a5-407,?' p\n"
./select R '?,?,a3-241,a4-158,a5-407,?' p

printf "\n./select R '?,?,a3-241,a4-158,a5-407,?' b\n"
./select R '?,?,a3-241,a4-158,a5-407,?' b

rm R.bsig
rm R.data
rm R.in
rm R.info
rm R.psig
rm R.tsig


