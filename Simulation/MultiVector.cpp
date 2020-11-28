//
// Created by xapulc on 14.11.2020.
//

#include "MultiVector.h"

void MultiVector::checkLength(const std::vector<int> dim) {
    for (auto el : dim) {
        if (el <= 0) {
            std::cout << "Wrong dimensional: " << el << std::endl;
            exit(-1);
        }
    }
}

void MultiVector::createElems(bool fillZeros) {
    if ((this->elems = std::vector<double>(prodDim)).empty()) {
        std::cout << "Cannot allocate enough memory. Length: " << prodDim << std::endl;
        exit(-1);
    }

    if (fillZeros) {
        for(int i = 0; i < prodDim; i++)
            (this->elems)[i] = 0;
    }
}

MultiVector::MultiVector(const std::vector<int> dim) {
    checkLength(dim);
    this->dim = dim;
    this->prodDim = 1;
    for (auto el : dim) {
        this->prodDim *= el;
    }
    createElems();
}

double &MultiVector::operator[](const std::vector<int> i) {
    return (this->elems)[getOneDimensionalIndex(i)];
}

double MultiVector::operator[](const std::vector<int> i) const {
    return (this->elems)[getOneDimensionalIndex(i)];
}

int MultiVector::getOneDimensionalIndex(const std::vector<int> i) const {
    int oneDimensionalIndex = 0;
    int multiplyIndex = prodDim;

    for (int j = 0; j < dim.size(); j++) {
        multiplyIndex /= dim[j];
        oneDimensionalIndex += multiplyIndex * i[j];
    }
    return oneDimensionalIndex;
}
