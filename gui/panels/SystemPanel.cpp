#include "SystemPanel.h"

#include <QFormLayout>
#include <QLabel>
#include <QSpinBox>
#include <QVBoxLayout>

#include "../widgets/MatrixInputWidget.h"

namespace numerical {

SystemPanel::SystemPanel(QWidget *parent)
    : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);
    auto *form = new QFormLayout();

    sizeSpin_ = new QSpinBox(this);
    sizeSpin_->setRange(2, 10);
    sizeSpin_->setValue(3);

    matrixWidget_ = new MatrixInputWidget(this);
    infoLabel_ = new QLabel(this);
    infoLabel_->setWordWrap(true);
    infoLabel_->setStyleSheet("color: #475569;");

    form->addRow("Matrix size", sizeSpin_);
    layout->addLayout(form);
    layout->addWidget(matrixWidget_);
    layout->addWidget(infoLabel_);
    layout->addStretch();

    connect(sizeSpin_, qOverload<int>(&QSpinBox::valueChanged),
            matrixWidget_, &MatrixInputWidget::setSize);

    loadExample();
}

void SystemPanel::setMethod(MethodId methodId) {
    methodId_ = methodId;
    infoLabel_->setText(methodId_ == MethodId::Cramer
                            ? "Cramer's Rule is practical only for small systems."
                            : QString());
    loadExample();
}

MethodId SystemPanel::methodId() const {
    return methodId_;
}

SolverConfig SystemPanel::buildConfig() const {
    SolverConfig config;
    config.methodId = methodId_;
    config.matrixA = matrixWidget_->matrix();
    config.vectorB = matrixWidget_->rhs();
    return config;
}

void SystemPanel::loadExample() {
    sizeSpin_->setValue(3);
    matrixWidget_->setSystem(
        {
            {2.0, 1.0, -1.0},
            {-3.0, -1.0, 2.0},
            {-2.0, 1.0, 2.0},
        },
        {8.0, -11.0, -3.0});
}

void SystemPanel::clearInputs() {
    matrixWidget_->clearValues();
}

} // namespace numerical
