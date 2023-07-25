if [ $# -ne 2 ]; then
    echo "Usage: $0 [install|uninstall|test]"
    exit 1
fi

modules_path=/lib/modules/5.19.0-50-generic/extra
drive_path=/dev/my*

if [ $1 = "install" ]; then
    echo "****************** install *********************"
    dmesg -c
    insmod $modules_path/$2.ko
    dmesg
    chmod 777 /dev/my*
    exit 0
elif [ $1 = "uninstall" ]; then
    echo "****************** uninstall *********************"
    rmmod $2
    dmesg
    exit 0
elif [ $1 = "reinstall" ]; then
    echo "****************** reinstall *********************"
    rmmod $2
    dmesg
    insmod $modules_path/$2.ko
    dmesg
    chmod 777 /dev/my*
    exit 0
elif [ $1 = "test" ]; then
    echo "****************** test *********************"
    dmesg -c
    insmod $modules_path/$2.ko
    dmesg
    chmod 777 /dev/my*
    echo "****************** start *********************"
    ./test
    exit 0
else
    echo "Usage: $0 [install|uninstall]"
    exit 1
fi