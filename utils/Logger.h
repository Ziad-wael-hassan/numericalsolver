#pragma once

#include <exception>
#include <string>

#include <QString>

namespace numerical {

class Logger {
public:
    static void logError(const std::string &context, const std::exception &error);
    static QString logFilePath();
};

} // namespace numerical
