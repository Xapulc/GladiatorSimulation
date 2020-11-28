//
// Created by xapulc on 14.11.2020.
//

#ifndef GLADIATORSIMULATION_MULTIVECTOR_H
#define GLADIATORSIMULATION_MULTIVECTOR_H


#include <iostream>
#include <vector>

class MultiVector {
public:
    MultiVector() = default;
    MultiVector(std::vector<int> dim);

    double &operator[](std::vector<int> i);
    double operator[](std::vector<int> i) const;
    int getOneDimensionalIndex(std::vector<int> i) const;
private:
    std::vector<int> dim{0};
    int prodDim{1};
    std::vector<double> elems;
    static void checkLength(std::vector<int> len);
    void createElems(bool fillZeros = true);
};


#endif //GLADIATORSIMULATION_MULTIVECTOR_H
