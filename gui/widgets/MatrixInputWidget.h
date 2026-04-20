#pragma once

#include <QWidget>

class QGridLayout;
class QDoubleSpinBox;

namespace numerical {

class MatrixInputWidget final : public QWidget {
    Q_OBJECT

public:
    explicit MatrixInputWidget(QWidget *parent = nullptr);

    void setSize(int size);
    int size() const;
    void clearValues();
    void setSystem(const std::vector<std::vector<double>> &matrix,
                   const std::vector<double> &rhs);
    std::vector<std::vector<double>> matrix() const;
    std::vector<double> rhs() const;

private:
    void rebuild();
    QDoubleSpinBox *createSpinBox() const;

    int currentSize_ {3};
    QGridLayout *gridLayout_ {nullptr};
    std::vector<std::vector<QDoubleSpinBox *>> matrixInputs_;
    std::vector<QDoubleSpinBox *> rhsInputs_;
};

} // namespace numerical
