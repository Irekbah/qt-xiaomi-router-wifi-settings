#include "ontimeswitcher.h"

OnTimeSwitcher::OnTimeSwitcher(QWidget *parent) : QWidget(parent)
{
    init();

}

void OnTimeSwitcher::init()
{
    this->setWindowTitle("Таймер");
    QFormLayout *layout = new QFormLayout(this);

    this->setLayout(layout);
    _timer = new QTimer(this);
    _timer->setInterval(1000);
    _switcher2_4 = new Switch();
    _switcher2_4->setCursor(Qt::PointingHandCursor);
    _switcher5 = new Switch();
    _switcher5->setCursor(Qt::PointingHandCursor);
    _hoursBox = new QSpinBox();
    _minutesBox = new QSpinBox();
    _secondsBox = new QSpinBox();
    _turnOffComputerFlag = new QCheckBox(this);
    _turnOffComputerFlag->setChecked(false);

    _minutesBox->setMaximum(60);
    _secondsBox->setMaximum(60);
    _minutesBox->setMinimum(-1);
    _secondsBox->setMinimum(-1);
    _hoursBox->setMinimum(0);

    QPushButton *okButton = new QPushButton(this);
    okButton->setText("Ок");
    QPushButton *cancelButton = new QPushButton(this);
    cancelButton->setText("Отмена");

    layout->addRow(_switcher2_4, new QLabel("Выкл/Вкл 2.4 GHz Wi-Fi."));
    layout->addRow(_switcher5, new QLabel("Выкл/Вкл 5 GHz Wi-Fi."));
    layout->addRow(_turnOffComputerFlag, new QLabel("Выключить компьютер после."));
    layout->addRow(_hoursBox, new QLabel("Часов"));
    layout->addRow(_minutesBox, new QLabel("Минут"));
    layout->addRow(_secondsBox, new QLabel("Секунд"));
    layout->addRow(okButton, cancelButton);


    connect(okButton, &QPushButton::clicked, this, &OnTimeSwitcher::onOkButton);
    connect(cancelButton, &QPushButton::clicked, this, &OnTimeSwitcher::onCancelButton);
    connect(_minutesBox, &QSpinBox::valueChanged, this, &OnTimeSwitcher::onSpinBoxesChanged);
    connect(_secondsBox, &QSpinBox::valueChanged, this, &OnTimeSwitcher::onSpinBoxesChanged);
    connect(_timer, &QTimer::timeout, this, &OnTimeSwitcher::onTimeout);
}

bool OnTimeSwitcher::isTimeToMessage(int hours, int minutes, int seconds)
{
    if (hours==0 && minutes==5 && seconds==0)
    {
        return true;
    }
    if (hours==0 && minutes==1 && seconds==0)
    {
        return true;
    }
    if (_onStartTime.hour()==0 && _onStartTime.minute()<5)
    {
        if (minutes==0 && seconds==30)
            return true;
    }
    return false;
}

void OnTimeSwitcher::onOkButton()
{
    _onStartTime = QTime(_hoursBox->value(), _minutesBox->value(),
                         _secondsBox->value());
    _timer->start();
    this->hide();
}

void OnTimeSwitcher::onCancelButton()
{
    emit deleteWindow(this);
    this->close();
}

void OnTimeSwitcher::onSpinBoxesChanged()
{
    if (_secondsBox->value() == 60)
    {
        _secondsBox->setValue(0);
        _minutesBox->setValue(_minutesBox->value()+1);
    }
    if (_secondsBox->value() == -1)
    {
        if (_hoursBox->value()!=0 || _minutesBox->value()!=0)
            _secondsBox->setValue(59);
        else
            _secondsBox->setValue(0);
        _minutesBox->setValue(_minutesBox->value()-1);
    }

    if (_minutesBox->value() == 60)
    {
        _minutesBox->setValue(0);
        _hoursBox->setValue(_hoursBox->value()+1);
    }
    if (_minutesBox->value() == -1)
    {
        if (_hoursBox->value()!=0)
            _minutesBox->setValue(59);
        else
            _minutesBox->setValue(0);
        _hoursBox->setValue(_hoursBox->value()-1);
    }
}

void OnTimeSwitcher::onTimeout()
{
    _secondsBox->setValue(_secondsBox->value()-1);
    int hs = _hoursBox->value();
    int ms = _minutesBox->value();
    int ss = _secondsBox->value();
    if (hs==0 && ms==0 && ss==0)
    {
        _timer->stop();


        emit timeoutToSwitchWifi5(_switcher5->isChecked());
        emit timeoutToSwitchWifi2_4(_switcher2_4->isChecked());
        emit deleteWindow(this);

        QThread::sleep(5);
        if (_turnOffComputerFlag->isChecked())
            QProcess::startDetached("shutdown -s -f -t 00");

        this->close();
    }

    if (isTimeToMessage(hs, ms, ss))
    {
        if (_trayIcon!=nullptr)
        {
            emit trayIconUsed();
            QString word = _switcher2_4->isChecked() ? QString("включения") : QString("выключения");
            if (ms>1)
                _trayIcon->showMessage("Wi-Fi", "До " + word + " осталось "
                                    + QString::number(ms) + " минут.",
                                    QIcon(":/img/wifi_timer.ico"));
            else
                if (ms==1)
                    _trayIcon->showMessage("Wi-Fi", "До " + word + " осталось " + QString::number(ms) + " минута.",
                                           QIcon(":/img/wifi_timer.ico"));
                else
                    _trayIcon->showMessage("Wi-Fi", "До " + word + " осталось " + QString::number(ss) + " секунд.",
                                           QIcon(":/img/wifi_timer.ico"));
        }
    }
}

void OnTimeSwitcher::closeEvent(QCloseEvent *event)
{
//    if (_timer->isActive())
//    {
        event->ignore();
        this->hide();
//    }
//    else
//        event->accept();
}
