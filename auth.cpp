#include "auth.h"
#include "ui_auth.h"

const QString Auth::MIROUTERKEY = QString("a2ffa5c9be07488bbb04a3a47d3c5f6a");

Auth::Auth(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Auth)
{
    ui->setupUi(this);
    connect(this->ui->okButton, SIGNAL(clicked()), this, SLOT(onOkButton()));
    connect(this->ui->cancelButton, SIGNAL(clicked()), this, SLOT(onCancelButton()));

}

Auth::~Auth()
{
    delete ui;
}

void Auth::onOkButton()
{
    QJsonObject content;
    QJsonValue hostname(ui->lineEdit->text());
    QJsonValue username(ui->lineEdit_2->text());
    QJsonValue password(sha1(ui->lineEdit_3->text()+MIROUTERKEY));
    content.insert("hostname", hostname);
    content.insert("username", username);
    content.insert("password", password);
    QJsonDocument authJson;
    authJson.setObject(content);
    QByteArray authJsonBytes = authJson.toJson(QJsonDocument::Indented);
    if( _config->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate) )
    {
        QTextStream iStream( _config );
        iStream << authJsonBytes;
        _config->close();
    }
    else
    {
        qDebug() << "Error while opening auth.json file for writing\n";
    }
    emit closed();
    this->close();
}

void Auth::onCancelButton()
{
    emit closed();
    this->close();
}

QString Auth::sha1(QString str)
{
    QByteArray bytes = str.toUtf8();
    return QString(QCryptographicHash::hash(bytes, QCryptographicHash::Sha1).toHex());
}

//sha1(psw+MIROUTERKEY)
