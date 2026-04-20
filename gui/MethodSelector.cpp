#include "MethodSelector.h"

#include <QHeaderView>

namespace numerical {

namespace {

QTreeWidgetItem *makeLeaf(QTreeWidgetItem *parent,
                          const QString &label,
                          MethodId methodId) {
    auto *item = new QTreeWidgetItem(parent, {label});
    item->setData(0, Qt::UserRole, static_cast<int>(methodId));
    return item;
}

} // namespace

MethodSelector::MethodSelector(QWidget *parent)
    : QTreeWidget(parent) {
    setHeaderHidden(true);
    setRootIsDecorated(true);
    setAlternatingRowColors(true);
    setMinimumWidth(240);
    header()->setSectionResizeMode(QHeaderView::Stretch);
    populate();

    connect(this, &QTreeWidget::currentItemChanged,
            this, &MethodSelector::emitCurrentMethod);
}

void MethodSelector::populate() {
    clear();

    auto *chapter1 = new QTreeWidgetItem(this, {"Chapter 1: Root Finding"});
    chapter1->setFlags(chapter1->flags() & ~Qt::ItemIsSelectable);
    makeLeaf(chapter1, "Bisection Method", MethodId::Bisection);
    makeLeaf(chapter1, "False Position Method", MethodId::FalsePosition);
    makeLeaf(chapter1, "Newton-Raphson Method", MethodId::NewtonRaphson);
    makeLeaf(chapter1, "Fixed-Point Iteration", MethodId::FixedPoint);
    makeLeaf(chapter1, "Secant Method", MethodId::Secant);

    auto *chapter2 = new QTreeWidgetItem(this, {"Chapter 2: System of Equations"});
    chapter2->setFlags(chapter2->flags() & ~Qt::ItemIsSelectable);
    makeLeaf(chapter2, "Gaussian Elimination", MethodId::GaussianElimination);
    makeLeaf(chapter2, "LU Factorization", MethodId::LUFactorization);
    makeLeaf(chapter2, "PA = LU Factorization", MethodId::PALUFactorization);
    makeLeaf(chapter2, "Gauss-Jordan Elimination", MethodId::GaussJordan);
    makeLeaf(chapter2, "Cramer's Rule", MethodId::Cramer);

    auto *chapter3 = new QTreeWidgetItem(this, {"Chapter 3: Optimization"});
    chapter3->setFlags(chapter3->flags() & ~Qt::ItemIsSelectable);
    makeLeaf(chapter3, "Golden-Section Search", MethodId::GoldenSection);
    makeLeaf(chapter3, "Conjugate Gradient Method", MethodId::ConjugateGradient);

    expandAll();
    selectMethod(MethodId::Bisection);
}

void MethodSelector::selectMethod(MethodId methodId) {
    for (int chapter = 0; chapter < topLevelItemCount(); ++chapter) {
        auto *parent = topLevelItem(chapter);
        for (int row = 0; row < parent->childCount(); ++row) {
            auto *item = parent->child(row);
            if (item->data(0, Qt::UserRole).toInt() == static_cast<int>(methodId)) {
                setCurrentItem(item);
                return;
            }
        }
    }

    auto *item = topLevelItem(0)->child(0);
    setCurrentItem(item);
}

void MethodSelector::emitCurrentMethod(QTreeWidgetItem *current,
                                       QTreeWidgetItem * /*previous*/) {
    if (current == nullptr || current->childCount() > 0) {
        return;
    }
    emit methodSelected(static_cast<MethodId>(current->data(0, Qt::UserRole).toInt()));
}

} // namespace numerical
