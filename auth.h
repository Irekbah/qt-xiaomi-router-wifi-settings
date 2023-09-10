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

#define CONFIG_FILE "auth.json"

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
