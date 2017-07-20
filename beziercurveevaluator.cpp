#include "beziercurveevaluator.h"
#include <cassert>
#include "mat.h"
#include "vec.h"

void eval(Vec4<float> coords[2], std::vector<Point>& ptvEvaluatedCurvePts)
{
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

void BezierCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts,
	std::vector<Point>& ptvEvaluatedCurvePts,
	const float& fAniLength,
	const bool& bWrap) const
{
	int iCtrlPtCount = ptvCtrlPts.size();
	float len = ptvCtrlPts[0].x + fAniLength - ptvCtrlPts[ptvCtrlPts.size() - 1].x;
	bool mod3 = iCtrlPtCount % 3 == 0, mod3_2 = iCtrlPtCount % 3 == 2;

	std::vector<Point> vPoints(ptvCtrlPts);
	ptvEvaluatedCurvePts.clear();

	// start
	
	if (bWrap && !mod3)
	{
		vPoints.insert(
			vPoints.begin(),
			Point(
				ptvCtrlPts[0].x,
				ptvCtrlPts[0].y
			)
		);

		vPoints.insert(
			vPoints.begin(),
			2,
			Point(
				ptvCtrlPts[0].x - len,
				ptvCtrlPts[iCtrlPtCount - 1].y
			)
		);
	}
	else if (!bWrap || !mod3)
	{
		vPoints.insert(
			vPoints.begin(),
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
	}

	if (bWrap && mod3)
	{
		vPoints.insert(
			vPoints.end(),
			2,
			Point(
				ptvCtrlPts[iCtrlPtCount - 1].x + len,
				ptvCtrlPts[0].y
			)
		);
	}

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

	const static Mat4<float> bezMat = {
		-1.f, 3.f, -3.f, 1.f,
		3.f, -6.f, 3.f, 0.f,
		-3.f, 3.f, 0.f, 0.f,
		1.f, 0.f, 0.f, 0.f
	};

	Vec4<float> input[2];
	Vec4<float> coords[2];

	if (bWrap && mod3)
	{
		input[0] = {
			ptvCtrlPts[iCtrlPtCount - 3].x - fAniLength,
			ptvCtrlPts[iCtrlPtCount - 2].x - fAniLength,
			ptvCtrlPts[iCtrlPtCount - 1].x - fAniLength,
			ptvCtrlPts[0].x
		};

		input[1] = {
			ptvCtrlPts[iCtrlPtCount - 3].y,
			ptvCtrlPts[iCtrlPtCount - 2].y,
			ptvCtrlPts[iCtrlPtCount - 1].y,
			ptvCtrlPts[0].y
		};

		coords[0] = bezMat * input[0];
		coords[1] = bezMat * input[1];

		eval(coords, ptvEvaluatedCurvePts);
	}

	for (int i = 0; i < vPoints.size() - 7; i += 3)
	{
		input[0] = {
			vPoints[i].x,
			vPoints[i + 1].x,
			vPoints[i + 2].x,
			vPoints[i + 3].x
		};

		input[1] = {
			vPoints[i].y,
			vPoints[i + 1].y,
			vPoints[i + 2].y,
			vPoints[i + 3].y
		};

		coords[0] = bezMat * input[0];
		coords[1] = bezMat * input[1];

		eval(coords, ptvEvaluatedCurvePts);
	}

	if (!bWrap && mod3)
	{
		ptvEvaluatedCurvePts.push_back(ptvCtrlPts[iCtrlPtCount - 2]);
		ptvEvaluatedCurvePts.push_back(ptvCtrlPts[iCtrlPtCount - 1]);
		ptvEvaluatedCurvePts.push_back(
			Point(
				fAniLength + 4,
				ptvEvaluatedCurvePts[ptvEvaluatedCurvePts.size() - 1].y
				)
		);
	}

	if (!mod3)
	{
		if (mod3_2)
			ptvEvaluatedCurvePts.push_back(ptvCtrlPts[iCtrlPtCount - 1]);

		if (!bWrap)
			ptvEvaluatedCurvePts.push_back(
				Point(
					fAniLength + 4,
					ptvEvaluatedCurvePts[ptvEvaluatedCurvePts.size() - 1].y
				)
			);
		else
			ptvEvaluatedCurvePts.push_back(
				Point(
					ptvCtrlPts[iCtrlPtCount - 1].x + len,
					ptvCtrlPts[0].y
				)
			);
	}
}
