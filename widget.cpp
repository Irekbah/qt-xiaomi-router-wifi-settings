#include "widget.h"

Widget::Widget(QWidget *parent, Qt::WindowFlags f,bool hide, QHBoxLayout *layout) : QWidget(parent, f),
    _layout(layout)
{
    _wifiManager = new Wifimanager(this);
    _lockFile = new QLockFile(QDir::temp().absoluteFilePath("wifiswitcher.lock"));
    if (!_lockFile->tryLock(100))
    {
        setCloseEventType(CONTEXT_MENU_CLOSE);
        _wifiManager->deactivateTrayIcon();
        qDebug() << "\nЭКЗЕМПЛЯР ПРОГРАММЫ УЖЕ СОЗДАН.\n";
    }
    qDebug() << _lockFile->fileName();
    _gridLayout = new QGridLayout(this);
    _createTimerButton = new QPushButton(this);
    _createTimerButton->setText("+ Новый таймер");
    _createTimerButton->setVisible(true);
    _wifiRequestTimer = new QTimer(this);
    setUpdateInterval(hide ? INTERVAL_ON_SILENCE : INTERVAL_ON_SHOWED);

    this->setLayout(_gridLayout);

    switcher2_4 = new Switch(this);
    switcher2_4->setMaximumWidth(40);
    switcher5 = new Switch(this);
    switcher5->setMaximumWidth(40);

    errLabel = new QLabel("Нет доступа к роутеру");
    errLabel->setVisible(false);
    errLabel->setStyleSheet("QLabel { background-color : yellow; }");
    _gridLayout->addWidget(errLabel, 0, 0);
    _gridLayout->addItem(new QSpacerItem(10, 30), 0, 1);
    _gridLayout->addItem(new QSpacerItem(180, 10), 1, 0);
    _gridLayout->addWidget(new QLabel("2.4 GHz "), 1, 0);
    _gridLayout->addWidget(switcher2_4, 1, 1);
    _gridLayout->addItem(new QSpacerItem(10, 20), 2, 1);
    _gridLayout->addWidget(new QLabel("5 GHz "), 3, 0);
    _gridLayout->addWidget(switcher5, 3, 1);
    _gridLayout->addItem(new QSpacerItem(10, 50), 4, 1);
    _gridLayout->addWidget(_createTimerButton, 5, 0);

    //icon
    connect(_createTimerButton, &QPushButton::clicked, _wifiManager, &Wifimanager::onCreateNewTimer);

    switcher2_4->setCursor(Qt::PointingHandCursor);
    switcher5->setCursor(Qt::PointingHandCursor);

    connect(_wifiManager, &Wifimanager::wifi2_4StateResult, switcher2_4, &Switch::setCheck);
    connect(_wifiManager, &Wifimanager::wifi5StateResult, switcher5, &Switch::setCheck);
    connect(switcher2_4, &Switch::switcherClicked, _wifiManager, &Wifimanager::switch2_4Wifi);
    connect(switcher5, &Switch::switcherClicked, _wifiManager, &Wifimanager::switch5Wifi);
    connect(_wifiRequestTimer, &QTimer::timeout, _wifiManager, &Wifimanager::updateStateWifi);
    connect(this, SIGNAL(silentModeOn()), this, SLOT(onSilentModeOn()));
    _wifiRequestTimer->start();

    _wifiManager->updateStateWifi();
}

Widget::~Widget()
{
    delete switcher2_4;
    delete switcher5;
    delete _wifiManager;
    delete _lockFile;
    delete _createTimerButton;
    delete _wifiRequestTimer;
    _gridLayout->deleteLater();
    delete errLabel;
}

void Widget::setCloseEventType(const int eventType)
{
    //if (eventType == BUTTON_CLOSE || eventType == CONTEXT_MENU_CLOSE)
        CloseEventType = eventType;
//    else
//        throw std::runtime_error(std::to_string(eventType) + " event type not supported");
}

bool Widget::isOnButtonCloseEvent(){return CloseEventType==BUTTON_CLOSE;}

void Widget::closeEvent(QCloseEvent *event)
{
    if (isOnButtonCloseEvent())
    {
        emit silentModeOn();
        event->ignore();
        this->hide();
    }
    else
    {
        _wifiRequestTimer->stop();
        qDebug() << "\nВыход из приложения\n";
        event->accept();
    }
}

void Widget::setUpdateInterval(int interval)
{
    _wifiRequestTimer->setInterval(interval*1000);
    qDebug() << "\nInterval: " << interval << "\n";
}

void Widget::onSilentModeOn()
{
    setUpdateInterval(INTERVAL_ON_SILENCE);
}
