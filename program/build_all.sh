# サブディレクトリの全てのプログラムをビルドする
for dir_list in `find ./ -maxdepth 1 -type d`
do
    if [ $dir_list != "./" -a $dir_list != "./share" ]; then
        cd $dir_list
        make -j4 1> /dev/null 2> log
        if [ ! -s log ]; then
            echo "$dir_list"
        else
            err=`cat log`
            echo -e "\033[1;31m\n$dir_list error\033[0;39m"
            echo -e "$err\n"
        fi
        rm log
        cd ../
    fi
done