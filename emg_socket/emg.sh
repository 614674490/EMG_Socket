#! /bin/bash


function network()
{
    #超时时间
    local timeout=1

    #目标网站
    local target=www.baidu.com

    #获取响应状态码
    local ret_code=`curl -I -s --connect-timeout ${timeout} ${target} -w %{http_code} | tail -n1`

    if [ "x$ret_code" = "x200" ]; then
        #网络畅通
        return 1
    else
        #网络不畅通
        return 0
    fi

    return 0
}


network
val=$?
while [ $val -eq 0 ]
do
    network
    val=$?
done

echo "network exits"

cd /home/pi/Work/emg_socket/

# g++ -g ./source/*.cpp -I ./include -lpthread -o ./bin/emg_socket

#sleep 20s

./bin/emg_socket --ip 192.168.43.195 --port 225

exit 0

