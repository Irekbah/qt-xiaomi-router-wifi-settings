#include "networkmanager.h"

NetworkManager::NetworkManager()
{

}

QNetworkReply* NetworkManager::get(const QNetworkRequest &request)
{
    QEventLoop loop;
    connect(this, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    QNetworkReply *reply = QNetworkAccessManager::get(request);
    loop.exec();
    disconnect(this, SIGNAL(finished(QNetworkReply*)), nullptr, nullptr);
    return reply;
}
