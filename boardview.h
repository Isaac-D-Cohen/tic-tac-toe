#pragma once

#include <QGraphicsItem>
#include <QPen>
#include "settings.h"

class BoardView : public QGraphicsItem
{
public:
    BoardView(settings *s, qreal width, qreal height);

    QRectF boundingRect() const;
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);

    QChar board[3][3];
    QPen computers_pen, humans_pen;

    QPoint top_left_corners[3][3];
    QSize square_size;

    settings *s;

    qreal width, height;
};
