#ifndef __C2_INTERP_EVALUATOR__
#define __C2_INTERP_EVALUATOR__


#pragma warning(disable : 4786)  

#include "curveevaluator.h"

//using namespace std;

class C2InterpCurveEvaluator : public CurveEvaluator
{
public:
	void evaluateCurve(const std::vector<Point>& ptvCtrlPts,
		std::vector<Point>& ptvEvaluatedCurvePts,
		const float& fAniLength,
		const bool& bWrap) const;
};

#endif