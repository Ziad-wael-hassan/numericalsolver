#include "MatrixInputWidget.h"

#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QLayoutItem>

namespace numerical {

MatrixInputWidget::MatrixInputWidget(QWidget *parent)
    : QWidget(parent)
    , gridLayout_(new QGridLayout(this)) {
    gridLayout_->setContentsMargins(0, 0, 0, 0);
    gridLayout_->setHorizontalSpacing(8);
    gridLayout_->setVerticalSpacing(8);
    rebuild();
}

void MatrixInputWidget::setSize(int size) {
    if (size == currentSize_) {
        return;
    }
    currentSize_ = size;
    rebuild();
}

int MatrixInputWidget::size() const {
    return currentSize_;
}

void MatrixInputWidget::clearValues() {
    for (auto &row : matrixInputs_) {
        for (auto *input : row) {
            input->setValue(0.0);
        }
    }
    for (auto *input : rhsInputs_) {
        input->setValue(0.0);
    }
}

void MatrixInputWidget::setSystem(const std::vector<std::vector<double>> &matrix,
                                  const std::vector<double> &rhs) {
    setSize(static_cast<int>(matrix.size()));
    for (int row = 0; row < currentSize_; ++row) {
        for (int column = 0; column < currentSize_; ++column) {
            matrixInputs_[row][column]->setValue(matrix[row][column]);
        }
        rhsInputs_[row]->setValue(rhs[row]);
    }
}

std::vector<std::vector<double>> MatrixInputWidget::matrix() const {
    std::vector<std::vector<double>> values(
        static_cast<std::size_t>(currentSize_),
        std::vector<double>(static_cast<std::size_t>(currentSize_), 0.0));

    for (int row = 0; row < currentSize_; ++row) {
        for (int column = 0; column < currentSize_; ++column) {
            values[row][column] = matrixInputs_[row][column]->value();
        }
    }
    return values;
}

std::vector<double> MatrixInputWidget::rhs() const {
    std::vector<double> values(static_cast<std::size_t>(currentSize_), 0.0);
    for (int row = 0; row < currentSize_; ++row) {
        values[row] = rhsInputs_[row]->value();
    }
    return values;
}

void MatrixInputWidget::rebuild() {
    while (auto *item = gridLayout_->takeAt(0)) {
        if (auto *widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }

    matrixInputs_.assign(static_cast<std::size_t>(currentSize_), {});
    rhsInputs_.assign(static_cast<std::size_t>(currentSize_), nullptr);

    for (int column = 0; column < currentSize_; ++column) {
        auto *label = new QLabel(QString("a%1").arg(column + 1), this);
        label->setAlignment(Qt::AlignCenter);
        gridLayout_->addWidget(label, 0, column);
    }
    auto *rhsLabel = new QLabel("b", this);
    rhsLabel->setAlignment(Qt::AlignCenter);
    gridLayout_->addWidget(rhsLabel, 0, currentSize_);

    for (int row = 0; row < currentSize_; ++row) {
        matrixInputs_[row].reserve(static_cast<std::size_t>(currentSize_));
        for (int column = 0; column < currentSize_; ++column) {
            auto *input = createSpinBox();
            matrixInputs_[row].push_back(input);
            gridLayout_->addWidget(input, row + 1, column);
        }

        auto *rhsInput = createSpinBox();
        rhsInputs_[row] = rhsInput;
        gridLayout_->addWidget(rhsInput, row + 1, currentSize_);
    }
}

QDoubleSpinBox *MatrixInputWidget::createSpinBox() const {
    auto *spinBox = new QDoubleSpinBox();
    spinBox->setRange(-1e6, 1e6);
    spinBox->setDecimals(6);
    spinBox->setSingleStep(0.1);
    spinBox->setAccelerated(true);
    return spinBox;
}

} // namespace numerical
