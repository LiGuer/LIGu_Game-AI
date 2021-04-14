#include "LiGu_AlgorithmLib/Mat.h"
/*--------------------------------[3D_Reconstruction]--------------------------------
*	定义 PointSet[0th] 为原点. PointSet1 为 (X-Y)， 通过PointSet2 求Z
*	PointSet2 相对 PointSet1 以Y轴旋转α
*	Z = X·tan(α1)		α1 + α2 = α
	S / cos(α2) = X / cosα1 = S / cos(α-α1)
	S / X = (cosαcosα1 + sinαsinα1) / cosα1 = cosα + sinαtanα1
	Z = (S - Xcosα) / sinα
**----------------------------------------------------------------------------------*/
int Reconstruct_3D(Mat<double> PointSet1, Mat<double> PointSet2, double alpha, Mat<double>& PointSet_3D) {
	PointSet_3D(PointSet1.rows, 3);
	double K = (PointSet1(1, 0) - PointSet1(0, 0)) / (PointSet2(1, 1) - PointSet2(0, 1));
	for (int i = 1; i < PointSet1.rows; i++) {
		double X = PointSet1(i, 0) - PointSet1(0, 0), Y = PointSet1(i, 1) - PointSet1(0, 1);
		double S = (PointSet2(i, 1) - PointSet2(0, 1)) * K;
		double Z = (S - X*cos(alpha)) / sin(alpha);
		PointSet_3D(i, 0) = X; PointSet_3D(i, 1) = Y; PointSet_3D(i, 2) = Z;
	}
}
