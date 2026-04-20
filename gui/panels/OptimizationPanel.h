#pragma once

#include <QWidget>

#include "../../core/SolverResult.h"

class QLabel;
class QCheckBox;
class QDoubleSpinBox;
class QSpinBox;

namespace numerical {

class ExpressionEditor;
class MatrixInputWidget;

class OptimizationPanel final : public QWidget {
    Q_OBJECT

public:
    explicit OptimizationPanel(QWidget *parent = nullptr);

    void setMethod(MethodId methodId);
    MethodId methodId() const;
    SolverConfig buildConfig() const;
    void loadExample();
    void clearInputs();

private:
    void updateVisibility();
    void rebuildInitialGuessInputs(int size);

    MethodId methodId_ {MethodId::GoldenSection};

    QLabel *functionLabel_ {nullptr};
    QLabel *intervalLabel_ {nullptr};
    QLabel *matrixSizeLabel_ {nullptr};
    QLabel *matrixLabel_ {nullptr};
    QLabel *initialVectorLabel_ {nullptr};
    QLabel *toleranceLabel_ {nullptr};
    QLabel *maxIterationsLabel_ {nullptr};
    QLabel *infoLabel_ {nullptr};

    ExpressionEditor *functionEdit_ {nullptr};
    QWidget *intervalWidget_ {nullptr};
    QDoubleSpinBox *intervalStartSpin_ {nullptr};
    QDoubleSpinBox *intervalEndSpin_ {nullptr};
    QCheckBox *maximizeCheck_ {nullptr};
    QSpinBox *matrixSizeSpin_ {nullptr};
    MatrixInputWidget *matrixWidget_ {nullptr};
    QWidget *initialVectorWidget_ {nullptr};
    QDoubleSpinBox *toleranceSpin_ {nullptr};
    QSpinBox *maxIterationsSpin_ {nullptr};

    std::vector<QDoubleSpinBox *> initialVectorInputs_;
};

} // namespace numerical
