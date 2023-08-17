#pragma once
#include <QtWidgets>
#include <QDebug>

class Switch : public QAbstractButton {
    Q_OBJECT
    Q_PROPERTY(int offset READ offset WRITE setOffset)
    Q_PROPERTY(QBrush brush READ brush WRITE setBrush)

public:
    Switch(QWidget* parent = nullptr);
    Switch(const QBrush& brush, QWidget* parent = nullptr);
    ~Switch();

    void setCheck(bool);
    void enableTimeout(int);
    QSize sizeHint() const override;
    bool isChecked(){return _switch;}
    QBrush brush() const {
        return _brush;
    }
    void setBrush(const QBrush &brsh) {
        _brush = brsh;
    }
    int offset() const {
        return _x;
    }
    void setOffset(int o) {
        _x = o;
        update();
    }

signals:
    void mouseReleased(QMouseEvent *e);
    void switcherClicked(bool);

public slots:
    void onButtonReleased();

protected:
    void paintEvent(QPaintEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void enterEvent(QEvent*);

private:
    bool _switch;
    qreal _opacity;
    int _x, _y, _height, _margin;
    QBrush _thumb, _track, _brush;
    QPropertyAnimation *_anim = nullptr;
};
