#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H
#pragma once
#include <QtWidgets>
#include <QPushButton>


class PushButton : public QPushButton
{
    Q_OBJECT
public:
    PushButton(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent*) override;
};

#endif // PUSHBUTTON_H
