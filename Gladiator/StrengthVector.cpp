//
// Created by xapulc on 13.11.2020.
//

#include "StrengthVector.h"

void StrengthVector::checkLength(const int len) {
    if (len <= 0) {
        std::cout << "Wrong length: " << len << std::endl;
        exit(-1);
    }
}

void StrengthVector::createElems(const int len, bool fillZeros) {
    if ((this->elems = std::vector<double>(len)).empty()) {
        std::cout << "Cannot allocate enough memory. Length: " << len << std::endl;
        exit(-1);
    }

    if (fillZeros) {
        for(int i = 0; i < len; i++)
            (this->elems)[i] = 0;
    }
}

StrengthVector::StrengthVector(int len) {
    checkLength(len);
    this->d = len;
    createElems(len);
}

double &StrengthVector::operator[](const int i) {
    return (this->elems)[i];
}

double StrengthVector::operator[](const int i) const {
    return (this->elems)[i];
}

StrengthVector &StrengthVector::operator=(const StrengthVector &other) {
    this->d = other.d;
    createElems(d, false);
    for(int i = 0; i < d; i++)
        (this->elems)[i] = other.elems[i];
    return *this;
}

void StrengthVector::checkLengths(const StrengthVector& left, const StrengthVector& right) {
    if (left.d != right.d) {
        std::cout << "Wrong lengths: " << left.d << "and " << right.d << ")" << std::endl;
        exit(-1);
    }
}

StrengthVector& StrengthVector::operator+=(const StrengthVector &other) {
    checkLengths(*this, other);
    for(int i = 0; i < this->d; i++)
        elems[i] += other.elems[i];
    return *this;
}

StrengthVector StrengthVector::operator+(const StrengthVector &other) const {
    auto res = *this;
    res += other;
    return res;
}

StrengthVector& StrengthVector::operator-=(const StrengthVector &other) {
    checkLengths(*this, other);
    for(int i = 0; i < this->d; i++)
        elems[i] -= other.elems[i];
    return *this;
}

StrengthVector StrengthVector::operator-(const StrengthVector &other) const {
    auto res = *this;
    res -= other;
    return res;
}

StrengthVector& StrengthVector::operator*=(const StrengthVector &other) {
    checkLengths(*this, other);
    for(int i = 0; i < this->d; i++)
        elems[i] *= other.elems[i];
    return *this;
}

StrengthVector StrengthVector::operator*(const StrengthVector &other) const {
    auto res = *this;
    res *= other;
    return res;
}

StrengthVector& StrengthVector::operator/=(const StrengthVector &other) {
    checkLengths(*this, other);
    for(int i = 0; i < this->d; i++)
        elems[i] /= other.elems[i];
    return *this;
}

StrengthVector StrengthVector::operator/(const StrengthVector &other) const {
    auto res = *this;
    res /= other;
    return res;
}

StrengthVector& StrengthVector::operator*=(const double a) {
    for(int i = 0; i < this->d; i++)
        elems[i] *= a;
    return *this;
}

StrengthVector StrengthVector::operator*(const double a) const {
    auto res = *this;
    res *= a;
    return res;
}

StrengthVector& StrengthVector::operator/=(const double a) {
    for(int i = 0; i < this->d; i++)
        elems[i] /= a;
    return *this;
}

StrengthVector StrengthVector::operator/(const double a) const {
    auto res = *this;
    res /= a;
    return res;
}

void StrengthVector::print() const {
    for(int i = 0; i < this->d; i++) {
        std::cout << (*this)[i] << " ";
    }
    std::cout << std::endl;
}

int StrengthVector::getLength() const {
    return this->d;
}
