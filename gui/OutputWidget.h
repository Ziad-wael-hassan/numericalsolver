#pragma once

#include <QWidget>

#include "../core/SolverResult.h"

class QCheckBox;
class QTableWidget;
class QTextEdit;

namespace numerical {

class OutputWidget final : public QWidget {
    Q_OBJECT

public:
    explicit OutputWidget(QWidget *parent = nullptr);

    void displayResult(const SolverResult &result);

private:
    void showContextMenu(const QPoint &position);

    QTextEdit *resultText_ {nullptr};
    QTableWidget *iterationTable_ {nullptr};
    QCheckBox *showIterationsCheck_ {nullptr};
};

} // namespace numerical