# scp /home/ubuntu/Desktop/tencent.pem z5092597@grieg.cse.unsw.edu.au:/srvr/z5092597/testing/
# scp -i tencent.pem ubuntu@quzicheng.com:/home/ubuntu/Desktop/COMP9315_21T1/intset.c .
# scp -i tencent.pem ubuntu@quzicheng.com:/home/ubuntu/Desktop/COMP9315_21T1/intset.source .

# 为了清晰DEBUG，将旧的控制台内容清空
clear
# 复制本地文件到postgresql的目录下
cp -rf intset.c /home/ubuntu/Desktop/postgresql-12.5/src/tutorial/
cp -rf intset.source /home/ubuntu/Desktop/postgresql-12.5/src/tutorial/
cp -rf Makefile /home/ubuntu/Desktop/postgresql-12.5/src/tutorial/
cp -rf test.sql /home/ubuntu/Desktop/postgresql-12.5/src/tutorial/

# 删除postgresql的目录下的sql文件，如果哪里出错，则不会继续用旧的sql执行
rm -rf /home/ubuntu/Desktop/postgresql-12.5/src/tutorial/intset.sql

# 修改程序执行目录，source和make
source /home/ubuntu/Desktop/COMP9315/pgsql/env
cd /home/ubuntu/Desktop/postgresql-12.5/src/tutorial
make

# 执行测试脚本
dropdb ass1db
createdb ass1db
psql ass1db -f intset.sql
psql ass1db -f test.sql

# 如果没有权限
# chmod a+x test.sh
# ./test.sh