#include "wifimanager.h"
#include "auth.h"

const QString Wifimanager::MIROUTERKEY = QString("a2ffa5c9be07488bbb04a3a47d3c5f6a");

Wifimanager::Wifimanager(Widget *parent) : QNetworkAccessManager(parent),
_widgetParent(parent)
{
    //icon
    qDebug() << "Supports Tray Icon Messages: " << _trayIcon->supportsMessages();
    changeIcon(WifiState::WifiOn);
    _trayIcon->show();
    _trayIcon->setVisible(true);
    _quitAction = new QAction("Выход", this);
    _createNewTimer = new QAction("+ Новый таймер", this);
    _trayIconMenu = new QMenu(_widgetParent);
    _trayIconMenu->addAction(_quitAction);
    _trayIconMenu->addAction(_createNewTimer);
    _trayIcon->setContextMenu(_trayIconMenu);
    connect(_trayIcon, &QSystemTrayIcon::activated, this, &Wifimanager::onTrayIconActivated);
    connect(_quitAction, &QAction::triggered, this, &Wifimanager::onQuitAction);
    connect(_createNewTimer, &QAction::triggered, this, &Wifimanager::onCreateNewTimer);
    connect(_trayIcon, &QSystemTrayIcon::messageClicked, this, &Wifimanager::onMessageClicked);

    //end icon

    QString add;
    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    for (const QHostAddress &address: QNetworkInterface::allAddresses())
    {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
        {
            qDebug() << address.toString();
            add = address.toString();
        }
    }

    loginToRouter();

    _req = new Request(QUrl(requestUrl), Request::WIFI_STATE_REQUEST);
    man = new QNetworkAccessManager();
    connect (man, SIGNAL(finished(QNetworkReply*)), this, SLOT(onStatusResult(QNetworkReply*)));
}

Wifimanager::~Wifimanager()
{
    _trayIcon->hide();
    delete _trayIcon;
}

void Wifimanager::setWidgetParent(Widget *widget)
{
    _widgetParent = widget;
}

bool Wifimanager::isLastWifiStateEquals(WifiState state)
{
    return _lastWifi2_4State==state;
}

int Wifimanager::getWifiState()
{
    return _wifi2_4State;
}

bool Wifimanager::isWifiOn()
{
    return _wifi2_4State == WifiState::WifiOn;
}

void Wifimanager::setWifiState(const WifiState state)
{
    _wifi2_4State = state;
}

void Wifimanager::setWifiState(bool isWifiOn)
{
    _wifi2_4State = isWifiOn ? WifiState::WifiOn : WifiState::WifiOff;
}

int Wifimanager::switchWifi(int index, bool state)
{
    switchingWifiStateNow = true;
    QString reqStr = _authinfo->hostname + "/cgi-bin/luci/;stok=";
    reqStr.append(getToken());
    reqStr.append("/api/xqnetwork/set_wifi?wifiIndex=");
    reqStr.append(QString::number(index));
    reqStr.append("&on=");
    reqStr.append(state ? "1" : "0");
    reqStr.append("&ssid=SweetHome&pwd=frlsUhga1737&encryption=mixed-psk&channel=0&bandwidth=0&hidden=0&txpwr=max");
    qDebug() << reqStr;
    _req->setUrl(QUrl(reqStr), Request::WIFI_STATE_CHANGE);
    man->get(*_req);
    if (_lastWifi2_4State!=WifiState::WifiError)
        showMessage(state ? "Wi-Fi Теперь ВКЛЮЧЕН." : "Wi-Fi Теперь ВЫКЛЮЧЕН.", state ? WifiState::WifiOn : WifiState::WifiOff);
    _lastWifi2_4State = state ? WifiState::WifiOn : WifiState::WifiOff;
    emit wifiStateChanged(state);

    return 0;
}

int Wifimanager::switch2_4Wifi(bool state)
{
    //2.4 GHz MiWiFi
    return switchWifi(Wifimanager::WifiIndex::G2_4, state);

}

int Wifimanager::switch5Wifi(bool state)
{
    //5 GHz MiWiFi
    return switchWifi(Wifimanager::WifiIndex::G5, state);
}

