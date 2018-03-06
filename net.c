#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/in.h>

int init_net()
{
	int sock_ser = -1;
	sock_ser = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_ser < 0)
	{
		puts("create socket is failed.");
		return -1;
	}
	puts("create socket success.");

	struct sockaddr_in ser;
	memset(&ser, 0, sizeof(struct sockaddr_in));
	ser.sin_family = AF_INET;
	ser.sin_port = htons(8888);
	ser.sin_addr.s_addr = htonl(INADDR_ANY);
	
	int ret = bind(sock_ser, (struct sockaddr *)&ser, sizeof(struct sockaddr_in));
	if(ret != 0)
	{
		puts("bind error.");
		close(sock_ser);
		return -1;
	}
	puts("bind success.");

	ret = listen(sock_ser, 5);
	if(ret != 0)
	{
		puts("listen error.");
		close(sock_ser);
	}
	puts("listen success.");
	return sock_ser;
}

int accept_client(int sock_ser)
{
	if(sock_ser < 0)
	{
		puts("sock_ser < 0:invalue argument.");
		return -1;
	}

	struct sockaddr_in iClient;
	int connfd = -1;
	int len = sizeof(struct sockaddr_in);
	connfd = accept(sock_ser, (struct sockaddr *)&iClient, &len);
	return connfd;
}

extern pthread_mutex_t mutex;
int send_pic(int connfd, char *jpgbuf, int size_jpg)
{
	if(connfd < 0)
	{
		puts("connfd < 0");
		return -1;
	}

	if(jpgbuf == NULL)
	{
		puts("jpgbuf is null pointer.");
		return -1;
	}

	char request[32] = {0};
	char response[128] = {0};

	memset(request, 0, sizeof(request));
	int ret = recv(connfd,request,sizeof(request),0);
	if(ret > 0)
	{
		puts(request);
	}
	else
	{
		puts("buf is null.");
		exit(0);
	}

	if(strstr(request, "pic") != NULL)
	{
		snprintf(response, sizeof(response), "%dlen", size_jpg);
	}

	puts(response);
	ret = send(connfd, response, sizeof(response), 0);
	
	if(ret != sizeof(response))
	{
		puts("send response failed");
		return -1;
	}
	pthread_mutex_lock(&mutex);
	ret = send(connfd, jpgbuf, size_jpg, 0);
	pthread_mutex_unlock(&mutex);
	if(ret != size_jpg)
	{
		puts("send buf error.");
		return -1;
	}
	return 0;
}
