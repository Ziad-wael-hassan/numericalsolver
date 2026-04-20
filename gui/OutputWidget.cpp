#include "OutputWidget.h"

#include <cmath>

#include <QCheckBox>
#include <QContextMenuEvent>
#include <QFileDialog>
#include <QHeaderView>
#include <QMenu>
#include <QMessageBox>
#include <QTableWidget>
#include <QTextEdit>
#include <QVBoxLayout>

#include "../utils/CsvExporter.h"
#include "../core/solvers/SolverCommon.h"

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

QString matrixToInlineString(const std::vector<std::vector<double>> &matrix) {
    QStringList rows;
    for (const auto &row : matrix) {
        QStringList values;
        for (double value : row) {
            values << QString::fromStdString(formatNumber(value));
        }
        rows << values.join(", ");
    }
    return rows.join(" | ");
}

} // namespace

OutputWidget::OutputWidget(QWidget *parent)
    : QWidget(parent)
    , resultText_(new QTextEdit(this))
    , iterationTable_(new QTableWidget(this))
    , showIterationsCheck_(new QCheckBox("Show iterations", this)) {

    resultText_->setReadOnly(true);
    resultText_->setMaximumHeight(150);

    iterationTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    iterationTable_->setSelectionBehavior(QAbstractItemView::SelectItems);
    iterationTable_->setSelectionMode(QAbstractItemView::ExtendedSelection);
    iterationTable_->setAlternatingRowColors(true);
    iterationTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    iterationTable_->setContextMenuPolicy(Qt::CustomContextMenu);

    showIterationsCheck_->setChecked(true);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(resultText_);
    layout->addWidget(showIterationsCheck_);
    layout->addWidget(iterationTable_);

    connect(iterationTable_, &QTableWidget::customContextMenuRequested,
            this, &OutputWidget::showContextMenu);

    connect(showIterationsCheck_, &QCheckBox::toggled,
            iterationTable_, &QTableWidget::setVisible);
}

void OutputWidget::displayResult(const SolverResult &result) {
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

    resultText_->setPlainText(lines.join('\n'));

    if (result.headers.empty() && result.rows.empty()) {
        iterationTable_->setVisible(false);
        showIterationsCheck_->setVisible(false);
        return;
    }

    iterationTable_->setVisible(showIterationsCheck_->isChecked());
    showIterationsCheck_->setVisible(true);

    QStringList headers;
    for (const auto &header : result.headers) {
        headers << QString::fromStdString(header);
    }
    if (headers.isEmpty()) {
        headers << "Field" << "Value";
    }

    int rowCount = static_cast<int>(result.rows.size() + result.matrixSteps.size());
    iterationTable_->clear();
    iterationTable_->setColumnCount(headers.size());
    iterationTable_->setHorizontalHeaderLabels(headers);
    iterationTable_->setRowCount(rowCount);

    int rowIndex = 0;
    for (const auto &row : result.rows) {
        for (int column = 0; column < static_cast<int>(row.size()) && column < headers.size(); ++column) {
            iterationTable_->setItem(rowIndex, column,
                                  new QTableWidgetItem(QString::fromStdString(row[column])));
        }
        ++rowIndex;
    }

    if (!result.matrixSteps.empty()) {
        if (headers.size() < 2) {
            iterationTable_->setColumnCount(2);
            iterationTable_->setHorizontalHeaderLabels({"Step", "Matrix"});
        }
        for (const auto &step : result.matrixSteps) {
            iterationTable_->setItem(rowIndex, 0,
                                  new QTableWidgetItem(QString::fromStdString(step.label)));
            iterationTable_->setItem(rowIndex, 1,
                                  new QTableWidgetItem(matrixToInlineString(step.matrix)));
            ++rowIndex;
        }
    }
}

void OutputWidget::showContextMenu(const QPoint &position) {
    QMenu menu(this);
    QAction *exportAction = menu.addAction("Export CSV");
    QAction *selected = menu.exec(iterationTable_->viewport()->mapToGlobal(position));
    if (selected != exportAction) {
        return;
    }

    const QString path = QFileDialog::getSaveFileName(
        this,
        "Export Iterations",
        "iterations.csv",
        "CSV Files (*.csv)");
    if (path.isEmpty()) {
        return;
    }

    QStringList headers;
    for (int column = 0; column < iterationTable_->columnCount(); ++column) {
        auto *item = iterationTable_->horizontalHeaderItem(column);
        headers << (item != nullptr ? item->text() : QString("Column %1").arg(column + 1));
    }

    QList<QStringList> rows;
    for (int row = 0; row < iterationTable_->rowCount(); ++row) {
        QStringList rowValues;
        for (int column = 0; column < iterationTable_->columnCount(); ++column) {
            auto *item = iterationTable_->item(row, column);
            rowValues << (item != nullptr ? item->text() : QString());
        }
        rows.push_back(rowValues);
    }

    QString errorMessage;
    if (!CsvExporter::exportTable(path, headers, rows, &errorMessage)) {
        QMessageBox::critical(this, "Export Failed", errorMessage);
    }
}

} // namespace numerical