#include "CsvExporter.h"

#include <QFile>
#include <QTextStream>

namespace numerical {

namespace {

QString escapeCell(const QString &cell) {
    QString escaped = cell;
    escaped.replace('"', "\"\"");
    if (escaped.contains(',') || escaped.contains('"') || escaped.contains('\n')) {
        escaped = '"' + escaped + '"';
    }
    return escaped;
}

} // namespace

bool CsvExporter::exportTable(const QString &path,
                              const QStringList &headers,
                              const QList<QStringList> &rows,
                              QString *errorMessage) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        if (errorMessage != nullptr) {
            *errorMessage = file.errorString();
        }
        return false;
    }

    QTextStream stream(&file);
    stream << headers.join(',') << '\n';
    for (const auto &row : rows) {
        QStringList escaped;
        escaped.reserve(row.size());
        for (const auto &cell : row) {
            escaped << escapeCell(cell);
        }
        stream << escaped.join(',') << '\n';
    }
    return true;
}

} // namespace numerical
