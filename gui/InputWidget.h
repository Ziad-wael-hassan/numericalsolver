#pragma once

#include <QWidget>

#include "../core/SolverResult.h"

class QCheckBox;
class QDoubleSpinBox;
class QFormLayout;
class QLabel;
class QSpinBox;

namespace numerical {

class ExpressionEditor;
class MatrixInputWidget;

class InputWidget final : public QWidget {
    Q_OBJECT

public:
    explicit InputWidget(QWidget *parent = nullptr);

    void setMethod(MethodId methodId);
    MethodId methodId() const;
    SolverConfig buildConfig() const;
    void loadExample();
    void clearInputs();

signals:
    void methodChanged(MethodId methodId);

private:
    void updateVisibility();
    void rebuildInitialGuessInputs(int size);

    MethodId methodId_ {MethodId::Bisection};

    QLabel *functionLabel_ {nullptr};
    QLabel *derivativeLabel_ {nullptr};
    QLabel *fixedPointLabel_ {nullptr};
    QLabel *intervalLabel_ {nullptr};
    QLabel *initialGuessLabel_ {nullptr};
    QLabel *secondGuessLabel_ {nullptr};
    QLabel *toleranceLabel_ {nullptr};
    QLabel *maxIterationsLabel_ {nullptr};
    QLabel *matrixSizeLabel_ {nullptr};
    QLabel *matrixLabel_ {nullptr};
    QLabel *initialVectorLabel_ {nullptr};
    QLabel *infoLabel_ {nullptr};

    QWidget *intervalWidget_ {nullptr};
    QDoubleSpinBox *intervalStartSpin_ {nullptr};
    QDoubleSpinBox *intervalEndSpin_ {nullptr};
    QDoubleSpinBox *initialGuessSpin_ {nullptr};
    QDoubleSpinBox *secondGuessSpin_ {nullptr};
    QDoubleSpinBox *toleranceSpin_ {nullptr};
    QSpinBox *maxIterationsSpin_ {nullptr};
    QSpinBox *matrixSizeSpin_ {nullptr};
    QCheckBox *showIterationsCheck_ {nullptr};
    QCheckBox *maximizeCheck_ {nullptr};

    ExpressionEditor *functionEdit_ {nullptr};
    ExpressionEditor *derivativeEdit_ {nullptr};
    ExpressionEditor *fixedPointEdit_ {nullptr};
    MatrixInputWidget *matrixWidget_ {nullptr};
    QWidget *initialVectorWidget_ {nullptr};
    std::vector<QDoubleSpinBox *> initialVectorInputs_;

    QFormLayout *formLayout_ {nullptr};
};

} // namespace numerical
