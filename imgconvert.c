#include <stdio.h>
#include "include/mytypes.h"
#include <string.h>
#include <stdlib.h>
#include "include/jpeglib.h"

/*
 *YUV422转RGB公式
 *y0 cb0 y1 cr0 ---> r0 g0 b0 r1 g1 b1
 *r0 = YCbCrtoR(y0, cb0, cr0)
 *g0 = YCbCrtoG(y0, cb0, cr0)
 *b0 = YCbCrtoB(y0, cb0, cr0)
 *r1 = YCbCrtoR(y1, cb0, cr0)
 *g1 = YCbCrtoG(y1, cb0, cr0)
 *b1 = YCbCrtoB(y1, cb0, cr0)
 *YUYV转RGB公式
 */

#define YCbCrtoR(Y,Cb,Cr) (1000000 * Y + 1370705 * (Cr-128)) / 1000000
#define YCbCrtoG(Y,Cb,Cr) (1000000 * Y - 337633 * (Cb-128) - 698001 * (Cr-128)) / 1000000
#define YCbCrtoB(Y,Cb,Cr) (1000000 * Y + 1732446 * (Cb-128)) / 1000000

/*rgb24缓存 len:w*h*3*/
typedef struct __rgb24data
{
	int len;	/*rgb24缓存长度*/
	u8* buf;	/*rgb24缓存首地址*/
}RGB24;

/*BMP文件头*/
typedef struct tagBMPFILEHEADER
{
	u16 align;
	u16 bfType;		//BMP文件类型，值为"BM"
	u32 bfSize;		//BMP文件大小，单位是字节
	u32 bfReserved;		//0
	u32 bfOffBits;		//必须是54
}BMPFILEHEADER;

/*BMP信息头*/
typedef struct tagBMPINFOHEADER
{
	u32 biSize;		//必须是 0x28
	u32 biWidth;
	u32 biHeight;
	u16 biPlanes;
	u16 biBitCount;
	u32 biCompression;
	u32 biSizeImage;
	u32 biXPelsPerMeter;
	u32 biYPelsPerMeter;
	u32 biClrUsed;
	u32 biClrImportant;
}BMPINFOHEADER;

#define WIDTHBYTES(i) ((i+31)/32*4)

/*bmp缓存 len:bf+bi+rgb24*/
typedef struct __bmpdata
{
	int len;	/*54+rgb24.len*/
	u8* buf;	/*bmp缓存首地址*/
}BMP;

static RGB24 rgb24;
static BMP   bmp24;

int yuvtorgb24(u8 *rgb_buf, u8 *yuv, u32 w, u32 h)
{
	u8 y0,cb0,y1,cr0;
	int r,g,b;
	
	//printf("yuvtorgb24: w=%d,h=%d\r\n", w, h);
	int len = w*h*2;
	u8 *prgb = rgb24.buf;
		
	len /= 4;
	while(len--)
	{
		y0  = *yuv++;
		cb0 = *yuv++;
		y1  = *yuv++;
		cr0 = *yuv++;

		r = YCbCrtoR(y0, cb0, cr0);
		g = YCbCrtoG(y0, cb0, cr0);
		b = YCbCrtoB(y0, cb0, cr0);

		if(r > 255) r = 255;
		if(r < 0)   r = 0; 
		if(g > 255) g = 255;
		if(g < 0)   g = 0; 
		if(b > 255) b = 255;
		if(b < 0)   b = 0; 
		
	#if 0
		*prgb++ = ((b * 220 / 256) & 0xFF);
		*prgb++ = ((g * 220 / 256) & 0xFF);
		*prgb++ = ((r * 220 / 256) & 0xFF);
	#else
		*prgb++ = b;
		*prgb++ = g;
		*prgb++ = r;
	#endif

		r = YCbCrtoR(y1, cb0, cr0);
		g = YCbCrtoG(y1, cb0, cr0);
		b = YCbCrtoB(y1, cb0, cr0);
		
		if(r > 255) r = 255;
		if(r < 0)   r = 0; 
		if(g > 255) g = 255;
		if(g < 0)   g = 0; 
		if(b > 255) b = 255;
		if(b < 0)   b = 0; 

	#if 0
		*prgb++ = ((b * 220 / 256) & 0xFF);
		*prgb++ = ((g * 220 / 256) & 0xFF);
		*prgb++ = ((r * 220 / 256) & 0xFF);
	#else
		*prgb++ = b;
		*prgb++ = g;
		*prgb++ = r;
	#endif
	}

	int i = 0;
	unsigned char temp[3] = {0};
	for(i = 0; i < 320*120; i++)
	{
		temp[0] = rgb24.buf[3*i];
		temp[1] = rgb24.buf[3*i + 1];
		temp[2] = rgb24.buf[3*i + 2];

		rgb24.buf[3*i] = rgb24.buf[(320*240-i-1)*3];
		rgb24.buf[3*i + 1] = rgb24.buf[(320*240-i-1)*3 + 1];
		rgb24.buf[3*i + 2] = rgb24.buf[(320*240-i-1)*3 + 2];
		
		rgb24.buf[(320*240-i-1)*3] = temp[0];
		rgb24.buf[(320*240-i-1)*3 + 1] = temp[1];
		rgb24.buf[(320*240-i-1)*3 + 2] = temp[2];
	}
	memcpy(rgb_buf, rgb24.buf, rgb24.len);
	return 0;
}

