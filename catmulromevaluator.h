#ifndef __CATMUL_ROM_EVALUATOR_H__
#define __CATMUL_ROM_EVALUATOR_H__

#pragma warning(disable : 4786)  

#include "curveevaluator.h"

//using namespace std;

class CatRomCurveEvaluator : public CurveEvaluator
{
public:
	void evaluateCurve(const std::vector<Point>& ptvCtrlPts,
		std::vector<Point>& ptvEvaluatedCurvePts,
		const float& fAniLength,
		const bool& bWrap) const;
};

#endif