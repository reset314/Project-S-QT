#include <QCoreApplication>
#include <QSslSocket>
#include <QDebug>
int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    qDebug() << "OpenSSL support:" << QSslSocket::supportsSsl();
    qDebug() << "SSL version:" << QSslSocket::sslLibraryBuildVersionString();
    qDebug() << "SSL runtime:" << QSslSocket::sslLibraryVersionString();
    return 0;
}
