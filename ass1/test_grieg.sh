# ssh z5092597@grieg.cse.unsw.edu.au
# scp intset.c intset.source test_grieg.sh test_grieg.sql z5092597@grieg.cse.unsw.edu.au:/srvr/z5092597/testing
# ./test_grieg.sh 在testing下执行
# 为了清晰DEBUG，将旧的控制台内容清空
clear
# 复制本地文件到postgresql的目录下
cp -rf intset.c /srvr/z5092597/postgresql-12.5/src/tutorial
cp -rf intset.source /srvr/z5092597/postgresql-12.5/src/tutorial
cp -rf Makefile /srvr/z5092597/postgresql-12.5/src/tutorial
cp -rf test_grieg.sql /srvr/z5092597/postgresql-12.5/src/tutorial

# 删除postgresql的目录下的sql文件，如果哪里出错，则不会继续用旧的sql执行
rm -rf /srvr/z5092597/postgresql-12.5/src/tutorial

# 修改程序执行目录，source和make
source /srvr/z5092597/env
cd /srvr/z5092597/postgresql-12.5/src/tutorial
make

# 执行测试脚本
dropdb ass1db
createdb ass1db
psql ass1db -f intset.sql
psql ass1db -f test_grieg.sql

# 如果没有权限
# chmod a+x test.sh
# ./test.sh