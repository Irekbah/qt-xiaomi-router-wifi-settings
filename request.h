#ifndef REQUEST_H
#define REQUEST_H
#pragma once
#include <QtNetwork/QtNetwork>

class Request : public QNetworkRequest
{
public:
    Request(QUrl, int);

    static const int WIFI_STATE_REQUEST = 0;
    static const int WIFI_STATE_CHANGE = 1;

    void setUrl(const QUrl&, const int);

    int getRequestType();
    bool isStateRequest();
    bool isChangeRequest();
    void setRequestType(const int);

private:
    int requestType = WIFI_STATE_REQUEST;

//protected:
//    void setUrl(const QUrl&);

};

#endif // REQUEST_H
