//
// Created by xapulc on 15.11.2020.
//

#ifndef GLADIATORSIMULATION_MULTIINDEXGENERIC_H
#define GLADIATORSIMULATION_MULTIINDEXGENERIC_H


#include <iostream>
#include <vector>

class MultiIndexGeneric {
public:
    MultiIndexGeneric() = default;
    MultiIndexGeneric(std::vector<int> dim);

    bool next();
    std::vector<int>& getIndex();
private:
    std::vector<int> dim{0};
    std::vector<int> elems{0};
    static void checkLength(std::vector<int> len);
    void createElems(bool fillZeros = true);
};


#endif //GLADIATORSIMULATION_MULTIINDEXGENERIC_H
