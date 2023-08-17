#ifndef AUTH_H
#define AUTH_H

#include <QWidget>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QFile>
#include <QTextStream>
#include <QCryptographicHash>
#include <QDebug>

namespace Ui {
class Auth;
}

class Auth : public QWidget
{
    Q_OBJECT

public:
    explicit Auth(QWidget *parent = nullptr);
    ~Auth();

private:
    QFile *_config = new QFile("auth.json");
    static const QString MIROUTERKEY;

    QString sha1(QString str);
    Ui::Auth *ui;

signals:
    void closed();

private slots:

    void onOkButton();
    void onCancelButton();
};

#endif // AUTH_H
