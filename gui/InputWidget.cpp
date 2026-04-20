#include "InputWidget.h"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QVBoxLayout>

#include "widgets/ExpressionEditor.h"
#include "widgets/MatrixInputWidget.h"

namespace numerical {

namespace {

QDoubleSpinBox *makeDoubleSpin(QWidget *parent,
                               double value,
                               double minimum = -1e6,
                               double maximum = 1e6,
                               int decimals = 6) {
    auto *spin = new QDoubleSpinBox(parent);
    spin->setRange(minimum, maximum);
    spin->setDecimals(decimals);
    spin->setValue(value);
    spin->setSingleStep(0.1);
    spin->setAccelerated(true);
    return spin;
}

} // namespace

InputWidget::InputWidget(QWidget *parent)
    : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);
    formLayout_ = new QFormLayout();
    formLayout_->setLabelAlignment(Qt::AlignTop | Qt::AlignLeft);
    formLayout_->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    functionLabel_ = new QLabel("f(x)");
    derivativeLabel_ = new QLabel("f'(x)");
    fixedPointLabel_ = new QLabel("g(x)");
    intervalLabel_ = new QLabel("Interval [a, b]");
    initialGuessLabel_ = new QLabel("Initial guess");
    secondGuessLabel_ = new QLabel("Second guess");
    toleranceLabel_ = new QLabel("Tolerance");
    maxIterationsLabel_ = new QLabel("Max iterations");
    matrixSizeLabel_ = new QLabel("Matrix size");
    matrixLabel_ = new QLabel("A | b");
    initialVectorLabel_ = new QLabel("Initial x0");
    infoLabel_ = new QLabel(this);
    infoLabel_->setWordWrap(true);
    infoLabel_->setStyleSheet("color: #475569;");

    functionEdit_ = new ExpressionEditor(this);
    derivativeEdit_ = new ExpressionEditor(this);
    fixedPointEdit_ = new ExpressionEditor(this);

    intervalWidget_ = new QWidget(this);
    auto *intervalLayout = new QHBoxLayout(intervalWidget_);
    intervalLayout->setContentsMargins(0, 0, 0, 0);
    intervalStartSpin_ = makeDoubleSpin(this, 1.0);
    intervalEndSpin_ = makeDoubleSpin(this, 2.0);
    intervalLayout->addWidget(intervalStartSpin_);
    intervalLayout->addWidget(intervalEndSpin_);

    initialGuessSpin_ = makeDoubleSpin(this, 0.0);
    secondGuessSpin_ = makeDoubleSpin(this, 1.0);
    toleranceSpin_ = makeDoubleSpin(this, 1e-4, 1e-10, 0.1, 8);
    maxIterationsSpin_ = new QSpinBox(this);
    maxIterationsSpin_->setRange(1, 10000);
    maxIterationsSpin_->setValue(100);

    matrixSizeSpin_ = new QSpinBox(this);
    matrixSizeSpin_->setRange(2, 10);
    matrixSizeSpin_->setValue(3);
    matrixWidget_ = new MatrixInputWidget(this);

    initialVectorWidget_ = new QWidget(this);

    showIterationsCheck_ = new QCheckBox("Show all iterations", this);
    showIterationsCheck_->setChecked(true);
    maximizeCheck_ = new QCheckBox("Maximize instead of minimize", this);

    formLayout_->addRow(functionLabel_, functionEdit_);
    formLayout_->addRow(derivativeLabel_, derivativeEdit_);
    formLayout_->addRow(fixedPointLabel_, fixedPointEdit_);
    formLayout_->addRow(intervalLabel_, intervalWidget_);
    formLayout_->addRow(initialGuessLabel_, initialGuessSpin_);
    formLayout_->addRow(secondGuessLabel_, secondGuessSpin_);
    formLayout_->addRow(toleranceLabel_, toleranceSpin_);
    formLayout_->addRow(maxIterationsLabel_, maxIterationsSpin_);
    formLayout_->addRow(matrixSizeLabel_, matrixSizeSpin_);
    formLayout_->addRow(matrixLabel_, matrixWidget_);
    formLayout_->addRow(initialVectorLabel_, initialVectorWidget_);

    layout->addLayout(formLayout_);
    layout->addWidget(showIterationsCheck_);
    layout->addWidget(maximizeCheck_);
    layout->addWidget(infoLabel_);
    layout->addStretch();

    connect(matrixSizeSpin_, qOverload<int>(&QSpinBox::valueChanged),
            this, &InputWidget::updateVisibility);
    connect(matrixSizeSpin_, qOverload<int>(&QSpinBox::valueChanged),
            matrixWidget_, &MatrixInputWidget::setSize);

    updateVisibility();
    loadExample();
}

