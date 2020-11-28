//
// Created by xapulc on 14.11.2020.
//

#include "QualityEstimation.h"

double QualityEstimation::probabilityOfWinLeftTeam(const StrengthVector &leftTeam, const StrengthVector &rightTeam) {
    /**
     * Вероятность выживания фиксированного количества гладиаторов у каждой команды.
     *
     * Рассматриваются 2 команды гладиаторов с силами `leftTeam` и `rightTeam`.
     * Гладиаторы в этих командах сражаются последовательно до смерти.
     * То есть первыми встречаются a_1 и b_1, затем победитель
     * сражения встречается со вторым гладиатором команды противника и так далее.
     *
     * В возвращаемом методом векторе перечислены сначала
     * вероятности выжить `k` гладиаторам первой команды,
     * где 1 <= `k` <= `m` (количество гладиаторов в первой команде),
     * а заметм перечислены вероятности выжить `l` гладиаторам второй команды,
     * где 1 <= 'l' <= 'n' (количество гладиаторов во второй команде).
     *
     * Вероятность того, что выживет ровно `m-k` гладиаторов у первой команды можно посчитать следующим образом.
     * Положим A_k := \sum_{j=1}^k a_j X_j, B_l := \sum_{i=1}^l b_i Y_i.
     * Согласно работе Каминского вероятность того,
     * что останется в живых `m-k` гладиаторов у первой команды,
     * равна P(A_k < B_n < A_{k+1}) = P(B_n > A_k) - P(B_n > A_{k+1}).
     * Вероятности вида
     *     P(A_k > B_l) = p_{k,l}, где k = m или l = n, (1)
     * находятся итерационным алгоритмом, используя соотношение
     * (b_i / (a_j + b_i)) * p_{j-1,i} + (a_j / (a_j + b_i)) * p_{j,i-1} = p_{j,i}. (2)
     *
     * Объём памяти можно сэкономить на том, что на самом деле нужно хранить
     * два вектора: вектор `winLeftWithFullTeam` := (p_{m,1}, ..., p_{m,n})
     * и вектор `curWinLeft` := (p_{1,i}, p_{2,i}, ..., p_{j-1,i}, p_{j,i-1}, p_{j+1,i-1}, ..., p_{m,i-1}).
     * Первый будет нужен для нахождения вероятностей вида (1) при k = m,
     * второй по правилу (2) и начальным состоянием p_{j,0} = 1, p_{0,i} = 0
     * позволяет вычислить все элементы вида p_{j,n}.
     */
    auto m = leftTeam.getLength();
    auto n = rightTeam.getLength();
    std::vector<double> curWinLeft(m);
    std::fill(curWinLeft.begin(), curWinLeft.end(), 1.0);

    for (int i = 0; i < n; i++) {
        curWinLeft[0] = leftTeam[0] * curWinLeft[0] / (leftTeam[0] + rightTeam[i]);
        for (int j = 1; j < m; j++) {
            curWinLeft[j] = (rightTeam[i] * curWinLeft[j-1] + leftTeam[j] * curWinLeft[j])
                            / (leftTeam[j] + rightTeam[i]);
        }
    }

    return curWinLeft[m-1];
}

std::vector<double> QualityEstimation::probabilitiesOfWin(const std::vector<StrengthVector> &teams) {
    /*
     * Вектор вероятностей побед каждой команды.
     *
     * Рассматривается вектор команд `teams`.
     * Гладиаторы в этих командах сражаются последовательно до смерти.
     *
     * Функция возвращает вектор вероятностей побед каждой команды,
     * где j элемент вектора равен p_j(a_1, ..., a_n), a_j := (a_j^1, ..., a_j^{m_j}).
     *
     * Пусть i := (i_1, ..., i_n), a_j^{(k)} := (a_j^1, ..., a_j^{(k)}).
     * Заметим, что справедливо соотношение
     *     p_j(a_1^{(k_1)}, ..., a_n^{(k_n)}) =
     *         = \sum_{l=1}^n p_j(a_1^{(k_1)}, ..., a_l^{(k_l-1)}, ..., a_n^{(k_n)}) *
     *                        * (1/a_l^{k_l}) / (1/a_1^{k_1} + ... + 1/a_n^{k_n}).
     *
     * Таким образом, можно рекуррентно вычислять вероятность победы.
     * Эту рекурренту можно развернуть в цикл, итерируясь последовательно по k_1, ..., k_n.
     */
    auto dimensionalProbabilityMatrix = std::vector<int>(teams.size());
    int dimensionalSum = 0;
    for (int j = 0; j < teams.size(); j++) {
        dimensionalProbabilityMatrix[j] = teams[j].getLength() + 1;
        dimensionalSum += teams[j].getLength();
    }
    auto multiIndexGeneric = MultiIndexGeneric(dimensionalProbabilityMatrix);

    dimensionalProbabilityMatrix.push_back(teams.size());
    auto probabilityMatrix = MultiVector(dimensionalProbabilityMatrix);
    auto isZeroK = true;

    while (true) {
        auto k = multiIndexGeneric.getIndex();
        for (int j = 0; j < teams.size(); j++) {
            auto teamIndex = k;
            teamIndex.push_back(j);

            probabilityMatrix[teamIndex] = 0;
            double denominator = 0;

            for (int l = 0; l < teams.size(); l++) {
                if (k[l] != 0) {
                    auto loserGladiatorStrength = teams[l][k[l]-1];
                    auto loserTeam = k;
                    loserTeam.push_back(j);
                    loserTeam[l] -= 1;

                    denominator += 1 / loserGladiatorStrength;
                    probabilityMatrix[teamIndex] += probabilityMatrix[loserTeam] / loserGladiatorStrength;
                }
            }

            if (isZeroK) {
                probabilityMatrix[teamIndex] = 1;
            } else if (k[j] == 0) {
                probabilityMatrix[teamIndex] = 0;
            } else {
                probabilityMatrix[teamIndex] /= denominator;
            }
        }

        if (isZeroK) {
            isZeroK = false;
        }

        if (!multiIndexGeneric.next()) {
            auto result = std::vector<double>(teams.size());
            for (int j = 0; j < teams.size(); j++) {
                k[j] = dimensionalProbabilityMatrix[j]-1;
            }

            for (int j = 0; j < teams.size(); j++) {
                auto teamIndex = k;
                teamIndex.push_back(j);
                result[j] = probabilityMatrix[teamIndex];
            }
            return result;
        }
    }
}