int getbmp(u8 *bmpaddr, u32 w, u32 h)
{
	/*rgb24-->bmp24*/
	//printf("rgbtobmp: w=%d,h=%d\r\n",w,h);
	int len = w*h*3;
	
	BMPINFOHEADER bi;
	BMPFILEHEADER bf;

	/*bmp文件头和信息头的初始化*/
	bi.biSize	= 0x28;		/*40*/
	bi.biWidth	= w;
	bi.biHeight	= h;
	bi.biPlanes	= 1;
	bi.biBitCount	= 24;
	bi.biCompression= 0;
	bi.biSizeImage	= len;		/*WIDTHBYTES(bi.biWidth * bi.biBitCount) * bi.biHeight*/
	bi.biXPelsPerMeter	= 0;
	bi.biYPelsPerMeter	= 0;
	bi.biClrUsed		= 0;
	bi.biClrImportant	= 0;

	bf.bfType	= 0x4D42;	/* 'B''M' */
	bf.bfSize	= 54 + bi.biSizeImage;
	bf.bfReserved	= 0;
	bf.bfOffBits	= 54;		/*偏移*/

	u8 *pbmp = bmp24.buf;
	memcpy(pbmp, &bf.bfType, 14);
	pbmp += 14;
	memcpy(pbmp, &bi, 40);
	pbmp += 40;

	u8 *prgb24 = rgb24.buf;
#if 1
	/*直接拷贝*/
	memcpy(pbmp, prgb24, len);
#endif

#if 0
	/*行翻转拷贝*/
	int line = w*3;
	prgb24 += ((h-2) * line);
	int i = 0;
	for(i = 0; i < h; i++)
	{
		memcpy(pbmp, prgb24, line);
		pbmp += line;
		prgb24 -= line;
	}
#endif
	memcpy(bmpaddr,bmp24.buf,54+w*h*3);
}

unsigned long rgbtojpeg(u8 **dest_jpg, u8 *src_rgb, u32 w, u32 h)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	
	unsigned long size = 0;
	jpeg_mem_dest(&cinfo, dest_jpg, &size);

	cinfo.image_width  = w;
	cinfo.image_height = h;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, jpeg_quality_scaling(50), TRUE);
	jpeg_start_compress(&cinfo, TRUE);

	unsigned long row_wide = w * 3;
	
	/*将图像R与B分量互换*/
	u8 *buffer = (u8 *)malloc(w*h*3);
	int i = 0;
	for(i = 0; i < w*h; i++)
	{
		buffer[i*3]	= src_rgb[i*3+2];
		buffer[i*3 + 1]	= src_rgb[i*3+1];
		buffer[i*3 + 2] = src_rgb[i*3]; 
	}

	JSAMPROW row_pointer[1];
	while(cinfo.next_scanline < cinfo.image_height)
	{
		row_pointer[0] = &buffer[(cinfo.image_height - cinfo.next_scanline - 1) * row_wide];
		jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	free(buffer);
	jpeg_destroy_compress(&cinfo);

	return size;
}

void init_conv(u32 w, u32 h)
{
	rgb24.len = w*h*3;
	bmp24.len = sizeof(BMPFILEHEADER) + sizeof(BMPINFOHEADER) + rgb24.len;

	rgb24.buf = (u8 *)malloc(rgb24.len);
	if(NULL == rgb24.buf)
	{
		puts("init_conv rgb error.");
		exit(-1);
	}

	bmp24.buf = (u8 *)malloc(bmp24.len);
	if(NULL == bmp24.buf)
	{
		puts("init_conv bmp error.");
		exit(-1);
	}
}

void uninit_conv()
{
	if(rgb24.buf)
	{
		free(rgb24.buf);
	}

	if(bmp24.buf)
	{
		free(bmp24.buf);
	}
	rgb24.len = 0;
	bmp24.len = 0;

	rgb24.buf = NULL;
	bmp24.buf = NULL;
}
