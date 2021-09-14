
import serial
import  csv
import time



if __name__ == '__main__':
    ser = serial.Serial()

    ser.baudrate = 9600  # 设置波特率（这里使用的是stc89c52）
    ser.port = 'COM4'

    print(ser)
    ser.open()  # 打开串口
    print(ser.is_open)
    k=0
    headers=['time', 'ACC_x','ACC_y','ACC_z','Gyro_x','Gyro_y','Gyro_z','Angle_x','Angle_y','Angle_z','pressure_0','pressure_1','pressure_2']
    with open('qiye.csv','a',newline='') as f:
        f_csv = csv.writer(f)
        f_csv.writerow(headers)
        f.flush()

        while (1):
            s = []
            for i in range(6):
                s.append(str(ser.readline()))

            print(s[0].split(' '))
            print(s[1].split(' '))
            print(s[2].split(' '))
            print(s[3].split(' '))
            print(s[4].split(' '))
            print(s[5].split(' '))
            print('\n')
            s_0 = s[0].split(' ')
            s_1 = s[1].split(' ')
            s_2 = s[2].split(' ')
            s_3 = s[3].split(' ')
            s_4 = s[4].split(' ')
            s_5 = s[5].split(' ')
            localtime = time.asctime(time.localtime(time.time()))
            row = [localtime, s_0[1], s_0[2], s_0[3], s_1[1], s_1[2], s_1[3], s_2[1], s_2[2], s_2[3], s_3[1], s_4[1],
                   s_5[1]]
            f_csv.writerow(row)
            f.flush()








