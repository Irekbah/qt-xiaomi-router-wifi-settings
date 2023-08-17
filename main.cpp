#include "widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QHBoxLayout *layout = new QHBoxLayout();
//    layout->setAlignment(Qt::AlignCenter);
    layout->setDirection(QBoxLayout::LeftToRight);
    Widget *widget = new Widget(nullptr, Qt::WindowFlags(), true, layout);

    qDebug() << Qt::endl;
    qDebug() << argv[1];
    qDebug() << argc;
    qDebug() << Qt::endl;

    QString argument = QString::fromLatin1(argv[1]);
    if (argument == "-silent")
    {
        widget->setUpdateInterval(Widget::INTERVAL_ON_SILENCE);
    }
    else
    {
        widget->setUpdateInterval(Widget::INTERVAL_ON_SHOWED);
        widget->show();
        qDebug() << "SHOW WINDOW";
    }

    return a.exec();
}
