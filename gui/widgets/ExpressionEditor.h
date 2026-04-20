#pragma once

#include <QPlainTextEdit>

namespace numerical {

class ExpressionEditor final : public QPlainTextEdit {
    Q_OBJECT

public:
    explicit ExpressionEditor(QWidget *parent = nullptr);

    QString text() const;
    void setTextValue(const QString &value);
};

} // namespace numerical
