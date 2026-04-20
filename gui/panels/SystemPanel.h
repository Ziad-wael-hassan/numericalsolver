#pragma once

#include <QWidget>

#include "../../core/SolverResult.h"

class QLabel;
class QSpinBox;

namespace numerical {

class MatrixInputWidget;

class SystemPanel final : public QWidget {
    Q_OBJECT

public:
    explicit SystemPanel(QWidget *parent = nullptr);

    void setMethod(MethodId methodId);
    MethodId methodId() const;
    SolverConfig buildConfig() const;
    void loadExample();
    void clearInputs();

private:
    MethodId methodId_ {MethodId::GaussianElimination};
    QSpinBox *sizeSpin_ {nullptr};
    MatrixInputWidget *matrixWidget_ {nullptr};
    QLabel *infoLabel_ {nullptr};
};

} // namespace numerical
