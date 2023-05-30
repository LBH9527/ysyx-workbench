#!/bin/bash

#define echo print color.
RED_COLOR='\E[1;31m'
PINK_COLOR='\E[1;35m'
YELOW_COLOR='\E[1;33m'
BLUE_COLOR='\E[1;34m'
GREEN_COLOR='\E[1;32m'
END_COLOR='\E[0m'
PLAIN='\033[0m'

# Global variables 
    # Set linu host user name.
    user_name=bli123

# Check the results of the operation.
check_status() {
    ret=$?
    if [ "$ret" -ne "0" ]; then
        echo -e "${RED_COLOR}Failed setup, aborting..${END_COLOR}"
        exit 1
    fi
}

 


# check ubuntu version
function check_ubuntu_version(){
    Var=$(lsb_release -r)
    # echo "$Var"
    NumOnly=$(cut -f2 <<< "$Var")
    echo "$NumOnly"
    # return $NumOnly
}


# Check user must root.
check_root() {
    if [ $(id -u) != "0" ]; then
        echo -e "${RED_COLOR}Error: This script must be run as root!${END_COLOR}"
        exit 1
    fi
}

# Check set linux host user name.

# Install common software and configuration
install_common_software() {
    sudo apt-get update
    check_status

    # local install_software_list=("ssh" "git" "vim" "tftp" "nfs" "samba")
    local install_software_list=("git")

    echo -e "${BLUE_COLOR}install_software_list: ${install_software_list[*]}.${END_COLOR}"

    #install ssh
    if (echo "${install_software_list[@]}" | grep -wq "ssh");then
        apt-get -y install openssh-server && echo -e "${BLUE_COLOR}ssh install completed.${END_COLOR}"
    fi

    #install git, # revision control system
    if (echo "${install_software_list[@]}" | grep -wq "git");then
        sudo apt-get -y install git && echo -e "${BLUE_COLOR}git install completed.${END_COLOR}"
    fi

    #install and configure vim
    #if (echo "${install_software_list[@]}" | grep -wq "vim");then
        #apt-get -y install vim && vim_configure && echo -e "${BLUE_COLOR}vim install completed.${END_COLOR}"
    #fi

    #install and configure tftp
    if (echo "${install_software_list[@]}" | grep -wq "tftp");then
        apt-get -y install tftp-hpa tftpd-hpa && tftp_configure && echo -e "${BLUE_COLOR}tftp install completed.${END_COLOR}"
    fi

    #install and configure nfs
    if (echo "${install_software_list[@]}" | grep -wq "nfs");then
        apt-get -y install nfs-kernel-server && nfs_configure && echo -e "${BLUE_COLOR}nfs install completed.${END_COLOR}"
    fi

    #install and configure samba
    if (echo "${install_software_list[@]}" | grep -wq "samba");then
        apt-get -y install samba && samba_configure && echo -e "${BLUE_COLOR}samba install completed.${END_COLOR}"
    fi
}

# install tools for PAs
function install_tools_for_PAS() {
    echo -e "${YELOW_COLOR}start install tools for PAs.${END_COLOR}"
    sudo apt-get update
    check_status

    sudo apt-get install build-essential    # build-essential packages, include binary utilities, gcc, make, and so on
    sudo apt-get install man                # on-line reference manual
    sudo apt-get install gcc-doc            # on-line reference manual for gcc
    sudo apt-get install gdb                # GNU debugger
     
    sudo apt-get -y install libreadline-dev    # a library used later
    sudo apt-get -y install libsdl2-dev        # a library used later
    sudo apt-get -y install llvm llvm-dev      # llvm project, which contains libraries used later
    result=$( check_ubuntu_version )
    # echo "$result"
    if  [ $result = "20.04" ]; then
        sudo apt-get -y install llvm-11 llvm-11-dev # only for ubuntu20.04
        echo -e "${BLUE_COLOR}llvm-11 install completed.${END_COLOR}" 
    fi

    sudo apt-get -y install clang-format
    sudo apt-get -y install bison flex			# for make menuconfig
    sudo apt-get -y install gcc-riscv64-linux-gnu # install toolchain
    echo -e "${YELOW_COLOR}Done.${END_COLOR}"
}
# vim config

# update ysyx workbench
function update_ysyx_workbench() {
    echo -e "${YELOW_COLOR}start update ysyx workspace.${END_COLOR}"
    bash init.sh nemu
    bash init.sh abstract-machine
    bash init.sh am-kernels
    bash init.sh nanos-lite
    bash init.sh navy-apps
    bash init.sh nvboard

    # nemu 和 abstract-machine 目录 已经存在，不需要`init.sh` 来下载， 但是需要添加这两个目录到 bashrc
    bash init.sh addenv
    source ~/.bashrc
    echo $NEMU_HOME
    echo $AM_HOME

    echo -e "${YELOW_COLOR}Done.${END_COLOR}"
}



# Select menu
# # `echo -e "\E[1;33mPlease select the host use:\E[0m"`
# `echo -e "\E[1;33m    1. Configuring for Harmony OS development \E[0m"`
# `echo -e "\E[1;33m    2. Configuring for Linux development\E[0m"`
# `echo -e "\E[1;33m    3. Configuring for Android development\E[0m"`
# `echo -e "\E[1;33m    4. Quit\E[0m"`

function menu() {
cat <<EOF
`echo -e "\E[1;31mgit clone https://github.com/LBH9527/ysyx-workbench.git\E[0m"` 
EOF
}


function main() {
menu

install_common_software
install_tools_for_PAS
update_ysyx_workbench

while true 
do 
    exit 0
done
}

main 
exit 0


