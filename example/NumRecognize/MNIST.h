#ifndef MNIST_H
#define MNIST_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/******************************************************************************
*                    MNIST
*	[Code]:
	*	TRAINING SET LABEL FILE (train-labels-idx1-ubyte):
		[offset] [type]          [value]          [description]
		0000     32 bit integer  0x00000801(2049) magic number (MSB first)
		0004     32 bit integer  60000            number of items
		0008     unsigned byte   ??               label
		xxxx     unsigned byte   ??               label
		The labels values are 0 to 9.
	*	TRAINING SET IMAGE FILE (train-images-idx3-ubyte):
		[offset] [type]          [value]          [description]
		0000     32 bit integer  0x00000803(2051) magic number
		0004     32 bit integer  60000            number of images
		0008     32 bit integer  28               number of rows
		0012     32 bit integer  28               number of cols
		0016     unsigned byte   ??               pixel
		xxxx     unsigned byte   ??               pixel
		Pixels are organized row-wise. Pixel values are 0 to 255. 0 means background (white), 255 means foreground (black).
	*	TEST SET LABEL FILE (t10k-labels-idx1-ubyte)
	*	TEST SET IMAGE FILE (t10k-images-idx3-ubyte)
*	[Referance]: http://yann.lecun.com/exdb/mnist/
******************************************************************************/
class MNIST {
public:
	int testNum, trainNum, rows, cols;
	unsigned char
		* trainImgI, * trainLable,
		*  testImgI, *  testLable;
	double
		* trainImg, * testImg;
	MNIST(const char* filesPath) {
		int magic;
		char* filePath = (char*)malloc((strlen(filesPath) + 50) * 1);
		strcpy(filePath, filesPath);
		{//Train Lables
			strcpy(filePath + strlen(filesPath), "train-labels-idx1-ubyte");
			FILE* fi = fopen(filePath, "rb");
			for (int i = 3; i >= 0; i--) fread((char*)&magic	+ i, 1, 1, fi); if (magic != 0x00000801) exit(-1);
			for (int i = 3; i >= 0; i--) fread((char*)&trainNum	+ i, 1, 1, fi);	//大小端问题, 待优化
			trainLable = (unsigned char*)malloc(trainNum * sizeof(unsigned char));
			for (int i = 0; i < trainNum; i++)
				fread(trainLable + i, sizeof(unsigned char), 1, fi);
			fclose(fi);
		}
		{//Test  Lables
			strcpy(filePath + strlen(filesPath), "t10k-labels-idx1-ubyte");
			FILE* fi = fopen(filePath, "rb");
			for (int i = 3; i >= 0; i--) fread((char*)&magic	+ i, 1, 1, fi); if (magic != 0x00000801) exit(-1);
			for (int i = 3; i >= 0; i--) fread((char*)&testNum	+ i, 1, 1, fi);	//大小端问题, 待优化
			testLable  = (unsigned char*)malloc(testNum  * sizeof(unsigned char));
			for (int i = 0; i < trainNum; i++)
				fread( testLable + i, sizeof(unsigned char), 1, fi);
			fclose(fi);
		}
		{//Train Images
			strcpy(filePath + strlen(filesPath), "train-images-idx3-ubyte");
			FILE* fi = fopen(filePath, "rb");
			for (int i = 3; i >= 0; i--) fread((char*)&magic	+ i, 1, 1, fi); if (magic != 0x00000803) exit(-1);
			for (int i = 3; i >= 0; i--) fread((char*)&trainNum	+ i, 1, 1, fi);	//大小端问题, 待优化
			for (int i = 3; i >= 0; i--) fread((char*)&rows		+ i, 1, 1, fi);
			for (int i = 3; i >= 0; i--) fread((char*)&cols		+ i, 1, 1, fi);
			trainImgI = (unsigned char*)malloc(trainNum * rows * cols * sizeof(unsigned char));
			for (int i = 0; i < trainNum * rows * cols; i++)
				fread(trainImgI + i, sizeof(unsigned char), 1, fi);
			fclose(fi);
		}
		{//Test  Images
			strcpy(filePath + strlen(filesPath), "t10k-images-idx3-ubyte");
			FILE* fi = fopen(filePath, "rb");
			for (int i = 3; i >= 0; i--) fread((char*)&magic	+ i, 1, 1, fi); if (magic != 0x00000803) exit(-1);
			for (int i = 3; i >= 0; i--) fread((char*)&testNum	+ i, 1, 1, fi);	//大小端问题, 待优化
			for (int i = 3; i >= 0; i--) fread((char*)&rows		+ i, 1, 1, fi);
			for (int i = 3; i >= 0; i--) fread((char*)&cols		+ i, 1, 1, fi);
			testImgI  = (unsigned char*)malloc(testNum  * rows * cols * sizeof(unsigned char));
			for (int i = 0; i < testNum  * rows * cols; i++)
				fread(testImgI  + i, sizeof(unsigned char), 1, fi);
			fclose(fi);
		}
		free(filePath);
		{//to Double
			trainImg = (double*)malloc(trainNum * rows * cols * sizeof(double));
			 testImg = (double*)malloc( testNum * rows * cols * sizeof(double));
			for (int i = 0; i < trainNum * rows * cols; i++) trainImg[i] = trainImgI[i] / 255.0;
			for (int i = 0; i <  testNum * rows * cols; i++)  testImg[i] =  testImgI[i] / 255.0;
		}
	}
};
#endif