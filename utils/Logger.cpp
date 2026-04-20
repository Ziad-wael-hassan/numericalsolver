#include "Logger.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QTextStream>

namespace numerical {

QString Logger::logFilePath() {
    QString directory = QStandardPaths::writableLocation(
        QStandardPaths::AppDataLocation);
    if (directory.isEmpty()) {
        directory = QDir::homePath() + "/.numericalsolver";
    }

    QDir dir(directory);
    dir.mkpath(".");
    return dir.filePath("solver_errors.log");
}

void Logger::logError(const std::string &context, const std::exception &error) {
    QFile file(logFilePath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        return;
    }

    QTextStream stream(&file);
    stream << QDateTime::currentDateTime().toString(Qt::ISODate) << " | "
           << QString::fromStdString(context) << " | "
           << QString::fromStdString(error.what()) << '\n';
}

} // namespace numerical
