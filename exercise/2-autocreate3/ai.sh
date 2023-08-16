if [ $# -ne 2 ]; then
    echo "Usage: $0 [add|x86|arm|clean]"
    exit 1
fi

basic_path=/home/ming/Code/drive/
present_path=$(pwd)
foldername=$2
file_name=${present_path##*-}
# 1. select functions
# 1.1 add a new folder
if [ $1 = "add" ]; then
    echo "****************** create folder *********************"
    mkdir $basic_path/exercise/$foldername
    touch $basic_path/exercise/$foldername/test.c
    touch $basic_path/exercise/$foldername/${foldername##*-}.c
    touch $basic_path/exercise/$foldername/${foldername##*-}.h
    cp $basic_path/ai.sh $basic_path//exercise/$foldername/
    cp $basic_path/tools/Makefile $basic_path//exercise/$foldername/
    echo "****************** create success *********************"
    exit 0
fi
# 1.2 test program
if [ $1 = "make" ]; then
    if [ $2 = "x86" ]; then
        echo "****************** x86 create *********************"
        make arch=$1 modname=$file_name
        rmmod $file_name
        insmod $file_name.ko
        make arch=$1 modname=$file_name clean
        gcc test.c -o test
        echo "***************************** run *********************************"
        ./test
        exit 0
    elif [ $2 = "arm" ]; then
        echo "****************** arm create *********************"
        make arch=$1 modname=$file_name
        cp $file_name.ko /home/ming/rootfsmy/root/
        make arch=$1 modname=$file_name clean
        gcc test.c -o test
        cp test /home/ming/rootfsmy/root/
        rm test
        echo "************** move and clean *****************"
    fi
fi
# 1.3 clean
if [ $1 = "clean" ]; then
    echo "****************** clean *********************"
    if [ $2 = "x86" ]; then
        make arch=x86 modname=$file_name clean
    elif [ $2 = "arm" ]; then
        make arch=arm modname=$file_name clean
    fi
    rm test
    exit 0
fi