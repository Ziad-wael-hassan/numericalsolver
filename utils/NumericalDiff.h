#pragma once

#include <functional>

namespace numerical {

class NumericalDiff {
public:
    static double derivative(const std::function<double(double)> &function,
                             double x,
                             double h = 1e-7);
};

} // namespace numerical
