#include "GraphTab.h"

#include <QLabel>
#include <QVBoxLayout>

#ifdef NUMERICAL_SOLVER_HAS_QTCHARTS
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLegend>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QValueAxis>
#endif

namespace numerical {

GraphTab::GraphTab(QWidget *parent)
    : QWidget(parent)
#ifdef NUMERICAL_SOLVER_HAS_QTCHARTS
    , chart_(new QtCharts::QChart())
    , chartView_(new QtCharts::QChartView(chart_, this)) {
    chartView_->setRenderHint(QPainter::Antialiasing);
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(chartView_);
#else
    , placeholderLabel_(new QLabel(this)) {
    placeholderLabel_->setAlignment(Qt::AlignCenter);
    placeholderLabel_->setWordWrap(true);
    placeholderLabel_->setText("Qt Charts is not available in this build.\nInstall the Qt Charts module to enable plotting.");
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(placeholderLabel_);
#endif
}

void GraphTab::displayResult(const SolverResult &result) {
#ifdef NUMERICAL_SOLVER_HAS_QTCHARTS
    chart_->removeAllSeries();
    chart_->setTitle(QString::fromStdString(result.methodName));

    if (!result.functionSeries.empty()) {
        auto *series = new QtCharts::QLineSeries(chart_);
        series->setName("Function");
        for (const auto &point : result.functionSeries) {
            series->append(point.x, point.y);
        }
        chart_->addSeries(series);
    }

    if (!result.convergenceSeries.empty()) {
        auto *series = new QtCharts::QLineSeries(chart_);
        series->setName("Convergence");
        for (const auto &point : result.convergenceSeries) {
            series->append(point.x, point.y);
        }
        chart_->addSeries(series);
    }

    if (!result.markers.empty()) {
        auto *markers = new QtCharts::QScatterSeries(chart_);
        markers->setMarkerSize(12.0);
        markers->setName("Marker");
        for (const auto &point : result.markers) {
            markers->append(point.x, point.y);
        }
        chart_->addSeries(markers);
    }

    chart_->createDefaultAxes();
    chart_->legend()->setVisible(true);
#else
    placeholderLabel_->setText(
        QString("Qt Charts is not available in this build.\nMethod: %1\nFunction samples: %2\nConvergence points: %3")
            .arg(QString::fromStdString(result.methodName))
            .arg(result.functionSeries.size())
            .arg(result.convergenceSeries.size()));
#endif
}

} // namespace numerical
