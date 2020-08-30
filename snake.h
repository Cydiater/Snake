#ifndef SNAKE_H
#define SNAKE_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QPainter>

#include "snakecore.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Snake; }
QT_END_NAMESPACE

class Snake : public QMainWindow {
    Q_OBJECT

public:
    Snake(QWidget *parent = nullptr);
    ~Snake();

private:

    enum ButtonIdx {b_start = 0, b_pause, b_restart, b_load, b_exit, b_save};
    const static bool isButtonEnable[4][6];
	static int timePerStep;

    Ui::Snake *ui;
	QTimer *timer_move;
	SnakeCore *core;

    void keyPressEvent(QKeyEvent *event);
	void changeSpeed(int speed);
	void updateButton();

private slots:

    void start();
    void move();
    void pause();
    void restart();
    void save();
    void load();
    void exit();
    void updateSpeed();
};

#endif // SNAKE_H