void InputWidget::rebuildInitialGuessInputs(int size) {
    if (auto *oldLayout = initialVectorWidget_->layout()) {
        while (auto *item = oldLayout->takeAt(0)) {
            if (auto *widget = item->widget()) {
                widget->deleteLater();
            }
            delete item;
        }
        delete oldLayout;
    }

    auto *layout = new QHBoxLayout(initialVectorWidget_);
    layout->setContentsMargins(0, 0, 0, 0);
    initialVectorInputs_.clear();
    for (int index = 0; index < size; ++index) {
        auto *input = makeDoubleSpin(this, 0.0);
        layout->addWidget(input);
        initialVectorInputs_.push_back(input);
    }
}

void InputWidget::setMethod(MethodId methodId) {
    methodId_ = methodId;
    updateVisibility();
    loadExample();
    emit methodChanged(methodId);
}

MethodId InputWidget::methodId() const {
    return methodId_;
}

SolverConfig InputWidget::buildConfig() const {
    SolverConfig config;
    config.methodId = methodId_;
    config.functionExpression = functionEdit_->text().toStdString();
    config.derivativeExpression = derivativeEdit_->text().toStdString();
    config.fixedPointExpression = fixedPointEdit_->text().toStdString();
    config.intervalStart = intervalStartSpin_->value();
    config.intervalEnd = intervalEndSpin_->value();
    config.initialGuess = initialGuessSpin_->value();
    config.secondInitialGuess = secondGuessSpin_->value();
    config.tolerance = toleranceSpin_->value();
    config.maxIterations = maxIterationsSpin_->value();
    config.showAllIterations = showIterationsCheck_->isChecked();
    config.maximize = maximizeCheck_->isChecked();
    config.matrixA = matrixWidget_->matrix();
    config.vectorB = matrixWidget_->rhs();
    config.initialVector.reserve(initialVectorInputs_.size());
    for (auto *input : initialVectorInputs_) {
        config.initialVector.push_back(input->value());
    }
    return config;
}

void InputWidget::loadExample() {
    clearInputs();
    toleranceSpin_->setValue(1e-4);
    maxIterationsSpin_->setValue(100);
    showIterationsCheck_->setChecked(true);

    switch (methodId_) {
    case MethodId::Bisection:
        functionEdit_->setTextValue("x^3 - x - 2");
        intervalStartSpin_->setValue(1.0);
        intervalEndSpin_->setValue(2.0);
        infoLabel_->setText("Bisection: Requires function to change sign on interval.");
        break;
    case MethodId::FalsePosition:
        functionEdit_->setTextValue("x^2 - 3");
        intervalStartSpin_->setValue(1.0);
        intervalEndSpin_->setValue(2.0);
        infoLabel_->setText("False Position: Faster convergence than bisection.");
        break;
    case MethodId::NewtonRaphson:
        functionEdit_->setTextValue("x^3 - 2*x - 5");
        derivativeEdit_->setTextValue("3*x^2 - 2");
        initialGuessSpin_->setValue(2.0);
        infoLabel_->setText("Newton-Raphson: Fast quadratic convergence. Leave f'(x) blank for numerical differentiation.");
        break;
    case MethodId::FixedPoint:
        fixedPointEdit_->setTextValue("(x + cos(x)) / 2");
        initialGuessSpin_->setValue(0.5);
        infoLabel_->setText("Fixed-Point: Enter g(x) for x = g(x).");
        break;
    case MethodId::Secant:
        functionEdit_->setTextValue("exp(x) - 3*x");
        initialGuessSpin_->setValue(0.0);
        secondGuessSpin_->setValue(1.0);
        infoLabel_->setText("Secant: Uses two starting points instead of derivative.");
        break;
    case MethodId::GaussianElimination:
    case MethodId::LUFactorization:
    case MethodId::PALUFactorization:
    case MethodId::GaussJordan:
        matrixSizeSpin_->setValue(3);
        matrixWidget_->setSystem(
            {{2.0, 1.0, -1.0},
             {-3.0, -1.0, 2.0},
             {-2.0, 1.0, 2.0}},
            {8.0, -11.0, -3.0});
        infoLabel_->setText("Linear system Ax = b.");
        break;
    case MethodId::Cramer:
        matrixSizeSpin_->setValue(3);
        matrixWidget_->setSystem(
            {{2.0, 1.0, -1.0},
             {-3.0, -1.0, 2.0},
             {-2.0, 1.0, 2.0}},
            {8.0, -11.0, -3.0});
        infoLabel_->setText("Cramer's Rule: Practical only for small systems (n <= 3).");
        break;
    case MethodId::GoldenSection:
        functionEdit_->setTextValue("x^2 - 4*x + 4");
        intervalStartSpin_->setValue(0.0);
        intervalEndSpin_->setValue(4.0);
        maximizeCheck_->setChecked(false);
        infoLabel_->setText("Golden-Section: Assumes unimodal function on interval.");
        break;
    case MethodId::ConjugateGradient:
        matrixSizeSpin_->setValue(2);
        matrixWidget_->setSystem(
            {{4.0, 1.0},
             {1.0, 3.0}},
            {1.0, 2.0});
        rebuildInitialGuessInputs(2);
        for (auto *input : initialVectorInputs_) {
            input->setValue(0.0);
        }
        infoLabel_->setText("Conjugate Gradient: Requires symmetric positive-definite matrix.");
        break;
    }
}

