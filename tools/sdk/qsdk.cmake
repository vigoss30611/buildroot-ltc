#!/bin/bash
source tools/qsdk.env
help(){
    echo -e "usage :  must run in qsdk root dir"
    echo -e "\t app source_dir config source_dir [install_dir] [other define]"
    echo -e "\t app source_dir make"
    echo -e "\t app source_dir install"
    echo -e "\t app source_dir clean"
}

if [[ ! -d tools ]];then
    help
    exit 1
fi
target_rootfs=`pwd`/$1
old_pwd=`pwd`
case $2 in
    config )
        echo -e "\nentry source dir"
        cd $1
        echo -e "\tclean old build"
        rm -rf build
        echo -e "\tcreate temp build dir"
        mkdir -p build
        cd build
        echo -e "\tcongigure and create makefile"
        cmake $3  $4 $5 $6 $7 ..
        ls
        pwd
        cd $old_pwd
        ;;
    make )
        cd $1/build
        echo -e "\nmake target"
        make
        cd $old_pwd
        ;;
    install )
        echo -e "\ninstall target"
        cd $1/build
        make install
        cd $old_pwd
        ;;
    clean )
        echo -e "\nclean temp build"
        cd $1/build
        rm -rf build
        cd $old_pwd
        ;;
    * )
        help
        ;;
esac
