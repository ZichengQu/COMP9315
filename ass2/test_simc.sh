#chmod +x ./test.sh
# ./test.sh

clear

printf "\nrm -f R.bsig R.data R.info R.psig R.tsig\n"
rm -f R.bsig R.data R.info R.psig R.tsig

printf "\nmake clean\n"
make clean

printf "make\n"
make

printf "\n./create R simc 10100 4 1000\n"
./create R simc 10100 4 1000

printf "\n./gendata 10000 4 7654321 23 > R.in\n"
./gendata 10000 4 7654321 23 > R.in

printf "\n./insert R < R.in\n"
./insert R < R.in

printf "\n./stats R\n"
./stats R

printf "\n./select R '7663852,?,?,?'\n"
./select R '7663852,?,?,?'

printf "\n./select R '7663852,?,?,?' t\n"
./select R '7663852,?,?,?' t

printf "\n./select R '7663852,?,?,?' p\n"
./select R '7663852,?,?,?' p

printf "\n./select R '7663852,?,?,?' b\n"
./select R '7663852,?,?,?' b

printf "\n./select R '7664096,PjZZsBYoEYAMzgpCgRKg,?,?'\n"
./select R '7664096,PjZZsBYoEYAMzgpCgRKg,?,?'

printf "\n./select R '7664096,PjZZsBYoEYAMzgpCgRKg,?,?' t\n"
./select R '7664096,PjZZsBYoEYAMzgpCgRKg,?,?' t

printf "\n./select R '7664096,PjZZsBYoEYAMzgpCgRKg,?,?' p\n"
./select R '7664096,PjZZsBYoEYAMzgpCgRKg,?,?' p

printf "\n./select R '7664096,PjZZsBYoEYAMzgpCgRKg,?,?' b\n"
./select R '7664096,PjZZsBYoEYAMzgpCgRKg,?,?' b

printf "\n./select R '?,?,a3-242,a4-242'\n"
./select R '?,?,a3-242,a4-242'

printf "\n./select R '?,?,a3-242,a4-242' t\n"
./select R '?,?,a3-242,a4-242' t

printf "\n./select R '?,?,a3-242,a4-242' p\n"
./select R '?,?,a3-242,a4-242' p

printf "\n./select R '?,?,a3-242,a4-242' b\n"
./select R '?,?,a3-242,a4-242' b

printf "\n./select R '8765432,?,?,?'\n"
./select R '8765432,?,?,?'

printf "\n./select R '8765432,?,?,?' t\n"
./select R '8765432,?,?,?' t

printf "\n./select R '8765432,?,?,?' p\n"
./select R '8765432,?,?,?' p

printf "\n./select R '8765432,?,?,?' b\n"
./select R '8765432,?,?,?' b

printf "\n./select R '7664096,tRzgWRUlUEdoYPZjofYr,?,?'\n"
./select R '7664096,tRzgWRUlUEdoYPZjofYr,?,?'

printf "\n./select R '7664096,tRzgWRUlUEdoYPZjofYr,?,?' t\n"
./select R '7664096,tRzgWRUlUEdoYPZjofYr,?,?' t

printf "\n./select R '7664096,tRzgWRUlUEdoYPZjofYr,?,?' p\n"
./select R '7664096,tRzgWRUlUEdoYPZjofYr,?,?' p

printf "\n./select R '7664096,tRzgWRUlUEdoYPZjofYr,?,?' b\n"
./select R '7664096,tRzgWRUlUEdoYPZjofYr,?,?' b

rm R.bsig
rm R.data
rm R.in
rm R.info
rm R.psig
rm R.tsig


