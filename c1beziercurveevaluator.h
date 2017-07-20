#ifndef __C1_BEZIER_CURVE_EVALUATOR_H__
#define __C1_BEZIER_CURVE_EVALUATOR_H__

#pragma warning(disable : 4786)  

#include "curveevaluator.h"

//using namespace std;

class C1BezCurveEvaluator : public CurveEvaluator
{
public:
	void evaluateCurve(const std::vector<Point>& ptvCtrlPts,
		std::vector<Point>& ptvEvaluatedCurvePts,
		const float& fAniLength,
		const bool& bWrap) const;
};


#endif