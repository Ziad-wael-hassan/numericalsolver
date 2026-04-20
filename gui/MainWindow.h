#pragma once

#include <QFutureWatcher>
#include <QMainWindow>
#include <QSettings>

#include "../core/SolverResult.h"

class QLabel;
class QPushButton;
class QStackedWidget;
class QTabWidget;

namespace numerical {

class GraphTab;
class IterationTableTab;
class MethodSelector;
class OptimizationPanel;
class RootFindingPanel;
class SolutionTab;
class SystemPanel;

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
    SolverConfig buildCurrentConfig() const;
    void applyTheme(bool darkMode);
    void updateStatus(const QString &state, const QString &lastError = {});

    MethodId currentMethod_ {MethodId::Bisection};

    MethodSelector *methodSelector_ {nullptr};
    RootFindingPanel *rootFindingPanel_ {nullptr};
    SystemPanel *systemPanel_ {nullptr};
    OptimizationPanel *optimizationPanel_ {nullptr};
    QStackedWidget *inputStack_ {nullptr};
    SolutionTab *solutionTab_ {nullptr};
    IterationTableTab *iterationTableTab_ {nullptr};
    GraphTab *graphTab_ {nullptr};
    QTabWidget *outputTabs_ {nullptr};
    QPushButton *solveButton_ {nullptr};
    QPushButton *clearButton_ {nullptr};
    QPushButton *exampleButton_ {nullptr};
    QPushButton *themeToggleButton_ {nullptr};
    QLabel *methodStatusLabel_ {nullptr};
    QLabel *stateStatusLabel_ {nullptr};
    QLabel *errorStatusLabel_ {nullptr};
    QFutureWatcher<SolverResult> *watcher_ {nullptr};
    QSettings settings_;
};

} // namespace numerical
