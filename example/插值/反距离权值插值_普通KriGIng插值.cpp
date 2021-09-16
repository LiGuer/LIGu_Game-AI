#include "../LiGu_Codes/LiGu_AlgorithmLib/Mat.h"
#include "../LiGu_Codes/LiGu_AI/src/BasicMachineLearning.h"
#include "../LiGu_Codes/LiGu_AI/src/ReinforcementLearning.h"
#include "../LiGu_Codes/LiGu_Graphics/src/GraphicsND.h"
using namespace BasicMachineLearning;
int main() {
	Mat<> data(3, 100);
	data.rands(-1, 1);
	for (int i = 0; i < 100; i++) {
		data(2, i) = sin((pow(data(0, i), 2) + pow(data(1, i), 2)) * 3);
	}
	int n = 100;
	Mat<> ans1(n, n);
	Mat<> ans2(n, n);
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			ans1(i, j) = InvDisWeight(data, -1 + (2.0 / n) * i, -1 + (2.0 / n) * j);
			ans2(i, j) = KrigingOrdinary(data, -1 + (2.0 / n) * i, -1 + (2.0 / n) * j,
				[](double x1, double y1, double x2, double y2) {return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2)); }
			);
		}
	}
	
	GraphicsND G(n, n);
	G.contour(ans1); G.g.writeImg("D:/Ligu1.ppm");
	G.contour(ans2); G.g.writeImg("D:/Ligu2.ppm");
}