#pragma once
#include <QString>

class UuidGenerator {
public:
    /// Generate a UUID v4 (random) string without dashes.
    static QString generate();
};
