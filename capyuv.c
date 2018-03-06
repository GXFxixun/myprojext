#include <stdio.h>
#include <unistd.h>
#include "include/camera.h"
#include "include/mynet.h"
#include "include/mytypes.h"
#include <pthread.h>
#include <stdlib.h>

/*
 *此文档主要介绍v4l2接口的使用，并使用其采集yuv格式的图片
 */

u8 *jpgbuf = NULL;
unsigned long int size_jpg = 0;
pthread_mutex_t mutex;

void *camera_pfunc(void *fd)
{
	puts("enter camera_pfunc");
	if(NULL == fd)
	{
		puts("fd is null.");
		return fd;
	}

	int *pfd = (int *)fd;
	*pfd = open_dev("/dev/video0");
	if(*pfd < 0)
	{
		puts("open error.");
		return NULL;
	}
	check_dev_capability(*pfd);
	look_cap_fmt(*pfd);
	set_cap_fmt(*pfd);
	init_mmap(*pfd);
	start_capturing(*pfd);
	main_loop(*pfd);
	stop_capturing(*pfd);
	uninit_device();
	close(*pfd);
	return NULL;	
}

void *client_pfunc(void *pconnfd)
{
	if(NULL == pconnfd)
	{
		puts("pconnfd is null.");
		return NULL;
	}
	
	puts("enter client pfunc");
	int *pcfd = (int *)pconnfd;
	printf("*pcfd:%d\n",*pcfd);
	char buf[32] = {0};
	int ret = recv(*pcfd, buf, 32, 0);
	puts(buf);
	/*
	while(1)
	{
		send_pic(*pcfd, jpgbuf, size_jpg);
	}
	*/
	return NULL;
}

int main()
{
	pthread_mutex_init(&mutex,NULL);
	int fd = -1;
	pthread_t cam_thread;
	int ret = pthread_create(&cam_thread, NULL, camera_pfunc, (void *)&fd);
	if(ret)
	{
		perror("pthread create");
		return -1;
	}
	int ser_sock = init_net();
	int connfd = accept(ser_sock);
	while(1)
	{
		send_pic(connfd, jpgbuf, size_jpg);
	}
	pthread_mutex_destroy(&mutex);
	return 0;
}
