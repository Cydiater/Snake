#include <QMouseEvent>
#include <QTimer>

#include "snakeboard.h"

const QColor SnakeBoard::barrierColor = QColor("#202124");
const QColor SnakeBoard::bodyColor = QColor("#B5B6B7");
const QColor SnakeBoard::bodyHeadColor = QColor("#EEEEEE");
const QColor SnakeBoard::backgroundColor = QColor("#28292A");
const QColor SnakeBoard::bonusColor = QColor("#F4B458");
const QColor SnakeBoard::selectColor = QColor("#000000");

const int SnakeBoard::timePerRefresh = 50;
const int boardSideLength = 40;

SnakeBoard::SnakeBoard(QWidget *parent) : QWidget(parent),
										  painter(nullptr),
										  timer_refresh {new QTimer()},
										  core {new SnakeCore(boardSideLength, boardSideLength)} {

	connect(timer_refresh, SIGNAL(timeout()),
			this, SLOT(repaint()));

    timer_refresh -> setInterval(timePerRefresh);
    timer_refresh -> start();
}

SnakeCore* SnakeBoard::getCore() const {
	return core;
}

void SnakeBoard::colorGrid(int x, int y, const QColor &color) {
    QRect rect(x * squareSideLength, y * squareSideLength, squareSideLength, squareSideLength);
    painter -> fillRect(rect, QBrush(color));
}

void SnakeBoard::selectGrid(int x, int y, const QColor &color) {
    QRect rect(x * squareSideLength, y * squareSideLength, squareSideLength, squareSideLength);
    painter -> setPen(color);
    painter -> drawRect(rect);
}

void SnakeBoard::colorBorder() {
    int blockWidth = core -> getWidth(),
        blockHeight = core -> getHeight();

    for (int x = -1, y = -1; x <= blockWidth; x++) colorGrid(x, y, barrierColor);
    for (int x = blockWidth, y = -1; y <= blockHeight; y++) colorGrid(x, y, barrierColor);
    for (int x = blockWidth, y = blockHeight; x >= -1; x--) colorGrid(x, y, barrierColor);
    for (int x = -1, y = blockHeight; y >= -1; y--) colorGrid(x, y, barrierColor);
}

Coordinate SnakeBoard::getCursorCoordinate() const {
    Coordinate cursor = Coordinate(mapFromGlobal(QCursor::pos()));
    cursor -= Coordinate(topX, topY);
    cursor.x = cursor.x / squareSideLength;
    cursor.y = cursor.y / squareSideLength;
    return cursor;
}

void SnakeBoard::setTopCoordinate() {
	int fullSideLength = std::min(width(), height());
	squareSideLength = fullSideLength / (boardSideLength + 4);

	topX = (width() - sideLength) / 2 + squareSideLength;
	topY = (height() - sideLength) / 2 + squareSideLength;

	sideLength = squareSideLength * boardSideLength;
	qDebug() << width() << " " << height();
}

void SnakeBoard::mousePressEvent(QMouseEvent *event) {

    if (core -> getStatus() != SnakeCore::Pause &&
        core -> getStatus() != SnakeCore::Origin)
        return;

    if (event -> button() != Qt::LeftButton)
        return;

    qDebug() << "mouse press";

    Coordinate cursor = getCursorCoordinate();

    if (!core -> inBoard(cursor))
        return;

    if (!core -> inBarrier(cursor))
        core -> addBarrier(cursor);
    else
        core -> eraseBarrier(cursor);
}

void SnakeBoard::paintEvent(QPaintEvent *) {

    setTopCoordinate();
    painter = new QPainter(this);

    painter -> translate(QPointF(topX, topY));
    painter -> fillRect(QRect(0, 0, sideLength, sideLength), QBrush(backgroundColor));

    colorBorder();

    if (core != nullptr) {
        for (auto &chunk: *core) {
            colorGrid(chunk.x, chunk.y,
                      chunk == *(core -> begin()) ?
                          bodyHeadColor : bodyColor);
        }
        Coordinate bonus = core -> getBonus(), cursor = getCursorCoordinate();
        if (bonus != Coordinate(-1, -1)) {
            colorGrid(bonus.x, bonus.y, bonusColor);
        }

        if (core -> inBoard(cursor)) {
            selectGrid(cursor.x, cursor.y, selectColor);
        }

        for (auto it = core -> barrier_begin(); it != core -> barrier_end(); it++)
            colorGrid(it -> x, it -> y, barrierColor);
    }

    delete painter;
}

SnakeCore* SnakeBoard::newCore() {
    delete core;
    core = new SnakeCore(boardSideLength, boardSideLength);
    return core;
}

void SnakeBoard::replaceCore(SnakeCore *_core) {
    core = _core;
}
