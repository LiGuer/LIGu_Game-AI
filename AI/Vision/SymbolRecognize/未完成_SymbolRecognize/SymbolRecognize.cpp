#include"LiGu_AlgorithmLib/NeuralNetworks.h"
#include <stdio.h>

class NeuralNetworks {
public:
	ConvLayer Conv_1{ 1,16,3,1,1 }, Conv_2{ 16,32,3,1,1 };
	PoolLayer MaxPool_1{ 2,0,2,MaxPool_1.M }, MaxPool_2{ 2,0,2,MaxPool_2.M };
	NeuralLayer FullConnect_1{ 32 * 7 * 7,128 }, FullConnect_2{ 128,128 }, FullConnect_3{ 128,44 };
	NeuralNetworks() {
		FullConnect_1.ActivFuncType = FullConnect_2.ActivFuncType = FullConnect_3.ActivFuncType = 0;
	}
	/*----------------[ forward ]----------------*/
	void forward(Tensor<float>& input, Mat<float>& output) {
		Tensor<float>* y;
		y = Conv_1(input);
		y = MaxPool_1(*y);
		y = Conv_2(*y);
		y = MaxPool_2(*y);
		Tensor<float> t = *y;
		Mat<float> t2(t.dim.product(), 1); t2.data = t.data; t.data = NULL;
		Mat<float>* maty = &t2;
		maty = FullConnect_1(*maty);
		maty = FullConnect_2(*maty);
		maty = FullConnect_3(*maty);
		output = *maty;
	}
	/*----------------[ backward ]----------------*/
	/*----------------[ save/load ]----------------*/
	void save(const char* saveFile) {
		FILE* file = fopen(saveFile, "w+");
		Conv_1.save(file);
		Conv_2.save(file);
		FullConnect_1.save(file);
		FullConnect_2.save(file);
		FullConnect_3.save(file);
		fclose(file);
	}
	void load(const char* loadFile) {
		FILE* file = fopen(loadFile, "r+");
		Conv_1.load(file);
		Conv_2.load(file);
		FullConnect_1.load(file);
		FullConnect_2.load(file);
		FullConnect_3.load(file);
		fclose(file);
	}
};
/*
A B C D E F G H I J K L M N O P Q R S T U V W X Y Z a b d e f g h n r t 2 3 4 5 6 7 8 9
*/
int main() {
	NeuralNetworks nn;
	nn.load("SymbolRecognize_parameter.txt");
	Tensor<float> input(28, 28, 1);
	Mat<float> output;

	char symbolList[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabdefghnrt23456789";
	char inputUrl[100];
	int ans;

	while (true) {
		scanf("%s", &inputUrl);
		FILE* fi = fopen(inputUrl, "rb");
		for (int i = 0; i < 28 * 28; i++) {
			int pix; fscanf(fi, "%d", &pix);
			input[i] = (float)pix / 255;
		}
		nn.forward(input, output);
		output.max(ans);
		printf("%c\n", symbolList[ans]);
	}
}