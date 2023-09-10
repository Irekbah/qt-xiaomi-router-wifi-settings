#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QNetworkAccessManager>
#include <QEventLoop>

class NetworkManager: public QNetworkAccessManager
{
    Q_OBJECT
public:
    NetworkManager();
    QNetworkReply* get(const QNetworkRequest&);
};

#endif // NETWORKMANAGER_H
