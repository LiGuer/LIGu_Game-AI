#include "LiGu_AlgorithmLib/NeuralNetworks.h"
#include <vector>
#include <stdio.h>
/*--------------------------------[ MINST ]--------------------------------
*	http://yann.lecun.com/exdb/mnist/
**------------------------------------------------------------------------*/
class Minst {
public:
	NeuralNetworks nn;
	int trainNum, testNum, height, width;
	float* imageTrain,*imageTest;
	unsigned char* lableTrain, * lableTest;
	char* netSaveFile;
	/*----------------[ init ]----------------*/
	void init(
		const char* trainFileImage_Url, const char* trainFileLable_Url,
		const char* testFileImage_Url, const char* testFileLable_Url,
		const char* saveFile_Url
	) {
		getTrainDataset(trainFileImage_Url, trainFileLable_Url);
		getTestDataset(testFileImage_Url, testFileLable_Url);
		netSaveFile = (char*)saveFile_Url;
		initNN();
	}
	void initNN() {
		nn.addLayer(28 * 28, 512);
		nn.addLayer(512, 512);
		nn.addLayer(512, 10);
	}
	/*----------------[ dataset ]----------------*/
	void getTrainDataset(const char* trainFileImage_Url, const char* trainFileLable_Url) {
		// Image
		FILE* fi = fopen(trainFileImage_Url, "rb");
		int t;
		for (int i = 3; i >= 0; i--)fread((char*)&t + i, sizeof(char), 1, fi);			//大小端问题, 待优化
		for (int i = 3; i >= 0; i--)fread((char*)&trainNum + i, sizeof(char), 1, fi);
		for (int i = 3; i >= 0; i--)fread((char*)&height + i, sizeof(char), 1, fi);
		for (int i = 3; i >= 0; i--)fread((char*)&width + i, sizeof(char), 1, fi);
		imageTrain = (float*)calloc(trainNum * height * width, sizeof(float));
		for (int i = 0; i < trainNum * height * width; i++) {
			unsigned char pix;
			fread(&pix, sizeof(char), 1, fi);
			imageTrain[i] = (float)pix / 255;
		}
		fclose(fi);
		// Lable
		fi = fopen(trainFileLable_Url, "rb");
		for (int i = 3; i >= 0; i--)fread((char*)&t + i, sizeof(char), 1, fi);
		for (int i = 3; i >= 0; i--)fread((char*)&trainNum + i, sizeof(char), 1, fi);
		lableTrain = (unsigned char*)calloc(trainNum, sizeof(unsigned char));
		for (int i = 0; i < trainNum; i++)
			fread(&lableTrain[i], sizeof(char), 1, fi);
		fclose(fi);
	}
	void getTestDataset(const char* testFileImage_Url, const char* testFileLable_Url) {
		// Image
		FILE* fi = fopen(testFileImage_Url, "rb");
		int t;
		for (int i = 3; i >= 0; i--)fread((char*)&t + i, sizeof(char), 1, fi);			//大小端问题, 待优化
		for (int i = 3; i >= 0; i--)fread((char*)&testNum + i, sizeof(char), 1, fi);
		for (int i = 3; i >= 0; i--)fread((char*)&height + i, sizeof(char), 1, fi);
		for (int i = 3; i >= 0; i--)fread((char*)&width + i, sizeof(char), 1, fi);
		imageTest = (float*)calloc(testNum * height * width, sizeof(float));
		for (int i = 0; i < testNum * height * width; i++) {
			unsigned char pix;
			fread(&pix, sizeof(char), 1, fi);
			imageTest[i] = (float)pix / 255;
		}
		fclose(fi);
		// Lable
		fi = fopen(testFileLable_Url, "rb");
		for (int i = 3; i >= 0; i--)fread((char*)&t + i, sizeof(char), 1, fi);
		for (int i = 3; i >= 0; i--)fread((char*)&testNum + i, sizeof(char), 1, fi);
		lableTest = (unsigned char*)calloc(testNum, sizeof(unsigned char));
		for (int i = 0; i < testNum; i++)
			fread(&lableTest[i], sizeof(char), 1, fi);
		fclose(fi);
	}
	/*----------------[ train ]----------------*/
	void train() {
		Mat<float> input(height * width, 1), output;
		for (int i = 0; i < trainNum; i++) {
			for (int j = 0; j < height * width; j++)
				input[j] = imageTrain[height * width * i + j];
			if (i % 1000 == 0) { nn.save(netSaveFile); printf("Train Epoch:%d/60  Error Rate:%f\n", i / 1000, test());  }
			nn.forward(input, output);
			Mat<float> target(10, 1);
			target[lableTrain[i]] = 1;
			nn.backward(target);
		}
	}
	/*----------------[ test ]----------------*/
	float test() {
		Mat<float> input(height * width, 1), output;
		int errorNum = 0;
		for (int i = 0; i < testNum; i++) {
			for (int j = 0; j < height * width; j++)
				input[j] = imageTest[height * width * i + j];
			nn.forward(input, output);
			int ans; output.max(ans);
			if (ans != lableTest[i])errorNum++;
		}
		return (float)errorNum / testNum;
	}
};

int main() {
	Minst minst;
	minst.init(
		"C:/Users/熊梓豪/Desktop/symbol/train-images.idx3-ubyte",
		"C:/Users/熊梓豪/Desktop/symbol/train-labels.idx1-ubyte",
		"C:/Users/熊梓豪/Desktop/symbol/t10k-images.idx3-ubyte",
		"C:/Users/熊梓豪/Desktop/symbol/t10k-labels.idx1-ubyte",
		"D:/MinstNet.txt"
	);
	printf("Start Train\n");
	int trainEpoch = 100;
	for (int i = 0; i < trainEpoch; i++) {
		minst.train();
		printf("Train Epoch:%d  Error Rate:%f\n", i, minst.test());
	}
	return 0;
}