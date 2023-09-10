#ifndef ONTIMESWITCHER_H
#define ONTIMESWITCHER_H

#include <QWidget>
#include <QGridLayout>
#include <QTimer>
#include "switch.h"
#include <QSpinBox>
#include <QSystemTrayIcon>


class OnTimeSwitcher : public QWidget
{
    Q_OBJECT
public:
    explicit OnTimeSwitcher(QWidget *parent = nullptr);
    void setTrayIcon(QSystemTrayIcon *trayIcon){_trayIcon = trayIcon;}

private:
    void init();
    bool isTimeToMessage(int, int, int);

    QSystemTrayIcon *_trayIcon = nullptr;
    QTimer *_timer = nullptr;
    QTime _onStartTime;
    Switch *_switcher2_4 = nullptr;
    Switch *_switcher5 = nullptr;
    QSpinBox *_hoursBox = nullptr;
    QSpinBox *_minutesBox = nullptr;
    QSpinBox *_secondsBox = nullptr;
    QCheckBox *_turnOffComputerFlag = nullptr;


signals:
    void deleteWindow(OnTimeSwitcher *);
    void timeoutToSwitchWifi2_4(bool);
    void timeoutToSwitchWifi5(bool);
    void trayIconUsed();

private slots:
    void onOkButton();
    void onCancelButton();
    void onSpinBoxesChanged();
    void onTimeout();


protected:
    void closeEvent(QCloseEvent *) override;

};

#endif // ONTIMESWITCHER_H
