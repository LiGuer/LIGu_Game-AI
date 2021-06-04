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
		char* filePath = (char*)malloc((strlen(filesPath) + 50) * sizeof(char));
		strcpy(filePath, filesPath);
		{//Train Lables
			strcpy(filePath + strlen(filesPath), "train-labels-idx1-ubyte");
			FILE* fi = fopen(filePath, "rb");
			fread(&magic,    sizeof(int), 1, fi), reverseInt(magic); if (magic != 0x00000801) exit(-1);
			fread(&trainNum, sizeof(int), 1, fi), reverseInt(trainNum);
			trainLable = (unsigned char*)malloc(trainNum * sizeof(unsigned char));
			for (int i = 0; i < trainNum; i++)
				fread(trainLable + i, sizeof(unsigned char), 1, fi);
		}
		{//Test  Lables
			strcpy(filePath + strlen(filesPath), "t10k-labels-idx1-ubyte");
			FILE* fi = fopen(filePath, "rb");
			fread(&magic,    sizeof(int), 1, fi), reverseInt(magic); if (magic != 0x00000801) exit(-1);
			fread(&testNum,  sizeof(int), 1, fi), reverseInt(testNum);
			testLable  = (unsigned char*)malloc(testNum  * sizeof(unsigned char));
			for (int i = 0; i < trainNum; i++)
				fread( testLable + i, sizeof(unsigned char), 1, fi);
		}
		{//Train Images
			strcpy(filePath + strlen(filesPath), "train-images-idx3-ubyte");
			FILE* fi = fopen(filePath, "rb");
			fread(&magic,    sizeof(int), 1, fi), reverseInt(magic); if (magic != 0x00000803) exit(-1);
			fread(&trainNum, sizeof(int), 1, fi), reverseInt(trainNum);
			fread(&rows,     sizeof(int), 1, fi), reverseInt(rows);
			fread(&cols,     sizeof(int), 1, fi), reverseInt(cols);
			trainImgI = (unsigned char*)malloc(trainNum * rows * cols * sizeof(unsigned char));
			for (int i = 0; i < trainNum * rows * cols; i++)
				fread(trainImgI + i, sizeof(unsigned char), 1, fi);
		}
		{//Test  Images
			strcpy(filePath + strlen(filesPath), "t10k-images-idx3-ubyte");
			FILE* fi = fopen(filePath, "rb");
			fread(&magic,    sizeof(int), 1, fi), reverseInt(magic); if (magic != 0x00000803) exit(-1);
			fread(&testNum,  sizeof(int), 1, fi), reverseInt(testNum);
			fread(&rows,     sizeof(int), 1, fi), reverseInt(rows);
			fread(&cols,     sizeof(int), 1, fi), reverseInt(cols);
			testImgI  = (unsigned char*)malloc(testNum  * rows * cols * sizeof(unsigned char));
			for (int i = 0; i < testNum  * rows * cols; i++)
				fread(testImgI  + i, sizeof(unsigned char), 1, fi);
		}
		free(filePath);
		{//to Double
			trainImg = (double*)malloc(trainNum * rows * cols * sizeof(double));
			 testImg = (double*)malloc( testNum * rows * cols * sizeof(double));
			for (int i = 0; i < trainNum * rows * cols; i++) trainImg[i] = trainImgI[i] / 255.0;
			for (int i = 0; i <  testNum * rows * cols; i++)  testImg[i] =  testImgI[i] / 255.0;
		}
	}
	int reverseInt(int& i) {
		unsigned char c1, c2, c3, c4;
		c1 =  i        & 255;
		c2 = (i >> 8)  & 255;
		c3 = (i >> 16) & 255;
		c4 = (i >> 24) & 255;
		return i = ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
	}
};
#endif