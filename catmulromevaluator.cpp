#include "catmulromevaluator.h"
#include <cassert>
#include "mat.h"
#include "vec.h"

void CatRomCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts,
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
		vPoints.insert(
			vPoints.begin(),
			2,
			Point(
				-2.f,
				ptvCtrlPts[0].y
			)
		);

		vPoints.insert(
			vPoints.end(),
			2,
			Point(
				2 + fAniLength,
				ptvCtrlPts[iCtrlPtCount - 1].y
			)
		);
	}

	const static Mat4<float> catMat = {
		-1.f, 3.f, -3.f, 1.f,
		2.f, -5.f, 4.f, -1.f,
		-1.f, 0.f, 1.f, 0.f,
		0.f, 2.f, 0.f, 0.f
	};

	ptvEvaluatedCurvePts.clear();

	for (int i = 1; i < vPoints.size() - 2; i++)
	{/*
		Vec3f distances;

		for (int j = 0; j < 3; i++)
			distances[i] = vPoints[i - 1].distance(vPoints[i]);
		
		if (distances[0] + distances[1] + distances[2] < s_fFlatnessEpsilon)
		{

		}*/

		Vec4<float> input[2] = {
			{ vPoints[i - 1].x, vPoints[i].x, vPoints[i + 1].x, vPoints[i + 2].x },
			{ vPoints[i - 1].y, vPoints[i].y, vPoints[i + 1].y, vPoints[i + 2].y }
		};

		Vec4<float> coords[2] = {
			catMat / 2.f * input[0],
			catMat / 2.f * input[1]
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
