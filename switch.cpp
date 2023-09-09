#include "switch.h"

Switch::Switch(QWidget *parent) : QAbstractButton(nullptr),
_height(16),
_opacity(0.000),
_switch(false),
_margin(3),
_thumb("#D5D5D5"),
_anim(animation)
{
    _anim->setDuration(120);
    setOffset(_height / 2);
    _y = _height / 2;
    setBrush(QColor("#009688"));
}

Switch::Switch(const QBrush &brush, QWidget *parent) : QAbstractButton(parent),
_height(16),
_switch(false),
_opacity(0.000),
_margin(3),
_thumb("#D5D5D5"),
_anim(animation)
{
    setOffset(_height / 2);
    _y = _height / 2;
    setBrush(brush);
}

Switch::~Switch()
{}

void Switch::setCheck(bool state)
{
    bool animate = true;
    if (_switch==state) animate = false;
        _switch = state;
        _thumb = state ? _brush : QBrush("#D5D5D5");
        if (state) {
            _anim->setStartValue(_height / 2);
            _anim->setEndValue(width() - _height);
        } else {
            _anim->setStartValue(offset());
            _anim->setEndValue(_height / 2);
        }
        if (!animate) _anim->setDuration(0);
        else _anim->setDuration(120);
        _anim->start();
}

void Switch::enableTimeout(int timeout_milisec)
{
    this->setEnabled(false);
    QTimer::singleShot(timeout_milisec, this, SLOT(setEnabled(true)));
}

void Switch::paintEvent(QPaintEvent *e) {
    QPainter p(this);
    p.setPen(Qt::NoPen);
    if (isEnabled()) {
        p.setBrush(_switch ? brush() : Qt::black);
        p.setOpacity(_switch ? 0.5 : 0.38);
        p.setRenderHint(QPainter::Antialiasing, true);
        p.drawRoundedRect(QRect(_margin, _margin, width() - 2 * _margin, height() - 2 * _margin), 8.0, 8.0);
        p.setBrush(_thumb);
        p.setOpacity(1.0);
        p.drawEllipse(QRectF(offset() - (_height / 2), _y - (_height / 2), height(), height()));
    } else {
        p.setBrush(Qt::black);
        p.setOpacity(0.12);
        p.drawRoundedRect(QRect(_margin, _margin, width() - 2 * _margin, height() - 2 * _margin), 8.0, 8.0);
        p.setOpacity(1.0);
        p.setBrush(QColor("#BDBDBD"));
        p.drawEllipse(QRectF(offset() - (_height / 2), _y - (_height / 2), height(), height()));
    }
    e->ignore();
}

void Switch::mouseReleaseEvent(QMouseEvent *e) {
    if (e->button() & Qt::LeftButton) {
        _switch = !_switch;
        setCheck(_switch);
        emit switcherClicked(_switch);
    }
    QAbstractButton::mouseReleaseEvent(e);
    e->ignore();
}

void Switch::enterEvent(QEnterEvent *e) {
    setCursor(Qt::PointingHandCursor);
    //QAbstractButton::event(nullptr);
    e->ignore();
}

QSize Switch::sizeHint() const {
    return QSize(2 * (_height + _margin), _height + 2 * _margin);
}
