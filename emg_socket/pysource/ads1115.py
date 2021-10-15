import Adafruit_ADS1x15
import time
'''
增益
2/3 = +/-6.144V
1 = +/-4.096V
2 = +/-2.048V
4 = +/-1.024V
8 = +/-0.512V
16 = +/-0.256V
'''

GAIN = 1
adc1 = Adafruit_ADS1x15.ADS1115(address=0x48)
while True:
    '''
    A0 = Channel 0 minus channel 1
    A1 = Channel 0 minus channel 3
    A2 = Channel 1 minus channel 3
    A3 = Channel 2 minus channel 3
    data_rate = 128 为每秒返回样本数 最高为860
    '''
    print(adc1.read_adc(0, gain=GAIN, data_rate=128))
    time.sleep(1.0)
