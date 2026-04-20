#include "MainWindow.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSplitter>
#include <QStackedWidget>
#include <QStatusBar>
#include <QStyleFactory>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QtConcurrent/QtConcurrentRun>

#include "../core/solvers/SolverFactory.h"
#include "../utils/Logger.h"
#include "MethodSelector.h"
#include "output/GraphTab.h"
#include "output/IterationTableTab.h"
#include "output/SolutionTab.h"
#include "panels/OptimizationPanel.h"
#include "panels/RootFindingPanel.h"
#include "panels/SystemPanel.h"

namespace numerical {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , watcher_(new QFutureWatcher<SolverResult>(this))
    , settings_("NumericalSolver", "NumericalSolver") {
    setWindowTitle("Numerical Analysis Solver");
    resize(1480, 920);

    auto *central = new QWidget(this);
    auto *rootLayout = new QVBoxLayout(central);

    auto *headerLayout = new QHBoxLayout();
    auto *title = new QLabel("Numerical Analysis Solver", this);
    QFont titleFont = title->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    title->setFont(titleFont);

    themeToggleButton_ = new QPushButton(this);
    themeToggleButton_->setCheckable(true);

    headerLayout->addWidget(title);
    headerLayout->addStretch();
    headerLayout->addWidget(themeToggleButton_);
    rootLayout->addLayout(headerLayout);

    methodSelector_ = new MethodSelector(this);
    rootFindingPanel_ = new RootFindingPanel(this);
    systemPanel_ = new SystemPanel(this);
    optimizationPanel_ = new OptimizationPanel(this);

    inputStack_ = new QStackedWidget(this);
    inputStack_->addWidget(rootFindingPanel_);
    inputStack_->addWidget(systemPanel_);
    inputStack_->addWidget(optimizationPanel_);

    solveButton_ = new QPushButton("Solve", this);
    clearButton_ = new QPushButton("Clear", this);
    exampleButton_ = new QPushButton("Load Example", this);

    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(solveButton_);
    buttonLayout->addWidget(clearButton_);
    buttonLayout->addWidget(exampleButton_);

    auto *inputContainer = new QWidget(this);
    auto *inputLayout = new QVBoxLayout(inputContainer);
    inputLayout->addWidget(inputStack_);
    inputLayout->addLayout(buttonLayout);

    solutionTab_ = new SolutionTab(this);
    iterationTableTab_ = new IterationTableTab(this);
    graphTab_ = new GraphTab(this);

    outputTabs_ = new QTabWidget(this);
    outputTabs_->addTab(solutionTab_, "Solution");
    outputTabs_->addTab(iterationTableTab_, "Iterations");
    outputTabs_->addTab(graphTab_, "Graph");

    auto *splitter = new QSplitter(this);
    splitter->addWidget(methodSelector_);
    splitter->addWidget(inputContainer);
    splitter->addWidget(outputTabs_);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 2);
    splitter->setStretchFactor(2, 3);
    rootLayout->addWidget(splitter, 1);

    setCentralWidget(central);

    methodStatusLabel_ = new QLabel(this);
    stateStatusLabel_ = new QLabel(this);
    errorStatusLabel_ = new QLabel(this);
    statusBar()->addPermanentWidget(methodStatusLabel_, 1);
    statusBar()->addPermanentWidget(stateStatusLabel_, 1);
    statusBar()->addPermanentWidget(errorStatusLabel_, 2);

    connect(methodSelector_, &MethodSelector::methodSelected,
            this, &MainWindow::handleMethodSelected);
    connect(solveButton_, &QPushButton::clicked,
            this, &MainWindow::solveCurrentMethod);
    connect(clearButton_, &QPushButton::clicked,
            this, &MainWindow::clearCurrentInputs);
    connect(exampleButton_, &QPushButton::clicked,
            this, &MainWindow::loadCurrentExample);
    connect(themeToggleButton_, &QPushButton::toggled,
            this, &MainWindow::applyTheme);
    connect(watcher_, &QFutureWatcher<SolverResult>::finished,
            this, &MainWindow::onSolveFinished);

    const bool darkMode = settings_.value("ui/darkMode", false).toBool();
    themeToggleButton_->setChecked(darkMode);
    applyTheme(darkMode);
    handleMethodSelected(MethodId::Bisection);
}

