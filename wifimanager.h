#pragma once
#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H
#include <QNetworkAccessManager>
#include <QUrl>
#include <QtWidgets>
#include "request.h"
#include "widget.h"
#include "ontimeswitcher.h"
#include "auth.h"
#define ICON_ON 1
#define ICON_OFF 0
#define ICON_ERR -1

class Widget;

class Wifimanager : public QNetworkAccessManager
{
    Q_OBJECT
public:
    Wifimanager(Widget* parent = nullptr);
    ~Wifimanager();

    void setWidgetParent(Widget*);
    static const int WIFI_OFF = 0;
    static const int WIFI_ON = 1;
    static const int WIFI_IS_NOT_CHECKED = -2;
    static const int WIFI_ERR = -1;

    enum WifiState
    {
        WifiIsNotChecked = -2, WifiError, WifiOff, WifiOn, WifiBeep
    };
    static const QString MIROUTERKEY;
    QString requestUrl = "http://router.miwifi.com/cgi-bin/luci/;stok=/api/xqnetwork/wifi_detail_all";
    static const QString LOGIN;
    static const QString PASSWORD;
    bool isLastWifiStateEquals(WifiState);
    int getWifiState();
    bool isWifiOn();
    void setWifiState(const WifiState);
    void setWifiState(bool);
    void updateStateWifi();

    int switchWifi(int index, bool state);
    int switch2_4Wifi(bool);
    int switch5Wifi(bool);

    void deactivateTrayIcon();
    QString getToken() {return _token;};

signals:
    void wifi2_4StateResult(bool);
    void wifi5StateResult(bool);
    void wifiStateUpdated();
    void wifiStateChanged(bool);

    //icon
    void showedFromIcon();
public slots:
    void onCreateNewTimer();
    //

private slots:
    void onStatusResult(QNetworkReply*);

    //icon
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void onQuitAction();
    void onMessageClicked();
    void deleteTimerAction(OnTimeSwitcher*);


private:
    bool loginToRouter();
    WifiState _wifi2_4State = WifiState::WifiIsNotChecked;
    WifiState _wifi5State = WifiState::WifiIsNotChecked;
    const int _port = 8646;
    Widget *_widgetParent;
    WifiState _lastWifi2_4State = WifiState::WifiIsNotChecked;
    WifiState _lastWifi5State = WifiState::WifiIsNotChecked;
    Request *_req = new Request(QUrl(requestUrl), Request::WIFI_STATE_REQUEST);
    QList<QNetworkCookie> _cookieList;
    QNetworkAccessManager *man;

    QString sha1(QString str);
    QString getRandomMAC(QString);
    QString getPasswordHash(QString nonce, QString psw);
    QString getNonce(QString MACAddress);

    QString _token = "";

    bool switchingWifiStateNow = false;

    //icon
    void showMessage(QString, const WifiState);
    QIcon changeIcon(const WifiState);
    QSystemTrayIcon *_trayIcon = new QSystemTrayIcon(this);
    QMenu *_trayIconMenu;
    QAction *_quitAction;
    QAction *_createNewTimer;
    int _timerCount = 0;
    QMap <QAction*, OnTimeSwitcher*> _timerWindowsMap;
    bool isTimerWindowOwnsMessage = false;

    enum WifiIndex {
        G2_4 = 1,
        G5 = 2
    };

    QFile *_config = new QFile("auth.json");

    class AuthInfo
    {
    public:
        bool authentication = false;
        QString hostname = "192.168.0.1";
        QString username = "admin";
        QString password = "";
        AuthInfo(){}
        AuthInfo(AuthInfo const &authinfo): authentication(authinfo.authentication),
            hostname(authinfo.hostname), username(authinfo.username), password(authinfo.password){}
    };


    AuthInfo *_authinfo;
    AuthInfo readConfig();

};

#endif // WIFIMANAGER_H
