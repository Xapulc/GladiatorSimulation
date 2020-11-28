//
// Created by xapulc on 14.11.2020.
//

#ifndef GLADIATORSIMULATION_QUALITYESTIMATION_H
#define GLADIATORSIMULATION_QUALITYESTIMATION_H


#include "../Gladiator/StrengthVector.h"
#include "MultiVector.h"
#include "MultiIndexGeneric.h"


class QualityEstimation {
public:
    static double probabilityOfWinLeftTeam(const StrengthVector& leftTeam, const StrengthVector& rightTeam);
    static std::vector<double> probabilitiesOfWin(const std::vector<StrengthVector>& teams);
};


#endif //GLADIATORSIMULATION_QUALITYESTIMATION_H
