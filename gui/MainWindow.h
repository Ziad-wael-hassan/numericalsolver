#pragma once

#include <QFutureWatcher>
#include <QMainWindow>

#include "../core/SolverResult.h"

class QLabel;
class QPushButton;

namespace numerical {

class InputWidget;
class MethodSelector;
class OutputWidget;

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void handleMethodSelected(MethodId methodId);
    void solveCurrentMethod();
    void onSolveFinished();
    void clearCurrentInputs();
    void loadCurrentExample();
    void updateStatus(const QString &state);

    MethodId currentMethod_ {MethodId::Bisection};

    MethodSelector *methodSelector_ {nullptr};
    InputWidget *inputWidget_ {nullptr};
    OutputWidget *outputWidget_ {nullptr};
    QPushButton *solveButton_ {nullptr};
    QPushButton *clearButton_ {nullptr};
    QPushButton *exampleButton_ {nullptr};
    QLabel *methodStatusLabel_ {nullptr};
    QFutureWatcher<SolverResult> *watcher_ {nullptr};
};

} // namespace numerical