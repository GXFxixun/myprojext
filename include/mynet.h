#ifndef __NET_H__
#define __NET_H__

/*
 *函数名：init_net
 *函数功能：实现创建服务器套节字、绑定、监听，并返回创建的套节字的文件描述符
 *函数参数：void(没有)
 *函数返回值：int：成功返回创建的套节字文件描述符，失败返回-1
 */
int init_net();

/*
 *函数名：accept_client
 *函数功能：接受客户端的连接请求，并返回通信套节字
 *函数参数：int sock_ser:创建后被绑定的套节字文件的文件描述符
 *函数返回值：int:成功返回通信套节字的文件描述符，失败返回-1
 */
int accept_client(int sock_ser);

/*
 *函数名：send_pic
 *函数功能：发送一张图片给客户端
 *函数参数：int connfd:客户端和服务器通信的套节字
 *	    char *jpgbuf:jpg图片的缓存首地址
 *	    int size_jpg:jpg图片的大小
 *函数返回值：成功返回0，失败返回-1
 */
int send_pic(int connfd, char *jpgbuf, int size_jpg);

#endif
