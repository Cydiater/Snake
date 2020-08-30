#ifndef SNAKEBOARD_H
#define SNAKEBOARD_H

#include <QWidget>
#include <QPainter>
#include <QColor>
#include <QString>

#include "snakecore.h"
#include "utils.h"

class SnakeBoard : public QWidget {
    Q_OBJECT
public:
    explicit SnakeBoard(QWidget *parent = nullptr);
	bool move();

	SnakeCore* getCore() const;
    SnakeCore* newCore();
    void replaceCore(SnakeCore *_core);

private:

	static int timePerStep; 
	const static int timePerRefresh;

    void setTopCoordinate();

    QPainter *painter;
    QTimer *timer_move, *timer_refresh;
    SnakeCore *core;
	int squareSideLength;
	int topX, topY, sideLength;

    const static QColor backgroundColor;
    const static QColor bodyHeadColor;
    const static QColor bodyColor;
    const static QColor barrierColor;
    const static QColor bonusColor;
    const static QColor selectColor;

    void colorGrid(int x, int y, const QColor &color);
    void selectGrid(int x, int y, const QColor &color);
    void colorBorder();
    Coordinate getCursorCoordinate() const;

	void paintEvent(QPaintEvent *);

protected:

    void mousePressEvent(QMouseEvent *event);

signals:

};

#endif // SNAKEBOARD_H
