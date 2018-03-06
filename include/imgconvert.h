#ifndef __IMGCONVERT_H__
#define __IMGCONVERT_H__
#include "mytypes.h"

/*
 *函数名：yuvtorgb24
 *函数功能：YUYV422转RGB24
 *函数返回值：int 成功返回0，失败返回-1
 *函数参数：u8 *rgb_buf:用于存储rgb图片的buf的首地址
 *	    u8 *yuv:存储yuv422图片信息的buf的首地址
 *	    u32 w:图片的宽度，单位像素
 *	    u32 h:图片的高度，单位像素
 */
int yuvtorgb24(u8 *rgb_buf, u8*yuv, u32 w, u32 h);

/*
 *函数名：getbmp
 *函数功能：RGB24转BMP
 *函数返回值：int 成功返回0，失败返回-1
 *函数参数：u8 *bmpaddr:用于存储bmp图片的buf的首地址
 *	    u32 w:图片的宽度，单位像素
 *	    u32 h:图片的高度，单位像素
 */
int getbmp(u8 *bmpaddr, u32 w, u32 h);

/*
 *函数名：init_conv
 *函数功能：初始化及分配转换需要的buf空间
 *函数返回值：void
 *函数参数：u32 w:图片的宽度，单位像素
 *	    u32 h:图片的高度，单位像素
 */
void init_conv(u32 w, u32 h);

/*
 *函数名：uninit_conv
 *函数功能：释放初始化分配的内存空间
 *函数返回值：void
 *函数参数：void
 */
void uninit_conv();

/*
 *函数名：rgbtojpeg
 *函数功能：将rgb图片压缩成jpeg并存储在buff中
 *函数返回值：unsigned long：返回jpeg图片的大小
 *函数参数：u8 **dest_jpg
 *	    u8 *src_rgb
 *	    u32 w
 *	    u32 h
 */
unsigned long rgbtojpeg(u8 **dest_jpg, u8 *src_rgb, u32 w, u32 h);
#endif
