'''
Author: Ken Kaneki
Date: 2021-09-10 16:40:00
LastEditTime: 2021-10-14 16:50:48
Description: README
'''

import logging
import re
import socket
from threading import Thread

import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation  # 动图的核心函数

vIP = '192.168.43.195'
vPORT = 225
vWAIT_TIME = 30
vCLIENT_NUM = 10
class Connect:

    def __init__(self,IP = vIP,PORT = vPORT,WAIT_TIME = vWAIT_TIME,CLIENT_NUM = vCLIENT_NUM):
        self.ip=IP
        self.port=PORT
        self.wait_time=WAIT_TIME
        self.client_num=CLIENT_NUM
        self.mySocket=0
        self.myClient=0
        self.address=0

    def setSocket(self):
        print("服务端开启")
        # 套接字接口
        self.mySocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            # bind绑定该端口
            self.mySocket.bind((self.ip, self.port))
            self.mySocket.listen(self.client_num)  # 允许同时监听10个客户端，不设置默认为0
            self.mySocket.settimeout(self.wait_time)  # 设置连接超时时间
        except Exception as e:
            print(e)
            return False
        return True

    def waitConnect(self):
        print("等待连接(30s)")
        self.myClient, self.address = self.mySocket.accept()
        print("新连接")
        print("IP is %s" % self.address[0])
        print("port is %d\n" % self.address[1])

    def check_ipv4(self):
        regular = re.compile('^((25[0-5]|2[0-4]\d|[01]?\d\d?)\.){3}(25[0-5]|2[0-4]\d|[01]?\d\d?)$')
        if regular.match(self.ip):
            return True
        else:
            return False

    def con_input(self):
        self.ip = input("IP(default {}): ".format(vIP))
        if len(self.ip) == 0:
            self.ip=vIP

        while not self.check_ipv4():
            print("您输入的ip地址不合法，请重新输入！")
            self.ip = input("IP: ")

        flag=1
        while flag:
            try:
                flag=0
                self.port=input("PORT(default {}): ".format(vPORT))
                if len(self.port)==0:
                    self.port=vPORT
                else:
                    self.port=int(self.port)
            except Exception:
                flag=1
                print("您输入的端口号不合法，请重新输入！")

class plotUI:
    POINTS = 200
    data_list = [0] * POINTS


class getEmg(Thread,Connect):
    emg_val = 0.0
    threshold = 1.25

    def __init__(self, name):
        super().__init__()
        self.name = name
        Connect.__init__(self)
    def setConnect(self):
        self.con_input()

        while not self.setSocket():
            self.con_input()
        con_flag=1
        while con_flag:
            try:
                con_flag=0
                self.waitConnect()
            except socket.error as e:
                con_flag=1
                print(e)

    def run(self):
        LOG_FORMAT = "%(asctime)s - %(levelname)s : %(message)s"
        logging.basicConfig(filename='emg.log', filemode='w',
                            level=logging.INFO, format=LOG_FORMAT)

        while True:
            # 接受数据
            try:
                msg = self.myClient.recv(4, 0)
            except Exception as e:
                print(e)
                self.setConnect()
            # 处理数据
            if len(msg) == 4:
                if (msg[0] == 0xAA and msg[3] == 0xFF):
                    getEmg.emg_val = (float)(
                        msg[1] << 8 | msg[2]) * 187.5 / 1000000.0
                    '''
                    getEmg.emg_val = math.sqrt(getEmg.emg_val)
                    if getEmg.emg_val <= getEmg.threshold:
                        getEmg.emg_val=0
                    '''
                    logging.info("emg_val = %.3f V", getEmg.emg_val)
            # 客户端连接断开处理
            else:
                print('Receive timed out')
                try:
                    self.waitConnect()
                except socket.error as e:
                    print(e)


fig, ax = plt.subplots()
# 设置坐标轴
ax.set_title('emg signal')
ax.set_xlabel('time/ms')
ax.set_ylabel('emg/V')
ax.set_ylim([0.0, 5.0])
ax.set_xlim([0, plotUI.POINTS])
ax.set_autoscale_on(False)
ax.set_xticks(range(0, 200, 10))
ax.set_yticks(range(0, 5, 1))
ax.grid(True)
line_data, = ax.plot(range(plotUI.POINTS), plotUI.data_list)


# 用来更新line_data
def update_line(i):
    plotUI.data_list = plotUI.data_list[1:] + [getEmg.emg_val]
    line_data.set_ydata(plotUI.data_list)
    return [line_data]

emg = getEmg('Thread-1')
emg.setConnect()

emg.start()

ani = FuncAnimation(fig, update_line, interval=1, frames=1000)
plt.show()
