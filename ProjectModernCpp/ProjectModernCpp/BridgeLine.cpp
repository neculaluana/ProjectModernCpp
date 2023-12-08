#include "BridgeLine.h"

BridgeLine::BridgeLine(CircleButton* startButton, CircleButton* endButton, QGraphicsItem* parent)
    : QGraphicsLineItem(parent), startButton(startButton), endButton(endButton)
{
    updatePosition();
}



void BridgeLine::updatePosition()
{
    if (startButton && endButton) {
        QLineF newLine(startButton->pos(), endButton->pos());
        setLine(newLine);
    }
    update();
}

QRectF BridgeLine::boundingRect() const 
{
    if (!startButton || !endButton) {
        return QRectF();
    }

    QPointF startPoint = startButton->pos();
    QPointF endPoint = endButton->pos();

    qreal left = std::min(startPoint.x(), endPoint.x());
    qreal right = std::max(startPoint.x(), endPoint.x());
    qreal top = std::min(startPoint.y(), endPoint.y());
    qreal bottom = std::max(startPoint.y(), endPoint.y());

    QRectF rect(left, top, right - left, bottom - top);
    qreal margin = 1.0;
    rect.adjust(-margin, -margin, margin, margin);
    return rect;
}

void BridgeLine::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) 
{
    if (!startButton || !endButton) return;

    QPen pen(Qt::black, 2); 
    painter->setPen(pen);

    painter->drawLine(line());
}