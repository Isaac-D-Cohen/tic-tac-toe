#include "boardview.h"
#include <QPainter>

BoardView::BoardView(settings *s, qreal width, qreal height) {

    this->s = s;

    this->width = width;
    this->height = height;

    // for the example board
    // to understand these coordinates, look at the comment above MainWindow::paintEvent()
    top_left_corners[0][0] = QPoint(-150,-145);
    top_left_corners[0][1] = QPoint(-50,-145);
    top_left_corners[0][2] = QPoint(50,-145);
    top_left_corners[1][0] = QPoint(-150,-45);
    top_left_corners[1][1] = QPoint(-50,-45);
    top_left_corners[1][2] = QPoint(50,-45);
    top_left_corners[2][0] = QPoint(-150,55);
    top_left_corners[2][1] = QPoint(-50,55);
    top_left_corners[2][2] = QPoint(50,55);

    square_size = QSize(100, 100);

    // clear our example board
    for (int i=0; i<3; i++)
        for (int j=0; j<3; j++)
            board[i][j] = QChar::Null;
}

QRectF BoardView::boundingRect() const {
    return QRectF(0,0,width,height);
}

void BoardView::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget){

    QRectF rect = boundingRect();

    int w = rect.width(), h = rect.height();
    int shorter_side = w < h? w: h;

    painter->setRenderHint(QPainter::Antialiasing);
    painter->translate(w/2, h/2);
    painter->scale(shorter_side / 318.0, shorter_side / 318.0);
    painter->setPen(s->board_color);

    /* draw the board */

    // vertical lines
    painter->drawLine(-50,-145,-50,155);
    painter->drawLine(50,-145,50,155);

    // horizontal lines
    painter->drawLine(-150,-45,150,-45);
    painter->drawLine(-150,55,150,55);

    // put/update the symbols on our board
    board[0][0] = s->humans_symbol;
    board[1][1] = s->computers_symbol;
    board[2][2] = s->humans_symbol;

    computers_pen.setColor(s->computers_color);
    humans_pen.setColor(s->humans_color);

    // add the symbols
    for (int i=0; i<3; i++)
        for (int j=0; j<3; j++){
            if (board[i][j] == s->computers_symbol) {
                painter->setPen(computers_pen);
                painter->setFont(s->computers_font);
                painter->drawText(QRect(top_left_corners[i][j], square_size), Qt::AlignVCenter | Qt::AlignHCenter, QString(s->computers_symbol));
            }
            if (board[i][j] == s->humans_symbol){
                painter->setPen(humans_pen);
                painter->setFont(s->humans_font);
                painter->drawText(QRect(top_left_corners[i][j], square_size), Qt::AlignVCenter | Qt::AlignHCenter, QString(s->humans_symbol));
            }
        }
}