void MainWindow::handleMethodSelected(MethodId methodId) {
    currentMethod_ = methodId;
    switch (categoryFor(methodId)) {
    case MethodCategory::RootFinding:
        inputStack_->setCurrentWidget(rootFindingPanel_);
        rootFindingPanel_->setMethod(methodId);
        break;
    case MethodCategory::LinearSystem:
        inputStack_->setCurrentWidget(systemPanel_);
        systemPanel_->setMethod(methodId);
        break;
    case MethodCategory::Optimization:
        inputStack_->setCurrentWidget(optimizationPanel_);
        optimizationPanel_->setMethod(methodId);
        break;
    }
    updateStatus("Ready");
}

void MainWindow::solveCurrentMethod() {
    solveButton_->setEnabled(false);
    clearButton_->setEnabled(false);
    exampleButton_->setEnabled(false);
    updateStatus("Solving");

    const SolverConfig config = buildCurrentConfig();
    watcher_->setFuture(QtConcurrent::run([config]() {
        auto solver = SolverFactory::create(config.methodId);
        return solver->solve(config);
    }));
}

void MainWindow::onSolveFinished() {
    solveButton_->setEnabled(true);
    clearButton_->setEnabled(true);
    exampleButton_->setEnabled(true);

    try {
        const SolverResult result = watcher_->future().result();
        solutionTab_->displayResult(result);
        iterationTableTab_->displayResult(result);
        graphTab_->displayResult(result);
        updateStatus("Completed");
    } catch (const std::exception &error) {
        Logger::logError(toDisplayName(currentMethod_), error);
        updateStatus("Failed", error.what());
        QMessageBox::critical(this, "Solve Failed", error.what());
    }
}

void MainWindow::clearCurrentInputs() {
    switch (categoryFor(currentMethod_)) {
    case MethodCategory::RootFinding:
        rootFindingPanel_->clearInputs();
        break;
    case MethodCategory::LinearSystem:
        systemPanel_->clearInputs();
        break;
    case MethodCategory::Optimization:
        optimizationPanel_->clearInputs();
        break;
    }
    updateStatus("Cleared");
}

void MainWindow::loadCurrentExample() {
    switch (categoryFor(currentMethod_)) {
    case MethodCategory::RootFinding:
        rootFindingPanel_->loadExample();
        break;
    case MethodCategory::LinearSystem:
        systemPanel_->loadExample();
        break;
    case MethodCategory::Optimization:
        optimizationPanel_->loadExample();
        break;
    }
    updateStatus("Example loaded");
}

SolverConfig MainWindow::buildCurrentConfig() const {
    switch (categoryFor(currentMethod_)) {
    case MethodCategory::RootFinding:
        return rootFindingPanel_->buildConfig();
    case MethodCategory::LinearSystem:
        return systemPanel_->buildConfig();
    case MethodCategory::Optimization:
        return optimizationPanel_->buildConfig();
    }
    return rootFindingPanel_->buildConfig();
}

void MainWindow::applyTheme(bool darkMode) {
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    QPalette palette;
    if (darkMode) {
        palette.setColor(QPalette::Window, QColor("#0f172a"));
        palette.setColor(QPalette::WindowText, QColor("#e2e8f0"));
        palette.setColor(QPalette::Base, QColor("#111827"));
        palette.setColor(QPalette::AlternateBase, QColor("#1f2937"));
        palette.setColor(QPalette::Text, QColor("#e5e7eb"));
        palette.setColor(QPalette::Button, QColor("#1e293b"));
        palette.setColor(QPalette::ButtonText, QColor("#e2e8f0"));
        palette.setColor(QPalette::Highlight, QColor("#0ea5e9"));
        palette.setColor(QPalette::HighlightedText, QColor("#020617"));
    } else {
        palette = QApplication::style()->standardPalette();
    }

    qApp->setPalette(palette);
    themeToggleButton_->setText(darkMode ? "Light Mode" : "Dark Mode");
    settings_.setValue("ui/darkMode", darkMode);
}

void MainWindow::updateStatus(const QString &state, const QString &lastError) {
    methodStatusLabel_->setText(QString("Method: %1")
                                    .arg(QString::fromStdString(toDisplayName(currentMethod_))));
    stateStatusLabel_->setText(QString("Status: %1").arg(state));
    errorStatusLabel_->setText(QString("Last error: %1").arg(lastError));
}

} // namespace numerical
