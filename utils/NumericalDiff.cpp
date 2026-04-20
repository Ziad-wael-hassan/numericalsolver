#include "NumericalDiff.h"

namespace numerical {

double NumericalDiff::derivative(const std::function<double(double)> &function,
                                 double x,
                                 double h) {
    return (function(x + h) - function(x - h)) / (2.0 * h);
}

} // namespace numerical
