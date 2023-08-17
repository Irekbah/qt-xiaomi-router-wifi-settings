#pragma once
#ifndef WIDGET_H
#define WIDGET_H
#include <QtWidgets>
#include <QObject>
#include "switch.h"
#include "pushbutton.h"
#include "wifimanager.h"
#include <QHBoxLayout>
#include <QLockFile>
#include <QDir>

class Wifimanager;

class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags(), bool hide = false, QHBoxLayout *layout = nullptr);
    static const int BUTTON_CLOSE = 0;
    static const int CONTEXT_MENU_CLOSE = 1;
    void setCloseEventType(const int);

    //timer
    static const int INTERVAL_ON_SILENCE = 17;
    static const int INTERVAL_ON_SHOWED = 5;
    void setUpdateInterval(int);
    void setLayoutPtr(QHBoxLayout *layout)
    {
        _layout = layout;
    }

    void showErrLabel(bool visibility){errLabel->setVisible(visibility);}


signals:
    void silentModeOn();

private slots:

    //timer
    void onSilentModeOn();


private:
    bool isOnButtonCloseEvent();
    int CloseEventType = BUTTON_CLOSE;
    PushButton *_backgroundButton;
    QPushButton *_createTimerButton;
    Wifimanager *_wifiManager;
    Switch *switcher2_4;
    Switch *switcher5;
    //mainwindow
    QTimer *_wifiRequestTimer;
    QHBoxLayout *_layout;
    QLockFile *_lockFile;
    PushButton *_timerButton;
    QLabel *errLabel;


protected:
    void closeEvent(QCloseEvent *) override;

};

#endif // WIDGET_H
