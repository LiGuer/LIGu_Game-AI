#include"LiGu_AlgorithmLib/NeuralNetworks.h"
#include <stdio.h>

int main() {
	LeNet_NeuralNetworks nn;
	nn.load("parameter.txt");
	Tensor<float> input(28, 28, 1);
	Mat<float> output;

	char inputUrl[100];
	int ans;

	while (true) {
		scanf("%s", &inputUrl);
		FILE* fi = fopen(inputUrl, "rb");
		for (int i = 0; i < 28 * 28; i++) {
			int pix;
			fscanf(fi, "%d", &pix);
			input[i] = (float)pix / 255;
		}
		nn.forward(input, output);
		output.max(ans);
		printf("%d\n", ans);
	}
}