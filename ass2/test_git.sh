
#chmod +x ./test_git.sh
# ./test_git.sh

if test -z "$1"
then
  echo '请输出提交的内容！！命令执行格式是：./test_git.sh "Comment"'
else
    # printf "git add .\n"
    git add .

    # printf 'git commit -m ""\n'
    git commit -m "$1"

    # printf "git push\n"
    git push
fi