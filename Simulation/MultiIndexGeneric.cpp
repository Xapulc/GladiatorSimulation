//
// Created by xapulc on 15.11.2020.
//

#include "MultiIndexGeneric.h"

void MultiIndexGeneric::checkLength(const std::vector<int> dim) {
    for (auto el : dim) {
        if (el <= 0) {
            std::cout << "Wrong dimensional: " << el << std::endl;
            exit(-1);
        }
    }
}

void MultiIndexGeneric::createElems(bool fillZeros) {
    if ((this->elems = std::vector<int>(dim.size())).empty()) {
        std::cout << "Cannot allocate enough memory. Length: " << dim.size() << std::endl;
        exit(-1);
    }

    if (fillZeros) {
        for(int i = 0; i < dim.size(); i++)
            (this->elems)[i] = 0;
    }
}

MultiIndexGeneric::MultiIndexGeneric(const std::vector<int> dim) {
    checkLength(dim);
    this->dim = dim;
    createElems();
}

bool MultiIndexGeneric::next() {
    int firstNotMax = 0;
    for (; (firstNotMax < dim.size()) && ((this->elems)[firstNotMax] == dim[firstNotMax] - 1); firstNotMax++) {
        (this->elems)[firstNotMax] = 0;
    }

    if (firstNotMax != dim.size()) {
        (this->elems)[firstNotMax] += 1;
        return true;
    } else {
        return false;
    }
}

std::vector<int>& MultiIndexGeneric::getIndex() {
    return (this->elems);
}
