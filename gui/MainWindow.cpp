#include "MainWindow.h"

#include <QApplication>
#include <QPalette>
#include <QStyleFactory>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QtConcurrent/QtConcurrentRun>

#include "InputWidget.h"
#include "OutputWidget.h"
#include "MethodSelector.h"
#include "../core/solvers/SolverFactory.h"
#include "../utils/Logger.h"

namespace numerical {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , watcher_(new QFutureWatcher<SolverResult>(this)) {
    setWindowTitle("Numerical Solver");
    resize(1000, 700);

    applyDarkMode();

    auto *central = new QWidget(this);
    auto *rootLayout = new QVBoxLayout(central);
    rootLayout->setSpacing(8);

    methodSelector_ = new MethodSelector(this);

    inputWidget_ = new InputWidget(this);

    solveButton_ = new QPushButton("Solve", this);
    clearButton_ = new QPushButton("Clear", this);
    exampleButton_ = new QPushButton("Example", this);

    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(solveButton_);
    buttonLayout->addWidget(clearButton_);
    buttonLayout->addWidget(exampleButton_);
    buttonLayout->addStretch();

    outputWidget_ = new OutputWidget(this);

    rootLayout->addWidget(methodSelector_);
    rootLayout->addWidget(inputWidget_, 1);
    rootLayout->addLayout(buttonLayout);
    rootLayout->addWidget(outputWidget_, 2);

    setCentralWidget(central);

    methodStatusLabel_ = new QLabel(this);
    statusBar()->addWidget(methodStatusLabel_);

    connect(methodSelector_, &MethodSelector::methodSelected,
            this, &MainWindow::handleMethodSelected);
    connect(solveButton_, &QPushButton::clicked,
            this, &MainWindow::solveCurrentMethod);
    connect(clearButton_, &QPushButton::clicked,
            this, &MainWindow::clearCurrentInputs);
    connect(exampleButton_, &QPushButton::clicked,
            this, &MainWindow::loadCurrentExample);
    connect(watcher_, &QFutureWatcher<SolverResult>::finished,
            this, &MainWindow::onSolveFinished);

    handleMethodSelected(MethodId::Bisection);
}

void MainWindow::applyDarkMode() {
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    QPalette palette;
    palette.setColor(QPalette::Window, QColor("#1e1e1e"));
    palette.setColor(QPalette::WindowText, QColor("#e0e0e0"));
    palette.setColor(QPalette::Base, QColor("#2d2d2d"));
    palette.setColor(QPalette::AlternateBase, QColor("#3d3d3d"));
    palette.setColor(QPalette::Text, QColor("#e0e0e0"));
    palette.setColor(QPalette::Button, QColor("#3d3d3d"));
    palette.setColor(QPalette::ButtonText, QColor("#e0e0e0"));
    palette.setColor(QPalette::Highlight, QColor("#0078d4"));
    palette.setColor(QPalette::HighlightedText, QColor("#ffffff"));
    qApp->setPalette(palette);
}

void MainWindow::handleMethodSelected(MethodId methodId) {
    currentMethod_ = methodId;
    inputWidget_->setMethod(methodId);
    updateStatus("Ready");
}

void MainWindow::solveCurrentMethod() {
    solveButton_->setEnabled(false);
    clearButton_->setEnabled(false);
    exampleButton_->setEnabled(false);
    updateStatus("Solving...");

    const SolverConfig config = inputWidget_->buildConfig();
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
        outputWidget_->displayResult(result);
        updateStatus("Done");
    } catch (const std::exception &error) {
        Logger::logError(toDisplayName(currentMethod_), error);
        updateStatus("Error");
        QMessageBox::critical(this, "Error", error.what());
    }
}

void MainWindow::clearCurrentInputs() {
    inputWidget_->clearInputs();
    updateStatus("Cleared");
}

void MainWindow::loadCurrentExample() {
    inputWidget_->loadExample();
    updateStatus("Example loaded");
}

void MainWindow::updateStatus(const QString &state) {
    methodStatusLabel_->setText(QString("%1 - %2")
                                    .arg(QString::fromStdString(toDisplayName(currentMethod_)))
                                    .arg(state));
}

} // namespace numerical