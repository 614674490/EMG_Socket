#include <ads1115.h>
#include <head.h>

using namespace std;

bool emgCollector::Init_EmgHardWork()
{
    try
    {
        I2CFile = open("/dev/i2c-1", O_RDWR); //获取文件描述符
    }
    catch (exception e)
    {
        cout << e.what() << endl; //捕获异常，然后程序结束
        return false;
    }

    if (I2CFile < 0)
    {
        printf("Open I2C failed\n");
        return false;
    }

    if (ioctl(I2CFile, I2C_TENBIT, 0) < 0)
    {
        printf("Couldn't set I2C device 7 bit\n");
        return false;
    }

    if (ioctl(I2CFile, I2C_SLAVE, ADS_ADDRESS) < 0)
    {
        printf("Couldn't find I2C device\n");
        return false;
    }
    printf("I2CFile = %d\n", I2CFile);
    uint8_t writeBuf[3];
    int result = 0;
    //写入配置寄存器
    writeBuf[0] = ADS1115_REG_POINTER_CONFIG; //Pointer Byte: 01->config registe;00->conversion registe
    writeBuf[1] = CONFIG_REG_H;               //config register Data high OS=1;MUX=AIN0-GND;PGA=4.096V;MODE=Continuous-conversion mode
    writeBuf[2] = CONFIG_REG_L;               //config register Data low DR=128SPS;default
    try
    {
        result = write(I2CFile, writeBuf, 3); //返回写入文件的字节数
    }
    catch (exception e)
    {
        cout << e.what() << endl; //捕获异常，然后程序结束
        return false;
    }

    if (result != 3)
    {
        printf("write config registe: result = %d, errno=%d\n", result, errno);
        return false;
    }
    //point conversion registe
    writeBuf[0] = ADS1115_REG_POINTER_CONVERT; //selects ADCS for conversion
    try
    {
        result = write(I2CFile, writeBuf, 1);
    }
    catch (exception e)
    {
        cout << e.what() << endl; //捕获异常，然后程序结束
        return false;
    }

    if (result != 1)
    {
        printf("point conversion registe: result = %d, errno=%d\n", result, errno);
        return false;
    }
    printf("I2C Device Successed\n");
    usleep(1000); //usleep(sleeptime);
    return true;
}

//建议采用经过稳压后的直流电压3.3V-5V，切勿使用插排的交流电压！
bool emgCollector::ReadOneEmgDate()
{
    int nread = 0;
    uint16_t count = 0;
    //memset(readBuf, 0, READBUFFLEN);
    while (nread <= 0)
    {
        try
        {
            nread = read(I2CFile, readBuf, READBUFFLEN); //返回读取到的字节数(2 byte)
            if (count++ > 1000)
            {
                cout << "read timeout\n";
                return false;
            }
        }
        catch (exception e)
        {
            cout << e.what() << endl; //捕获异常，然后程序结束
            return false;
        }
    }
    raw_val = readBuf[0] << 8 | readBuf[1];        //高位在前 低位在后
    if ((raw_val == 0x7FFF) | (raw_val == 0X8000)) //是否超量程了
    {
        printf("emg signal over PGA\r\n");
    }
    emg_val = (float)raw_val * ADS1115_PGA_6_RATE;
    return true;
}
