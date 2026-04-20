#include "RootFindingPanel.h"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QVBoxLayout>

#include "../widgets/ExpressionEditor.h"

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

RootFindingPanel::RootFindingPanel(QWidget *parent)
    : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);
    auto *formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignTop | Qt::AlignLeft);
    formLayout->setFormAlignment(Qt::AlignTop);
    formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    functionLabel_ = new QLabel("f(x)");
    derivativeLabel_ = new QLabel("f'(x)");
    fixedPointLabel_ = new QLabel("g(x)");
    intervalLabel_ = new QLabel("Interval [a, b]");
    initialGuessLabel_ = new QLabel("Initial guess");
    secondGuessLabel_ = new QLabel("Second guess");
    toleranceLabel_ = new QLabel("Tolerance");
    maxIterationsLabel_ = new QLabel("Max iterations");

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

    showAllIterationsCheck_ = new QCheckBox("Show all iterations", this);
    showAllIterationsCheck_->setChecked(true);
    infoLabel_ = new QLabel(this);
    infoLabel_->setWordWrap(true);
    infoLabel_->setStyleSheet("color: #475569;");

    formLayout->addRow(functionLabel_, functionEdit_);
    formLayout->addRow(derivativeLabel_, derivativeEdit_);
    formLayout->addRow(fixedPointLabel_, fixedPointEdit_);
    formLayout->addRow(intervalLabel_, intervalWidget_);
    formLayout->addRow(initialGuessLabel_, initialGuessSpin_);
    formLayout->addRow(secondGuessLabel_, secondGuessSpin_);
    formLayout->addRow(toleranceLabel_, toleranceSpin_);
    formLayout->addRow(maxIterationsLabel_, maxIterationsSpin_);

    layout->addLayout(formLayout);
    layout->addWidget(showAllIterationsCheck_);
    layout->addWidget(infoLabel_);
    layout->addStretch();

    updateVisibility();
    loadExample();
}

void RootFindingPanel::setMethod(MethodId methodId) {
    methodId_ = methodId;
    updateVisibility();
    loadExample();
}

MethodId RootFindingPanel::methodId() const {
    return methodId_;
}

SolverConfig RootFindingPanel::buildConfig() const {
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
    config.showAllIterations = showAllIterationsCheck_->isChecked();
    return config;
}

void RootFindingPanel::loadExample() {
    clearInputs();
    toleranceSpin_->setValue(1e-4);
    maxIterationsSpin_->setValue(100);
    showAllIterationsCheck_->setChecked(true);

    switch (methodId_) {
    case MethodId::Bisection:
        functionEdit_->setTextValue("x^3 - x - 2");
        intervalStartSpin_->setValue(1.0);
        intervalEndSpin_->setValue(2.0);
        break;
    case MethodId::FalsePosition:
        functionEdit_->setTextValue("x^2 - 3");
        intervalStartSpin_->setValue(1.0);
        intervalEndSpin_->setValue(2.0);
        break;
    case MethodId::NewtonRaphson:
        functionEdit_->setTextValue("x^3 - 2*x - 5");
        derivativeEdit_->setTextValue("3*x^2 - 2");
        initialGuessSpin_->setValue(2.0);
        break;
    case MethodId::FixedPoint:
        fixedPointEdit_->setTextValue("(x + cos(x)) / 2");
        initialGuessSpin_->setValue(0.5);
        break;
    case MethodId::Secant:
        functionEdit_->setTextValue("exp(x) - 3*x");
        initialGuessSpin_->setValue(0.0);
        secondGuessSpin_->setValue(1.0);
        break;
    default:
        break;
    }
}

void RootFindingPanel::clearInputs() {
    functionEdit_->setTextValue({});
    derivativeEdit_->setTextValue({});
    fixedPointEdit_->setTextValue({});
    intervalStartSpin_->setValue(0.0);
    intervalEndSpin_->setValue(1.0);
    initialGuessSpin_->setValue(0.0);
    secondGuessSpin_->setValue(1.0);
}

void RootFindingPanel::updateVisibility() {
    const bool usesFunction = methodId_ != MethodId::FixedPoint;
    const bool usesDerivative = methodId_ == MethodId::NewtonRaphson;
    const bool usesFixedPoint = methodId_ == MethodId::FixedPoint;
    const bool usesInterval = methodId_ == MethodId::Bisection ||
                              methodId_ == MethodId::FalsePosition;
    const bool usesInitialGuess = methodId_ == MethodId::NewtonRaphson ||
                                  methodId_ == MethodId::FixedPoint ||
                                  methodId_ == MethodId::Secant;
    const bool usesSecondGuess = methodId_ == MethodId::Secant;

    functionLabel_->setVisible(usesFunction);
    functionEdit_->setVisible(usesFunction);
    derivativeLabel_->setVisible(usesDerivative);
    derivativeEdit_->setVisible(usesDerivative);
    fixedPointLabel_->setVisible(usesFixedPoint);
    fixedPointEdit_->setVisible(usesFixedPoint);
    intervalLabel_->setVisible(usesInterval);
    intervalWidget_->setVisible(usesInterval);
    initialGuessLabel_->setVisible(usesInitialGuess);
    initialGuessSpin_->setVisible(usesInitialGuess);
    secondGuessLabel_->setVisible(usesSecondGuess);
    secondGuessSpin_->setVisible(usesSecondGuess);

    if (methodId_ == MethodId::Secant) {
        infoLabel_->setText("Secant avoids an explicit derivative and uses two starting guesses.");
    } else if (methodId_ == MethodId::NewtonRaphson) {
        infoLabel_->setText("Leave f'(x) blank if you want the solver to fall back to numerical differentiation.");
    } else if (methodId_ == MethodId::FixedPoint) {
        infoLabel_->setText("Enter g(x) for a relation of the form x = g(x).");
    } else {
        infoLabel_->setText({});
    }
}

} // namespace numerical
