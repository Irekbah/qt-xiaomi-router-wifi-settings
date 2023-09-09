#pragma once
#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H
#include <QNetworkAccessManager>
#include <QUrl>
#include <QtWidgets>
#include "request.h"
#include "widget.h"
#include "ontimeswitcher.h"
#include "networkmanager.h"
#define ICON_ON 1
#define ICON_OFF 0
#define ICON_ERR -1
#define CONFIG_FILE "auth.json"

class Widget;

class Wifimanager : public QNetworkAccessManager
{
    Q_OBJECT
public:
    Wifimanager(Widget* parent = nullptr);
    ~Wifimanager();

    void setWidgetParent(Widget*);

    struct WifiInfo
    {
        bool G2_4 = false;
        bool G5 = false;
        bool error = false;
    };

    enum WifiState
    {
        WifiIsNotChecked = -2, WifiError, WifiOff, WifiOn, WifiBeep
    };
    static const QString MIROUTERKEY;
    QString requestUrl = "http://router.miwifi.com/cgi-bin/luci/;stok=/api/xqnetwork/wifi_detail_all";

    bool isLastWifiStateEquals(WifiState);
    WifiInfo updateStateWifi();

    int switchWifi(int index, bool state);
    int switch2_4Wifi(bool);
    int switch5Wifi(bool);

    void deactivateTrayIcon();

signals:
    void wifi2_4StateResult(bool);
    void wifi5StateResult(bool);

public slots:
    void onCreateNewTimer();
    //

private slots:

    //icon
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void onQuitAction();
    void onMessageClicked();
    void deleteTimerAction(OnTimeSwitcher*);


private:
    void routerError();
    bool loginToRouter();
    WifiState _wifi2_4State = WifiState::WifiIsNotChecked;
    WifiState _wifi5State = WifiState::WifiIsNotChecked;
    Widget *_widgetParent = nullptr;
    WifiState _lastWifi2_4State = WifiState::WifiIsNotChecked;
    WifiState _lastWifi5State = WifiState::WifiIsNotChecked;
    Request *_req = nullptr;
    NetworkManager *man = nullptr;

    QString sha1(QString str);
    QString getRandomMAC(QString);
    QString getPasswordHash(QString nonce, QString psw);
    QString getNonce(QString MACAddress);
    QString askSSID();

    QString _token = "";
    QString _ssid[2];

    bool switchWifiStateProcessing = false;

    //icon
    void showMessage(QString, const WifiState);
    QIcon changeIcon(const WifiState);
    QSystemTrayIcon *_trayIcon = new QSystemTrayIcon(this);
    QMenu *_trayIconMenu = nullptr;
    QAction *_quitAction = nullptr;
    QAction *_createNewTimer = nullptr;
    QMap <QAction*, OnTimeSwitcher*> _timerWindowsMap;
    bool isTimerWindowOwnsMessage = false;

    enum WifiIndex {
        G2_4 = 1,
        G5 = 2
    };

    QFile *_config = new QFile(CONFIG_FILE);

    class AuthInfo
    {
    public:
        bool authentication = false;
        QString hostname = "http://router.miwifi.com";
        QString username = "admin";
        QString password = "";
        AuthInfo(){}
        AuthInfo(AuthInfo const &authinfo): authentication(authinfo.authentication),
            hostname(authinfo.hostname), username(authinfo.username), password(authinfo.password){}
        ~AuthInfo(){}
    };


    AuthInfo *_authinfo = new AuthInfo();
    AuthInfo readConfig();

};

#endif // WIFIMANAGER_H
