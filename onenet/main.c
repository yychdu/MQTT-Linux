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

int main(int argc, char *argv[])
{
    pthread_t thread_ID;		//定义线程id

    pthread_create(&thread_ID, NULL, &cloud_mqtt_thread, NULL);	//创建一个线程执行mqtt客户端
    pthread_detach(thread_ID);	//设置线程结束收尸
    char str[] = "{\
    \"id\": 123,  \      
    \"dp\": {      \       
        \"temp\": [{  \   
            \"v\": 25,\
        }],\
        \"humi\": [{  \   
            \"v\": 80, \       
        }]\
    }\
}";
    printf("%s\r\n", str);
    while (1)
    {
        mqtt_data_write(str, sizeof(str), 0);//循环发布"my yes"
        sleep(3);						//睡眠3s
    }

    return 0;
}

