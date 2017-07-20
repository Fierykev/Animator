#include "c2interpevaluator.h"
#include <cassert>
#include "mat.h"
#include "vec.h"

void C2InterpCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts,
	std::vector<Point>& ptvEvaluatedCurvePts,
	const float& fAniLength,
	const bool& bWrap) const
{
	int iCtrlPtCount = ptvCtrlPts.size();
	float len = ptvCtrlPts[0].x + fAniLength - ptvCtrlPts[ptvCtrlPts.size() - 1].x;
	std::vector<Point> vPoints(ptvCtrlPts);

	if (bWrap)
	{
		// start
		vPoints.insert(
			vPoints.begin(),
			Point(
				ptvCtrlPts[0].x - len,
				ptvCtrlPts[iCtrlPtCount - 1].y
			)
		);

		// end

		vPoints.insert(
			vPoints.end(),
			Point(
				ptvCtrlPts[0].x + fAniLength,
				ptvCtrlPts[0].y
			)
		);

		vPoints.insert(
			vPoints.end(),
			Point(
				ptvCtrlPts[0].x + 1 + fAniLength,
				ptvCtrlPts[0].y
			)
		);

		vPoints.insert(
			vPoints.end(),
			Point(
				ptvCtrlPts[0].x + 2 + fAniLength,
				ptvCtrlPts[0].y
			)
		);
	}
	else
	{
		vPoints.insert(
			vPoints.begin(),
			Point(
				-4,
				ptvCtrlPts[0].y
			)
		);

		vPoints.insert(
			vPoints.end(),
			Point(
				fAniLength + 4.f,
				ptvCtrlPts[iCtrlPtCount - 1].y
			)
		);

		vPoints.insert(
			vPoints.end(),
			Point(
				fAniLength + 5.f,
				ptvCtrlPts[iCtrlPtCount - 1].y
			)
		);

		vPoints.insert(
			vPoints.end(),
			Point(
				fAniLength + 6.f,
				ptvCtrlPts[iCtrlPtCount - 1].y
			)
		);
	}

	const static Mat4<float> bezMat = {
		-1.f, 3.f, -3.f, 1.f,
		3.f, -6.f, 3.f, 0.f,
		-3.f, 3.f, 0.f, 0.f,
		1.f, 0.f, 0.f, 0.f
	};

	const static Mat4<float> interpMat = {
		26.f, -7.f, 2.f, -1.f,
		-7.f, 14.f, -4.f, 2.f,
		2.f, -4.f, 14.f, -7.f,
		-1.f, 2.f, -7.f, 26.f
	};

	Vec4<float> input[2];
	Vec4<float> coords[2];

	ptvEvaluatedCurvePts.clear();

	for (int i = 0; i < vPoints.size() - 3; i ++)
	{
		input[0] = {
			vPoints[i + 1].x - vPoints[i].x,
			vPoints[i + 2].x - vPoints[i].x,
			vPoints[i + 3].x - vPoints[i + 1].x,
			vPoints[i + 3].x - vPoints[i + 2].x
		};

		input[1] = {
			vPoints[i + 1].y - vPoints[i].y,
			vPoints[i + 2].y - vPoints[i].y,
			vPoints[i + 3].y - vPoints[i + 1].y,
			vPoints[i + 3].y - vPoints[i + 2].y
		};

		input[0] = interpMat * 3.f / 45.f * input[0];
		input[1] = interpMat * 3.f / 45.f * input[1];

		input[0] = Vec4f(
			vPoints[i].x,
			vPoints[i].x + 1.f / 3.f * input[0][0],
			vPoints[i + 1].x - 1.f / 3.f * input[0][1],
			vPoints[i + 1].x
		);

		input[1] = Vec4f(
			vPoints[i].y,
			vPoints[i].y + 1.f / 3.f * input[1][0],
			vPoints[i + 1].y - 1.f / 3.f * input[1][1],
			vPoints[i + 1].y
		);

		coords[0] = bezMat * input[0];
		coords[1] = bezMat * input[1];

		for (int j = 0; j <= CurveEvaluator::s_iSegCount; j++)
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