void Wifimanager::deactivateTrayIcon()
{
    _trayIcon->hide();
}

void Wifimanager::updateStateWifi()
{
    if (switchingWifiStateNow || !_authinfo->authentication) return;
    _req->setUrl(QUrl(requestUrl), Request::WIFI_STATE_REQUEST);
    //_req->setHeader(Request::CookieHeader, QVariant::fromValue(_cookieList));
    QString ua("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.114 YaBrowser/21.6.1.274 Yowser/2.5 Safari/537.36");
    _req->setHeader(Request::UserAgentHeader, QVariant(ua));
    _req->setHeader(Request::IfModifiedSinceHeader, QVariant("Sun, 04 Jan 1970 03:41:42 GMT"));
    changeIcon(WifiState::WifiBeep);
    man->get(*_req);
}

void Wifimanager::onStatusResult(QNetworkReply *reply)
{
    if (reply->error())
    {
        qDebug()<<"ERROR ";
        qDebug()<<reply->errorString();
        if(!isLastWifiStateEquals(WifiState::WifiError) || (_lastWifi5State == WifiState::WifiError))
        {
            _widgetParent->showErrLabel(true);
            showMessage("Нет доступа к роутеру.", WifiState::WifiError);
            _lastWifi2_4State = WifiState::WifiError;
            _lastWifi5State = WifiState::WifiError;
            _authinfo->authentication = false;
        }
        if (!_req->isStateRequest())
        {
            emit wifi2_4StateResult(false);
            emit wifi5StateResult(false);
        }
    }
    else
    {
        if (_req->isChangeRequest())
        {
            switchingWifiStateNow = false;
        }
        if (_req->isStateRequest())
        {
            QString responseText = reply->readAll();
            qDebug() << "ReadAll returns: " << responseText;
            if (responseText.indexOf("Invalid token")>=0)
            {
                _authinfo->authentication = false;
                loginToRouter();
                if (!isLastWifiStateEquals(WifiState::WifiError))
                    showMessage("Нет доступа к роутеру.", WifiState::WifiError);
                QTimer::singleShot(1000, this, &Wifimanager::updateStateWifi);
                return;
            }
            if (responseText.isEmpty() && !isLastWifiStateEquals(WifiState::WifiError))
            {
                _widgetParent->showErrLabel(true);
                showMessage("Нет доступа к роутеру.", WifiState::WifiError);
                _lastWifi2_4State = WifiState::WifiError;
                _lastWifi5State = WifiState::WifiError;
            }
            else
            {
                _widgetParent->showErrLabel(false);
                QChar wifiState = responseText[responseText.indexOf("status")+9];
                if (wifiState=='1')
                    _wifi2_4State = WifiState::WifiOn;
                else if (wifiState=='0')
                    _wifi2_4State = WifiState::WifiOff;

                wifiState = responseText[responseText.lastIndexOf("status")+9];
                if (wifiState=='1')
                    _wifi5State = WifiState::WifiOn;
                else if (wifiState=='0')
                    _wifi5State = WifiState::WifiOff;

                qDebug() << "Wi-fi 2.4 GHz is: " << _wifi2_4State;
                qDebug() << "Wi-fi 5 GHz is: " << _wifi5State;
                if (!switchingWifiStateNow)
                {
                    emit wifi5StateResult(_wifi5State == WifiState::WifiOn);
                    emit wifi2_4StateResult(_wifi2_4State == WifiState::WifiOn);
                }
                changeIcon(_wifi2_4State > _wifi5State ? _wifi2_4State : _wifi5State);
                if (!isLastWifiStateEquals(WifiState::WifiIsNotChecked) && _lastWifi2_4State!=_wifi2_4State)
                {
                    emit wifiStateUpdated();
                    showMessage(_wifi2_4State ? "Wi-Fi 2.4 GHz Теперь ВКЛЮЧЕН." : "Wi-Fi 2.4 GHz Теперь ВЫКЛЮЧЕН.", _wifi2_4State);
                    qDebug() << "\nWiFi изменил состояние\n";
                }

                if (!(_lastWifi5State == WifiState::WifiIsNotChecked) && _lastWifi2_4State!=_wifi2_4State)
                {
                    emit wifiStateUpdated();
                    showMessage(_wifi5State ? "Wi-Fi 5 GHz Теперь ВКЛЮЧЕН." : "Wi-Fi 5 GHz Теперь ВЫКЛЮЧЕН.", _wifi5State);
                    qDebug() << "\nWiFi изменил состояние\n";
                }

                _lastWifi2_4State = _wifi2_4State;
                _lastWifi5State = _wifi5State;
            }
        }
    }
}


