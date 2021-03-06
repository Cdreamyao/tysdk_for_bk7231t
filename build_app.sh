#!/bin/sh
if [ -z "$1" ];then
        echo "please input the app bin name(no suffix \".bin\")!!!"
        exit 1
else
        APP_BIN_NAME=$1
fi

if [ -z "$2" ];then
        echo "please input the app sw version(for example:the format is "1.1.1")!!!"
        exit 1
else
        USER_SW_VER=$2
fi
#  "1.0.1"
#NUM_PY = 101
# $3 compile parameter command as user set,for example clean and so on.  ${APP_BIN_NAME}_${USER_SW_VER}_crc_basic.bin
USER_DEF_CMD=$3

echo ""
echo "start..."
echo ""
set -e

for i in `find project/tuya_user/$APP_BIN_NAME/src -type d`
do
echo $i
rm -rf $i/*.o
done

for i in `find project/tuya_common/src -type d`
do
echo $i
rm -rf $i/*.o
done

make APP_BIN_NAME=$APP_BIN_NAME USER_SW_VER=$USER_SW_VER $USER_DEF_CMD -C ./

cp project/tuya_user/$APP_BIN_NAME/output/$USER_SW_VER/${APP_BIN_NAME}_${USER_SW_VER}.bin tools/generate/


cd tools/generate/
./otafix ${APP_BIN_NAME}_${USER_SW_VER}.bin &
./encrypt ${APP_BIN_NAME}_${USER_SW_VER}.bin 510fb093 a3cbeadc 5993a17e c7adeb03 10000
python mpytools.py ${APP_BIN_NAME}_${USER_SW_VER}_enc.bin

./beken_packager config.json

cp all_1.00.bin ../../project/tuya_user/$APP_BIN_NAME/output/$USER_SW_VER/${APP_BIN_NAME}_QIO_${USER_SW_VER}.bin
rm all_1.00.bin

cp ${APP_BIN_NAME}_${USER_SW_VER}_enc_uart_1.00.bin ../../project/tuya_user/$APP_BIN_NAME/output/$USER_SW_VER/${APP_BIN_NAME}_uart_${USER_SW_VER}.bin
rm ${APP_BIN_NAME}_${USER_SW_VER}_enc_uart_1.00.bin

#generate ota file
./rt_ota_packaging_tool_cli -f ${APP_BIN_NAME}_${USER_SW_VER}.bin -v $CURRENT_TIME -o ${APP_BIN_NAME}_${USER_SW_VER}.rbl -p app -c gzip -s aes -k 0123456789ABCDEF0123456789ABCDEF -i 0123456789ABCDEF
./package.bin ${APP_BIN_NAME}_${USER_SW_VER}.rbl ../../project/tuya_user/$APP_BIN_NAME/output/$USER_SW_VER/${APP_BIN_NAME}_ug_${USER_SW_VER}.bin $USER_SW_VER 
rm ${APP_BIN_NAME}_${USER_SW_VER}.rbl
rm ${APP_BIN_NAME}_${USER_SW_VER}.bin

echo "ug_file size:"
ls -l ../../project/tuya_user/$APP_BIN_NAME/output/$USER_SW_VER/${APP_BIN_NAME}_ug_${USER_SW_VER}.bin | awk '{print $5}'
if [ `ls -l ../../project/tuya_user/$APP_BIN_NAME/output/$USER_SW_VER/${APP_BIN_NAME}_ug_${USER_SW_VER}.bin | awk '{print $5}'` -gt 679936 ];then
	echo "*********************************************************************"
	echo "*********************************************************************"
	echo "********************bk7231_${APP_BIN_NAME}_$USER_SW_VER.bin**********"
	echo "************************** too large ********************************"
	echo "*********************************************************************"
	echo "*********************************************************************"
	echo "**********************COMPILE FAILED*********************************"
	rm ../../project/tuya_user/$APP_BIN_NAME/output/$USER_SW_VER/${APP_BIN_NAME}_ug_${USER_SW_VER}.bin
	rm ../../project/tuya_user/$APP_BIN_NAME/output/$USER_SW_VER/${APP_BIN_NAME}_uart_${USER_SW_VER}.bin
	rm ../../project/tuya_user/$APP_BIN_NAME/output/$USER_SW_VER/${APP_BIN_NAME}_QIO_${USER_SW_VER}.bin
	exit 1
fi

cd -

echo "*************************************************************************"
echo "*************************************************************************"
echo "*************************************************************************"
echo "**********************bk7231_${APP_BIN_NAME}_$USER_SW_VER.bin*********************"
echo "*************************************************************************"
echo "**********************COMPILE SUCCESS************************************"
echo "*************************************************************************"