void InputWidget::clearInputs() {
    functionEdit_->setTextValue({});
    derivativeEdit_->setTextValue({});
    fixedPointEdit_->setTextValue({});
    intervalStartSpin_->setValue(0.0);
    intervalEndSpin_->setValue(1.0);
    initialGuessSpin_->setValue(0.0);
    secondGuessSpin_->setValue(1.0);
    maximizeCheck_->setChecked(false);
    matrixWidget_->clearValues();
    for (auto *input : initialVectorInputs_) {
        input->setValue(0.0);
    }
}

void InputWidget::updateVisibility() {
    const bool isRootFinding = categoryFor(methodId_) == MethodCategory::RootFinding;
    const bool isLinearSystem = categoryFor(methodId_) == MethodCategory::LinearSystem;
    const bool isOptimization = categoryFor(methodId_) == MethodCategory::Optimization;
    const bool isGoldenSection = methodId_ == MethodId::GoldenSection;

    functionLabel_->setVisible(isRootFinding || isGoldenSection);
    functionEdit_->setVisible(isRootFinding || isGoldenSection);
    derivativeLabel_->setVisible(isRootFinding && methodId_ == MethodId::NewtonRaphson);
    derivativeEdit_->setVisible(isRootFinding && methodId_ == MethodId::NewtonRaphson);
    fixedPointLabel_->setVisible(isRootFinding && methodId_ == MethodId::FixedPoint);
    fixedPointEdit_->setVisible(isRootFinding && methodId_ == MethodId::FixedPoint);

    const bool usesInterval = isRootFinding && (methodId_ == MethodId::Bisection || methodId_ == MethodId::FalsePosition);
    intervalLabel_->setVisible(usesInterval);
    intervalWidget_->setVisible(usesInterval);

    const bool usesInitialGuess = isRootFinding && (methodId_ == MethodId::NewtonRaphson || methodId_ == MethodId::FixedPoint || methodId_ == MethodId::Secant);
    initialGuessLabel_->setVisible(usesInitialGuess);
    initialGuessSpin_->setVisible(usesInitialGuess);

    const bool usesSecondGuess = isRootFinding && methodId_ == MethodId::Secant;
    secondGuessLabel_->setVisible(usesSecondGuess);
    secondGuessSpin_->setVisible(usesSecondGuess);

    toleranceLabel_->setVisible(true);
    toleranceSpin_->setVisible(true);
    maxIterationsLabel_->setVisible(true);
    maxIterationsSpin_->setVisible(true);
    showIterationsCheck_->setVisible(isRootFinding);

    matrixSizeLabel_->setVisible(isLinearSystem || (isOptimization && !isGoldenSection));
    matrixSizeSpin_->setVisible(isLinearSystem || (isOptimization && !isGoldenSection));
    matrixLabel_->setVisible(isLinearSystem || (isOptimization && !isGoldenSection));
    matrixWidget_->setVisible(isLinearSystem || (isOptimization && !isGoldenSection));

    const bool usesInitialVector = isOptimization && !isGoldenSection;
    initialVectorLabel_->setVisible(usesInitialVector);
    initialVectorWidget_->setVisible(usesInitialVector);

    maximizeCheck_->setVisible(isGoldenSection);

    if (isOptimization) {
        rebuildInitialGuessInputs(matrixSizeSpin_->value());
    }
}

} // namespace numerical