//icon
void Wifimanager::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
        case QSystemTrayIcon::DoubleClick:
            _widgetParent->show();
            emit showedFromIcon();
            updateStateWifi();
            _widgetParent->setUpdateInterval(Widget::INTERVAL_ON_SHOWED);
            break;
        default:
            break;
    }
}

void Wifimanager::onQuitAction()
{
    _widgetParent->setCloseEventType(Widget::CONTEXT_MENU_CLOSE);
    _trayIcon->hide();
    _widgetParent->close();
}

void Wifimanager::onMessageClicked()
{
    if (isTimerWindowOwnsMessage)
    {
        QPoint lastPos;
        QMap<QAction*, OnTimeSwitcher*>::iterator it = _timerWindowsMap.begin();
        QMap<QAction*, OnTimeSwitcher*>::iterator end = _timerWindowsMap.end();
        for (;it!=end; ++it)
        {
            QPoint newPos;
            newPos.setX(lastPos.x()+100);
            newPos.setY(lastPos.y()+100);
            it.value()->move(newPos);
            it.value()->show();
            lastPos = it.value()->pos();
        }
    }
    else
        onTrayIconActivated(QSystemTrayIcon::DoubleClick);
}


void Wifimanager::onCreateNewTimer()
{
    OnTimeSwitcher *timerWindow = new OnTimeSwitcher();
    QAction *timerWindowShow = new QAction("Таймер " + QString::number(_timerWindowsMap.count()+1),this);
    timerWindow->setTrayIcon(_trayIcon);
    _timerWindowsMap.insert(timerWindowShow, timerWindow);
    connect(timerWindow, &OnTimeSwitcher::timeoutToSwitchWifi2_4, this, &Wifimanager::switch2_4Wifi);
    connect(timerWindow, &OnTimeSwitcher::timeoutToSwitchWifi5, this, &Wifimanager::switch5Wifi);
    connect(timerWindow, SIGNAL(deleteWindow(OnTimeSwitcher*)), this, SLOT(deleteTimerAction(OnTimeSwitcher*)));
    connect(timerWindowShow, &QAction::triggered, timerWindow, &OnTimeSwitcher::show);
    connect(timerWindow, &OnTimeSwitcher::trayIconUsed, this, [this](){
        isTimerWindowOwnsMessage = true;
    });
    _trayIconMenu->addAction(timerWindowShow);
    timerWindow->show();
}

void Wifimanager::deleteTimerAction(OnTimeSwitcher* window)
{
    QAction *action = _timerWindowsMap.key(window);
    _timerWindowsMap.remove(action);
    _trayIconMenu->removeAction(action);
    window->deleteLater();
    action->deleteLater();
}

