if [ $1 = "x86" ];
then
    echo "****************** create *********************"
    echo "         ********* module **********"
    make arch=$1 modname=mycdev
    rmmod mycdev
    insmod mycdev.ko
    make arch=$1 modname=mycdev clean
    echo "         ********* module **********"
    echo ""
    echo "         ********* gcc **********"
    gcc test.c -o test
    echo "         ********* gcc **********"
    echo "******************create*********************"
    echo ""
    echo "***************************** run *********************************"
    ./test
fi