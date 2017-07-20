#include "bsplinecurveevaluator.h"
#include <cassert>
#include "mat.h"
#include "vec.h"

void BSplineCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts,
	std::vector<Point>& ptvEvaluatedCurvePts,
	const float& fAniLength,
	const bool& bWrap) const
{
	int iCtrlPtCount = ptvCtrlPts.size();

	std::vector<Point> vPoints(ptvCtrlPts);

	if (bWrap)
	{
		// start

		vPoints.insert(
			vPoints.begin(),
			Point(
				ptvCtrlPts[iCtrlPtCount - 1].x - fAniLength,
				ptvCtrlPts[iCtrlPtCount - 1].y
			)
		);

		vPoints.insert(
			vPoints.begin(),
			Point(
				ptvCtrlPts[iCtrlPtCount - 2].x - fAniLength,
				ptvCtrlPts[iCtrlPtCount - 2].y
			)
		);

		// end

		vPoints.push_back(
			Point(
				ptvCtrlPts[0].x + fAniLength,
				ptvCtrlPts[0].y
			)
		);

		vPoints.push_back(
			Point(
				ptvCtrlPts[1].x + fAniLength,
				ptvCtrlPts[1].y
			)
		);
	}
	else
	{
		// start

		vPoints.insert(
			vPoints.begin(),
			2,
			Point(
				ptvCtrlPts[0].x,
				ptvCtrlPts[0].y
			)
		);

		vPoints.insert(
			vPoints.begin(),
			2,
			Point(
				-4,
				ptvCtrlPts[0].y
			)
		);

		// end

		vPoints.insert(
			vPoints.end(),
			2,
			Point(
				ptvCtrlPts[iCtrlPtCount - 1].x,
				ptvCtrlPts[iCtrlPtCount - 1].y
			)
		);

		vPoints.insert(
			vPoints.end(),
			2,
			Point(
				fAniLength + 4,
				ptvCtrlPts[iCtrlPtCount - 1].y
			)
		);
	}

	ptvEvaluatedCurvePts.clear();

	// from the slides: https://www.cs.utexas.edu/users/fussell/courses/cs384g/lectures/Lecture11-Parametric_curves.pdf
	const static Mat4<float> bSplineMat = {
		-1.f, 3.f, -3.f, 1.f,
		3.f, -6.f, 3.f, 0.f,
		-3.f, 0.f, 3.f, 0.f,
		1.f, 4.f, 1.f, 0.f
	};

	for (size_t i = 0; i < vPoints.size() - 3; i++)
	{
		Vec4<float> input[2] = {
			{ vPoints[i].x, vPoints[i + 1].x, vPoints[i + 2].x, vPoints[i + 3].x },
			{ vPoints[i].y, vPoints[i + 1].y, vPoints[i + 2].y, vPoints[i + 3].y }
		};

		Vec4<float> coords[2] = {
			bSplineMat / 6.f * input[0],
			bSplineMat / 6.f * input[1]
		};

		for (int j = 0; j < CurveEvaluator::s_iSegCount; j++)
		{
			float step = (float)j / (float)CurveEvaluator::s_iSegCount;
			float stepSq = step * step;
			float stepCube = stepSq * step;

			ptvEvaluatedCurvePts.push_back(
				Point(
					coords[0][3] + coords[0][2] * step + coords[0][1] * stepSq + coords[0][0] * stepCube,
					coords[1][3] + coords[1][2] * step + coords[1][1] * stepSq + coords[1][0] * stepCube
				)
			);
		}
	}
}