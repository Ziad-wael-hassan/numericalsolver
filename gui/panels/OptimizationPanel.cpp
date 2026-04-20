#include "OptimizationPanel.h"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayoutItem>
#include <QSpinBox>
#include <QVBoxLayout>

#include "../widgets/ExpressionEditor.h"
#include "../widgets/MatrixInputWidget.h"

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

OptimizationPanel::OptimizationPanel(QWidget *parent)
    : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);
    auto *formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignTop | Qt::AlignLeft);
    formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    functionLabel_ = new QLabel("f(x)");
    intervalLabel_ = new QLabel("Interval [a, b]");
    matrixSizeLabel_ = new QLabel("Matrix size");
    matrixLabel_ = new QLabel("A | b");
    initialVectorLabel_ = new QLabel("Initial x0");
    toleranceLabel_ = new QLabel("Tolerance");
    maxIterationsLabel_ = new QLabel("Max iterations");

    functionEdit_ = new ExpressionEditor(this);
    intervalWidget_ = new QWidget(this);
    auto *intervalLayout = new QHBoxLayout(intervalWidget_);
    intervalLayout->setContentsMargins(0, 0, 0, 0);
    intervalStartSpin_ = makeDoubleSpin(this, 0.0);
    intervalEndSpin_ = makeDoubleSpin(this, 4.0);
    intervalLayout->addWidget(intervalStartSpin_);
    intervalLayout->addWidget(intervalEndSpin_);

    maximizeCheck_ = new QCheckBox("Maximize instead of minimize", this);
    matrixSizeSpin_ = new QSpinBox(this);
    matrixSizeSpin_->setRange(2, 10);
    matrixSizeSpin_->setValue(2);
    matrixWidget_ = new MatrixInputWidget(this);
    initialVectorWidget_ = new QWidget(this);
    toleranceSpin_ = makeDoubleSpin(this, 1e-4, 1e-10, 0.1, 8);
    maxIterationsSpin_ = new QSpinBox(this);
    maxIterationsSpin_->setRange(1, 10000);
    maxIterationsSpin_->setValue(100);
    infoLabel_ = new QLabel(this);
    infoLabel_->setWordWrap(true);
    infoLabel_->setStyleSheet("color: #475569;");

    formLayout->addRow(functionLabel_, functionEdit_);
    formLayout->addRow(intervalLabel_, intervalWidget_);
    formLayout->addRow("", maximizeCheck_);
    formLayout->addRow(matrixSizeLabel_, matrixSizeSpin_);
    formLayout->addRow(matrixLabel_, matrixWidget_);
    formLayout->addRow(initialVectorLabel_, initialVectorWidget_);
    formLayout->addRow(toleranceLabel_, toleranceSpin_);
    formLayout->addRow(maxIterationsLabel_, maxIterationsSpin_);

    layout->addLayout(formLayout);
    layout->addWidget(infoLabel_);
    layout->addStretch();

    connect(matrixSizeSpin_, qOverload<int>(&QSpinBox::valueChanged),
            this, &OptimizationPanel::rebuildInitialGuessInputs);
    connect(matrixSizeSpin_, qOverload<int>(&QSpinBox::valueChanged),
            matrixWidget_, &MatrixInputWidget::setSize);

    rebuildInitialGuessInputs(matrixSizeSpin_->value());
    updateVisibility();
    loadExample();
}

void OptimizationPanel::setMethod(MethodId methodId) {
    methodId_ = methodId;
    updateVisibility();
    loadExample();
}

MethodId OptimizationPanel::methodId() const {
    return methodId_;
}

SolverConfig OptimizationPanel::buildConfig() const {
    SolverConfig config;
    config.methodId = methodId_;
    config.functionExpression = functionEdit_->text().toStdString();
    config.intervalStart = intervalStartSpin_->value();
    config.intervalEnd = intervalEndSpin_->value();
    config.tolerance = toleranceSpin_->value();
    config.maxIterations = maxIterationsSpin_->value();
    config.maximize = maximizeCheck_->isChecked();
    config.matrixA = matrixWidget_->matrix();
    config.vectorB = matrixWidget_->rhs();
    config.initialVector.reserve(initialVectorInputs_.size());
    for (auto *input : initialVectorInputs_) {
        config.initialVector.push_back(input->value());
    }
    return config;
}

void OptimizationPanel::loadExample() {
    clearInputs();
    toleranceSpin_->setValue(1e-4);
    maxIterationsSpin_->setValue(100);

    if (methodId_ == MethodId::GoldenSection) {
        functionEdit_->setTextValue("x^2 - 4*x + 4");
        intervalStartSpin_->setValue(0.0);
        intervalEndSpin_->setValue(4.0);
        maximizeCheck_->setChecked(false);
    } else {
        matrixSizeSpin_->setValue(2);
        matrixWidget_->setSystem(
            {
                {4.0, 1.0},
                {1.0, 3.0},
            },
            {1.0, 2.0});
        rebuildInitialGuessInputs(2);
        for (auto *input : initialVectorInputs_) {
            input->setValue(0.0);
        }
    }
}

void OptimizationPanel::clearInputs() {
    functionEdit_->setTextValue({});
    intervalStartSpin_->setValue(0.0);
    intervalEndSpin_->setValue(1.0);
    maximizeCheck_->setChecked(false);
    matrixWidget_->clearValues();
    for (auto *input : initialVectorInputs_) {
        input->setValue(0.0);
    }
}

void OptimizationPanel::updateVisibility() {
    const bool isGoldenSection = methodId_ == MethodId::GoldenSection;

    functionLabel_->setVisible(isGoldenSection);
    functionEdit_->setVisible(isGoldenSection);
    intervalLabel_->setVisible(isGoldenSection);
    intervalWidget_->setVisible(isGoldenSection);
    maximizeCheck_->setVisible(isGoldenSection);

    matrixSizeLabel_->setVisible(!isGoldenSection);
    matrixSizeSpin_->setVisible(!isGoldenSection);
    matrixLabel_->setVisible(!isGoldenSection);
    matrixWidget_->setVisible(!isGoldenSection);
    initialVectorLabel_->setVisible(!isGoldenSection);
    initialVectorWidget_->setVisible(!isGoldenSection);

    infoLabel_->setText(isGoldenSection
                            ? "Golden-Section Search assumes a unimodal function on the selected interval."
                            : "Conjugate Gradient requires a symmetric positive-definite matrix.");
}

void OptimizationPanel::rebuildInitialGuessInputs(int size) {
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

} // namespace numerical
