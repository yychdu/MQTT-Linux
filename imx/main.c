/*************************************************************************
	> File Name: main.c
	> Author: LFJ
	> Mail: 
	> Created Time: 2018年09月05日 星期三 13时48分17秒
 ************************************************************************/

#include <stdio.h>
#include "pthread.h"
#include "mqtt.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "sys/ioctl.h"
#include "fcntl.h"
#include "stdlib.h"
#include "string.h"
#include <poll.h>
#include <sys/select.h>
#include <sys/time.h>
#include <signal.h>
#include <fcntl.h>
int main(int argc, char *argv[])
{
    int fd;
	char *filename;
	signed int databuf[7];
	unsigned char data[14];
	signed int gyro_x_adc, gyro_y_adc, gyro_z_adc;
	signed int accel_x_adc, accel_y_adc, accel_z_adc;
	signed int temp_adc;

	float gyro_x_act, gyro_y_act, gyro_z_act;
	float accel_x_act, accel_y_act, accel_z_act;
	float temp_act;
    char str[1024];
    int ret = 0;
    fd = open("/dev/icm20608", O_RDWR);
	if(fd < 0) {
		printf("can't open file %s\r\n", "/dev/icm20608");
		return -1;
	}
    pthread_t thread_ID;		//定义线程id

    pthread_create(&thread_ID, NULL, &cloud_mqtt_thread, NULL);	//创建一个线程执行mqtt客户端
    pthread_detach(thread_ID);	//设置线程结束收尸
    while (1)
    {
        ret = read(fd, databuf, sizeof(databuf));
		if(ret == 0) { 			/* 数据读取成功 */
			gyro_x_adc = databuf[0];
			gyro_y_adc = databuf[1];
			gyro_z_adc = databuf[2];
			accel_x_adc = databuf[3];
			accel_y_adc = databuf[4];
			accel_z_adc = databuf[5];
			temp_adc = databuf[6];

			/* 计算实际值 */
			gyro_x_act = (float)(gyro_x_adc)  / 16.4;
			gyro_y_act = (float)(gyro_y_adc)  / 16.4;
			gyro_z_act = (float)(gyro_z_adc)  / 16.4;
			accel_x_act = (float)(accel_x_adc) / 2048;
			accel_y_act = (float)(accel_y_adc) / 2048;
			accel_z_act = (float)(accel_z_adc) / 2048;
			temp_act = ((float)(temp_adc) - 25 ) / 326.8 + 25;


			// printf("\r\n原始值:\r\n");
			// printf("gx = %d, gy = %d, gz = %d\r\n", gyro_x_adc, gyro_y_adc, gyro_z_adc);
			// printf("ax = %d, ay = %d, az = %d\r\n", accel_x_adc, accel_y_adc, accel_z_adc);
			// printf("temp = %d\r\n", temp_adc);
			// printf("实际值:");
			// printf("act gx = %.2f°/S, act gy = %.2f°/S, act gz = %.2f°/S\r\n", gyro_x_act, gyro_y_act, gyro_z_act);
			// printf("act ax = %.2fg, act ay = %.2fg, act az = %.2fg\r\n", accel_x_act, accel_y_act, accel_z_act);
			// printf("act temp = %.2f°C\r\n", temp_act);
		}
        memset(str, 0, sizeof(str));
        // {\"id\": 123,        \"dp\": {              \"temp\": [{       \"v\": 1, }],\"humi\": [{      \"v\": 0,         }]  }}
        sprintf(str, "{\"id\": 123, \"dp\": { \"temp\": [{ \"v\":%.2f}],\"gx\": [{ \"v\":%.2f}],\"gy\": [{ \"v\":%.2f}],\"gz\": [{ \"v\":%.2f}],\"ax\": [{ \"v\":%.2f}],\"ay\": [{ \"v\":%.2f}],\"az\": [{ \"v\":%.2f}]}}",temp_act,gyro_x_act, gyro_y_act, gyro_z_act, accel_x_act, accel_y_act, accel_z_act);
        mqtt_data_write(str, sizeof(str), 0);//循环发布"my yes"
        sleep(3);						//睡眠3s
    }
    close(fd);
    return 0;
}

