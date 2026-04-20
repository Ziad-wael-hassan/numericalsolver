#include "ExpressionEditor.h"

#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QTextDocument>

namespace numerical {

namespace {

class ExpressionHighlighter final : public QSyntaxHighlighter {
public:
    explicit ExpressionHighlighter(QTextDocument *document)
        : QSyntaxHighlighter(document) {
        QTextCharFormat numberFormat;
        numberFormat.setForeground(QColor("#2563eb"));
        rules_.push_back({QRegularExpression(R"(\b\d+(\.\d+)?([eE][+-]?\d+)?\b)"), numberFormat});

        QTextCharFormat operatorFormat;
        operatorFormat.setForeground(QColor("#64748b"));
        rules_.push_back({QRegularExpression(R"([+\-*/^()])"), operatorFormat});

        QTextCharFormat functionFormat;
        functionFormat.setForeground(QColor("#7c3aed"));
        rules_.push_back({QRegularExpression(R"(\b(sin|cos|tan|exp|log|log10|sqrt|abs)\b)"), functionFormat});

        QTextCharFormat variableFormat;
        variableFormat.setForeground(QColor("#ea580c"));
        variableFormat.setFontWeight(QFont::Bold);
        rules_.push_back({QRegularExpression(R"(\bx\b)"), variableFormat});

        QTextCharFormat invalidFormat;
        invalidFormat.setUnderlineColor(QColor("#dc2626"));
        invalidFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);
        invalidFormat.setForeground(QColor("#dc2626"));
        invalidRule_ = {QRegularExpression(R"(\b([A-Z][A-Za-z0-9_]*)\b)"), invalidFormat};
    }

protected:
    void highlightBlock(const QString &text) override {
        for (const auto &rule : rules_) {
            auto iterator = rule.pattern.globalMatch(text);
            while (iterator.hasNext()) {
                const auto match = iterator.next();
                setFormat(match.capturedStart(), match.capturedLength(), rule.format);
            }
        }

        auto invalidMatches = invalidRule_.pattern.globalMatch(text);
        while (invalidMatches.hasNext()) {
            const auto match = invalidMatches.next();
            setFormat(match.capturedStart(), match.capturedLength(), invalidRule_.format);
        }
    }

private:
    struct Rule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    QList<Rule> rules_;
    Rule invalidRule_;
};

} // namespace

ExpressionEditor::ExpressionEditor(QWidget *parent)
    : QPlainTextEdit(parent) {
    setPlaceholderText("e.g., x^3 - 2*x - 5");
    setToolTip("Supported functions: sin, cos, tan, exp, log, log10, sqrt, abs");
    setTabChangesFocus(true);
    setFixedHeight(54);
    QFont font("JetBrains Mono");
    font.setStyleHint(QFont::Monospace);
    font.setPointSize(11);
    setFont(font);
    new ExpressionHighlighter(document());
}

QString ExpressionEditor::text() const {
    return toPlainText().trimmed();
}

void ExpressionEditor::setTextValue(const QString &value) {
    setPlainText(value);
}

} // namespace numerical
