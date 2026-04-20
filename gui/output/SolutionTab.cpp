#include "SolutionTab.h"

#include <cmath>

#include <QTextEdit>
#include <QVBoxLayout>

#include "../../core/solvers/SolverCommon.h"

namespace numerical {

namespace {

QString matrixToString(const std::vector<std::vector<double>> &matrix) {
    QString text;
    for (const auto &row : matrix) {
        QStringList line;
        for (double value : row) {
            line << QString::fromStdString(formatNumber(value));
        }
        text += line.join('\t') + '\n';
    }
    return text.trimmed();
}

} // namespace

SolutionTab::SolutionTab(QWidget *parent)
    : QWidget(parent)
    , textEdit_(new QTextEdit(this)) {
    textEdit_->setReadOnly(true);
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(textEdit_);
}

void SolutionTab::displayResult(const SolverResult &result) {
    QStringList lines;
    lines << QString("Method: %1").arg(QString::fromStdString(result.methodName));
    lines << QString("Status: %1").arg(result.converged ? "Converged" : "Not converged");

    if (!std::isnan(result.scalarResult)) {
        lines << QString("Scalar result: %1")
                     .arg(QString::fromStdString(formatNumber(result.scalarResult)));
    }
    if (!std::isnan(result.optimumValue)) {
        lines << QString("Objective value: %1")
                     .arg(QString::fromStdString(formatNumber(result.optimumValue)));
    }
    if (!result.vectorResult.empty()) {
        lines << QString("Vector result: %1")
                     .arg(QString::fromStdString(formatVector(result.vectorResult)));
    }
    if (result.iterations > 0) {
        lines << QString("Iterations: %1").arg(result.iterations);
    }
    if (!std::isnan(result.achievedTolerance)) {
        lines << QString("Achieved tolerance: %1")
                     .arg(QString::fromStdString(formatNumber(result.achievedTolerance)));
    }
    if (!result.message.empty()) {
        lines << QString("Message: %1").arg(QString::fromStdString(result.message));
    }

    for (const auto &step : result.matrixSteps) {
        lines << "";
        lines << QString::fromStdString(step.label);
        lines << matrixToString(step.matrix);
    }

    textEdit_->setPlainText(lines.join('\n'));
}

} // namespace numerical
