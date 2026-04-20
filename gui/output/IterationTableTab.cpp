#include "IterationTableTab.h"

#include <QFileDialog>
#include <QHeaderView>
#include <QMenu>
#include <QMessageBox>
#include <QTableWidget>
#include <QVBoxLayout>

#include "../../utils/CsvExporter.h"
#include "../../core/solvers/SolverCommon.h"

namespace numerical {

namespace {

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

IterationTableTab::IterationTableTab(QWidget *parent)
    : QWidget(parent)
    , tableWidget_(new QTableWidget(this)) {
    tableWidget_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget_->setSelectionBehavior(QAbstractItemView::SelectItems);
    tableWidget_->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tableWidget_->setAlternatingRowColors(true);
    tableWidget_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget_->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(tableWidget_, &QTableWidget::customContextMenuRequested,
            this, &IterationTableTab::showContextMenu);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(tableWidget_);
}

void IterationTableTab::displayResult(const SolverResult &result) {
    QStringList headers;
    for (const auto &header : result.headers) {
        headers << QString::fromStdString(header);
    }
    if (headers.isEmpty()) {
        headers << "Field" << "Value";
    }

    int rowCount = static_cast<int>(result.rows.size() + result.matrixSteps.size());
    tableWidget_->clear();
    tableWidget_->setColumnCount(headers.size());
    tableWidget_->setHorizontalHeaderLabels(headers);
    tableWidget_->setRowCount(rowCount);

    int rowIndex = 0;
    for (const auto &row : result.rows) {
        for (int column = 0; column < static_cast<int>(row.size()) && column < headers.size(); ++column) {
            tableWidget_->setItem(rowIndex, column,
                                  new QTableWidgetItem(QString::fromStdString(row[column])));
        }
        ++rowIndex;
    }

    if (!result.matrixSteps.empty()) {
        if (headers.size() < 2) {
            tableWidget_->setColumnCount(2);
            tableWidget_->setHorizontalHeaderLabels({"Step", "Matrix"});
        }
        for (const auto &step : result.matrixSteps) {
            tableWidget_->setItem(rowIndex, 0,
                                  new QTableWidgetItem(QString::fromStdString(step.label)));
            tableWidget_->setItem(rowIndex, 1,
                                  new QTableWidgetItem(matrixToInlineString(step.matrix)));
            ++rowIndex;
        }
    }
}

void IterationTableTab::showContextMenu(const QPoint &position) {
    QMenu menu(this);
    QAction *exportAction = menu.addAction("Export CSV");
    QAction *selected = menu.exec(tableWidget_->viewport()->mapToGlobal(position));
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
    for (int column = 0; column < tableWidget_->columnCount(); ++column) {
        auto *item = tableWidget_->horizontalHeaderItem(column);
        headers << (item != nullptr ? item->text() : QString("Column %1").arg(column + 1));
    }

    QList<QStringList> rows;
    for (int row = 0; row < tableWidget_->rowCount(); ++row) {
        QStringList rowValues;
        for (int column = 0; column < tableWidget_->columnCount(); ++column) {
            auto *item = tableWidget_->item(row, column);
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