bool Wifimanager::loginToRouter()
{
    QNetworkReply *loginReply;
    QNetworkRequest *loginRequest;
    QNetworkAccessManager *loginNetworkManager = new QNetworkAccessManager();
    try
    {
        AuthInfo authinfo = readConfig();
        _authinfo = new AuthInfo(authinfo);

        QString nonce = getNonce(getRandomMAC("e4:46:da"));
        QEventLoop loop;
        connect(loginNetworkManager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
        qDebug() << QDateTime::currentMSecsSinceEpoch();
        QString url1 = QString(_authinfo->hostname + "/cgi-bin/luci/api/xqsystem/login");
        QUrlQuery postData;
        postData.addQueryItem("username", _authinfo->username);
        postData.addQueryItem("password", getPasswordHash(nonce, QString(_authinfo->password)));
        postData.addQueryItem("logtype" ,"2");
        postData.addQueryItem("nonce", nonce);
        loginRequest = new QNetworkRequest(QUrl(url1));
        loginRequest->setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
        qDebug() << postData.toString(QUrl::FullyEncoded).toUtf8();
        loginReply = loginNetworkManager->post(*loginRequest, postData.toString(QUrl::FullyEncoded).toUtf8());
        loop.exec();
        if (loginReply->error() == QNetworkReply::NoError)
        {
            QString replyText = loginReply->readAll();
            qDebug() << "LOGIN DATA: " << replyText;
            _token = replyText.remove(0, replyText.indexOf("token")+8);
            _token = _token.remove(_token.indexOf("\""), _token.length());
            qDebug() << "TOKEN: " << _token;

            requestUrl = QString(_authinfo->hostname + "/cgi-bin/luci/;stok=" + getToken() + "/api/xqnetwork/wifi_detail_all");
            delete loginReply;
            delete loginRequest;
            delete loginNetworkManager;
            return true;
        }
        qDebug() << "LOGIN ERROR: " << loginReply->errorString();
        return false;
    }
    catch (...)
    {
        delete loginReply;
        delete loginRequest;
        delete loginNetworkManager;
        return false;
    }
}

QString Wifimanager::sha1(QString str)
{
    QByteArray bytes = str.toUtf8();
    return QString(QCryptographicHash::hash(bytes, QCryptographicHash::Sha1).toHex());
}

QString Wifimanager::getRandomMAC(QString prefix)
{
    QRandomGenerator rand(QTime::currentTime().second());
    for (int i=1; i<=3; i++)
        prefix = prefix + ":" + QString::number(rand.bounded(0, 15), 16) + QString::number(rand.bounded(0, 15), 16);
    return prefix;
}

QString Wifimanager::getPasswordHash(QString nonce, QString psw)
{
    return sha1(nonce + psw);
}

QString Wifimanager::getNonce(QString MACAddress)
{
    return "0_" + MACAddress + "_" + QString::number(QDateTime::currentMSecsSinceEpoch()) + "_9999";
}


void Wifimanager::showMessage(QString message, const WifiState icon)
{
    isTimerWindowOwnsMessage = false;
    QIcon new_icon = changeIcon(icon);
    _trayIcon->showMessage("Wi-Fi", message, new_icon);
}


QIcon Wifimanager::changeIcon(const WifiState icon)
{
    QString icon_addr;
    switch (icon)
    {
    case WifiState::WifiOff:
        icon_addr=":/img/wifi_off.ico";
        break;
    case WifiState::WifiError:
        icon_addr=":/img/wifi_err.ico";
        break;
    case WifiState::WifiOn:
        icon_addr=":/img/wifi_on.ico";
        break;
    case WifiState::WifiBeep:
        icon_addr=":/img/wifi_beep.ico";
        break;
    default:
        icon_addr=":/img/wifi_on.ico";
    }
    QIcon new_icon = QIcon(icon_addr);
    _trayIcon->setIcon(new_icon);
    _widgetParent->setWindowIcon(new_icon);
    return new_icon;
}

Wifimanager::AuthInfo Wifimanager::readConfig()
{
    if (!_config->open(QIODevice::ReadOnly))
    {
        Auth *auth = new Auth();
        QEventLoop loop;
        connect(auth, SIGNAL(closed()), &loop, SLOT(quit()));
        auth->show();
        loop.exec();
    }
    else
    {
        _config->close();
    }
    if (_config->open(QIODevice::ReadOnly))
    {
        QByteArray bytes = _config->readAll();
        _config->close();

        QJsonParseError jsonError;
        QJsonDocument document = QJsonDocument::fromJson( bytes, &jsonError );
        if ( jsonError.error != QJsonParseError::NoError )
        {
            qDebug() << "fromJson failed: " << jsonError.errorString() << "\n";
            AuthInfo authinfo;
            return  authinfo;
        }
        QJsonObject jsonObj = document.object();
        AuthInfo authinfo;
        authinfo.hostname = jsonObj.value("hostname").toString();
        authinfo.username = jsonObj.value("username").toString();
        authinfo.password = jsonObj.value("password").toString();
        authinfo.authentication = true;
        return authinfo;
    }
    else
    {
        qDebug() << "Error while reading config file\n";
        return AuthInfo();
    }
}
//icon end
