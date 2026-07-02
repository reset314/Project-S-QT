#include "UuidGenerator.h"
#include <QUuid>

QString UuidGenerator::generate() {
    return QUuid::createUuid().toString(QUuid::WithoutBraces).remove('-');
}
