//
// Created by xapulc on 13.11.2020.
//

#ifndef GLADIATORSIMULATION_STRENGTHVECTOR_H
#define GLADIATORSIMULATION_STRENGTHVECTOR_H

#include <vector>
#include <iostream>


class StrengthVector {
public:
    StrengthVector() = default;
    StrengthVector(int len);

    double &operator[](int i);
    double operator[](int i) const;
    StrengthVector& operator=(const StrengthVector& other);
    StrengthVector& operator+=(const StrengthVector& other);
    StrengthVector operator+(const StrengthVector& other) const;
    StrengthVector& operator-=(const StrengthVector& other);
    StrengthVector operator-(const StrengthVector& other) const;
    StrengthVector& operator*=(const StrengthVector& other);
    StrengthVector operator*(const StrengthVector& other) const;
    StrengthVector& operator/=(const StrengthVector& other);
    StrengthVector operator/(const StrengthVector& other) const;
    StrengthVector& operator*=(double a);
    StrengthVector operator*(double a) const;
    StrengthVector& operator/=(double a);
    StrengthVector operator/(double a) const;
    void print() const;

    int getLength() const;
private:
    int d{0};
    std::vector<double> elems;
    static void checkLength(int len);
    static void checkLengths(const StrengthVector& left, const StrengthVector& right);
    void createElems(int len, bool fillZeros = true);
};


#endif //GLADIATORSIMULATION_STRENGTHVECTOR_H
