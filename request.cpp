#include "request.h"


Request::Request(QUrl url, int initRequestType) : QNetworkRequest(url), requestType(initRequestType){}

void Request::setUrl(const QUrl &url, const int RequestType)
{
    QNetworkRequest::setUrl(url);
    qDebug() << "URL: " << this->url().toString();
    setRequestType(RequestType);
}

int Request::getRequestType()
{
    return requestType;
}

bool Request::isStateRequest()
{
    return requestType==WIFI_STATE_REQUEST;
}

bool Request::isChangeRequest()
{
    return requestType==WIFI_STATE_CHANGE;
}

void Request::setRequestType(const int newRequestType)
{
    this->requestType = newRequestType;
}
