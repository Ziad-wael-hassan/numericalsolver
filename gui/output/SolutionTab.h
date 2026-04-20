#pragma once

#include <QWidget>

#include "../../core/SolverResult.h"

class QTextEdit;

namespace numerical {

class SolutionTab final : public QWidget {
    Q_OBJECT

public:
    explicit SolutionTab(QWidget *parent = nullptr);

    void displayResult(const SolverResult &result);

private:
    QTextEdit *textEdit_ {nullptr};
};

} // namespace numerical
