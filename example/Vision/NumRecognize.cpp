#include "../../../LiGu_AlgorithmLib/NeuralNetworks.h"
#include "../../../LiGu_Graphics/src/GraphicsFileCode.h"

int main() {
	GraphicsFileCode::ppmRead ("C:/Users/ÐÜè÷ºÀ/Desktop/76543.ppm");
	GraphicsFileCode::ppmWrite("C:/Users/ÐÜè÷ºÀ/Desktop/76543.ppm");








	LeNet_NeuralNetworks nn;
	nn.load("parameter.txt");
	Tensor<float> input(28, 28, 1);
	Mat<float> output;

	char inputUrl[100];
	int ans;

	while (true) {
		input.getData(inputUrl);
		nn.forward(input, output);
		output.max(ans);
		printf("%d\n", ans);
	}
}