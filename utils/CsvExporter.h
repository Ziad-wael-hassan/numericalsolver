#pragma once

#include <QList>
#include <QString>
#include <QStringList>

namespace numerical {

class CsvExporter {
public:
    static bool exportTable(const QString &path,
                            const QStringList &headers,
                            const QList<QStringList> &rows,
                            QString *errorMessage = nullptr);
};

} // namespace numerical
