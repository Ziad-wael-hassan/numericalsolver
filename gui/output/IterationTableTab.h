#pragma once

#include <QWidget>

#include "../../core/SolverResult.h"

class QTableWidget;

namespace numerical {

class IterationTableTab final : public QWidget {
    Q_OBJECT

public:
    explicit IterationTableTab(QWidget *parent = nullptr);

    void displayResult(const SolverResult &result);

private:
    void showContextMenu(const QPoint &position);

    QTableWidget *tableWidget_ {nullptr};
};

} // namespace numerical
