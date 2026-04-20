#pragma once

#include <QWidget>

#include "../../core/SolverResult.h"

class QLabel;
class QDoubleSpinBox;
class QSpinBox;
class QCheckBox;

namespace numerical {

class ExpressionEditor;

class RootFindingPanel final : public QWidget {
    Q_OBJECT

public:
    explicit RootFindingPanel(QWidget *parent = nullptr);

    void setMethod(MethodId methodId);
    MethodId methodId() const;
    SolverConfig buildConfig() const;
    void loadExample();
    void clearInputs();

private:
    void updateVisibility();

    MethodId methodId_ {MethodId::Bisection};

    QLabel *functionLabel_ {nullptr};
    QLabel *derivativeLabel_ {nullptr};
    QLabel *fixedPointLabel_ {nullptr};
    QLabel *intervalLabel_ {nullptr};
    QLabel *initialGuessLabel_ {nullptr};
    QLabel *secondGuessLabel_ {nullptr};
    QLabel *toleranceLabel_ {nullptr};
    QLabel *maxIterationsLabel_ {nullptr};
    QLabel *infoLabel_ {nullptr};

    ExpressionEditor *functionEdit_ {nullptr};
    ExpressionEditor *derivativeEdit_ {nullptr};
    ExpressionEditor *fixedPointEdit_ {nullptr};
    QWidget *intervalWidget_ {nullptr};
    QDoubleSpinBox *intervalStartSpin_ {nullptr};
    QDoubleSpinBox *intervalEndSpin_ {nullptr};
    QDoubleSpinBox *initialGuessSpin_ {nullptr};
    QDoubleSpinBox *secondGuessSpin_ {nullptr};
    QDoubleSpinBox *toleranceSpin_ {nullptr};
    QSpinBox *maxIterationsSpin_ {nullptr};
    QCheckBox *showAllIterationsCheck_ {nullptr};
};

} // namespace numerical
