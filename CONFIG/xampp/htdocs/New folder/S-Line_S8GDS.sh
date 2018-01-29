#!/bin/sh

#S-Line_S7B
#collaudatore: mp


ERROR=""
NCORE=`cat /proc/cpuinfo | grep processor | wc -l`
CPU=`dmesg | grep processors | grep $NCORE`
if [ -n "$CPU" ]
then
echo CPU OK >> report.txt
else

unset CPU
 echo CPU KO >> report.txt
fi
[ "$CPU" == "" ] && ERROR="CPU Error"
time sh -c "dd if=/dev/zero of=/dev/mmcblk1 bs=4K count=1 && sync" &> /dev/null
if [ $? -ge 0 ]
then
EMMC="ok"
echo eMMC ok >> report.txt
else
unset EMMC
echo eMMC KO >> report.txt
fi
[ "$EMMC" == "" ] && ERROR="eMMC Error"
ETH=`dmesg | grep eth0`
if [ -n "$ETH" ]
then
echo Ethernet OK >> report.txt
else
unset ETH
echo Ethernet KO >> report.txt
fi
[ "$ETH" == "" ] && ERROR="Ethernet Error"
RAM=`cat /proc/meminfo | grep MemTotal `
if [ -n "$RAM" ]
then
echo RAM OK >> report.txt
else
unset RAM
echo RAM KO >> report.txt
fi
[ "$RAM" == "" ] && ERROR="RAM Error"
if [ "$ERROR" == "" ]; then
RES="OK"
else
RES="KO"
fi
rm b*
get_exec b.php?argument1=$RES
