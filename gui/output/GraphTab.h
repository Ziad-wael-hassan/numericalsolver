#pragma once

#include <QWidget>

#include "../../core/SolverResult.h"

class QLabel;

#ifdef NUMERICAL_SOLVER_HAS_QTCHARTS
QT_BEGIN_NAMESPACE
namespace QtCharts {
class QChartView;
class QChart;
}
QT_END_NAMESPACE
#endif

namespace numerical {

class GraphTab final : public QWidget {
    Q_OBJECT

public:
    explicit GraphTab(QWidget *parent = nullptr);

    void displayResult(const SolverResult &result);

private:
#ifdef NUMERICAL_SOLVER_HAS_QTCHARTS
    QtCharts::QChart *chart_ {nullptr};
    QtCharts::QChartView *chartView_ {nullptr};
#else
    QLabel *placeholderLabel_ {nullptr};
#endif
};

} // namespace numerical
