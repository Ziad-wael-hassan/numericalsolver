#pragma once

#include <QTreeWidget>

#include "../core/SolverResult.h"

namespace numerical {

class MethodSelector final : public QTreeWidget {
    Q_OBJECT

public:
    explicit MethodSelector(QWidget *parent = nullptr);
    void selectMethod(MethodId methodId);

signals:
    void methodSelected(numerical::MethodId methodId);

private:
    void populate();
    void emitCurrentMethod(QTreeWidgetItem *current, QTreeWidgetItem *previous);
};

} // namespace numerical
