#pragma once
#include <QString>

class UuidGenerator {
public:
    /// Generate a UUID v7 (time-ordered) string without dashes.
    static QString generate();
};
