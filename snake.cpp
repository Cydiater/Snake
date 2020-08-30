#include "snake.h"
#include "ui_snake.h"

#include <iostream>
#include <QDebug>
#include <QMessageBox>
#include <QTimer>
#include <QKeyEvent>
#include <QFileDialog>
#include <QJsonDocument>
#include <QToolBar>

int Snake::timePerStep = 100;

const bool Snake::isButtonEnable[4][6] = {
    /* Origin */ {true, false, false, true, true, false},
    /* Running */ {false, true, false, false, true, false},
    /* Pause */ {false, true, true, true, true, true},
    /* Over */ {false, false, true, true, true, false}
};

Snake::Snake(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::Snake),
	  timer_move(new QTimer()) {

    ui -> setupUi(this);

	core = ui -> board -> getCore();

    connect(ui -> startButton, SIGNAL(clicked()),
            this, SLOT(start()));
    connect(ui -> pauseButton, SIGNAL(clicked()),
            this, SLOT(pause()));
    connect(ui -> restartButton, SIGNAL(clicked()),
            this, SLOT(restart()));
    connect(ui -> saveButton, SIGNAL(clicked()),
            this, SLOT(save()));
    connect(ui -> loadButton, SIGNAL(clicked()),
            this, SLOT(load()));
    connect(ui -> exitButton, SIGNAL(clicked()),
            this, SLOT(exit()));
    connect(ui -> speedSpinBox, SIGNAL(valueChanged(int)),
            this, SLOT(updateSpeed()));

    connect(timer_move, SIGNAL(timeout()),
            this, SLOT(move()));

	timer_move -> setInterval(timePerStep);

    ui -> speedSpinBox -> setValue( 1000 / timePerStep );

    updateButton();
}

void Snake::updateSpeed() {
    changeSpeed( ui -> speedSpinBox -> value() );
}

Snake::~Snake() {
    delete ui;
}

void Snake::start() {
    core -> start();
    timer_move -> start();

    updateButton();
}

void Snake::keyPressEvent(QKeyEvent *event) {
    switch (event -> key()) {
        case Qt::Key_A:
        case Qt::Key_Left:
            core -> changeDirection(Left);
            break;
        case Qt::Key_W:
        case Qt::Key_Up:
            core -> changeDirection(Up);
            break;
        case Qt::Key_S:
        case Qt::Key_Down:
            core -> changeDirection(Down);
            break;
        case Qt::Key_D:
        case Qt::Key_Right:
            core -> changeDirection(Right);
            break;
        default:
            qDebug() << "unvaild key";
    }
}

void Snake::move() {
	if (!core -> move()) {
        timer_move -> stop();
        QMessageBox::information(this, tr("Sorry"), tr("You are dead."));
		updateButton();
	}

    ui -> scoreLCD -> display(core -> getScore());
    ui -> timeLCD -> display(core -> getTime());
	ui -> board -> repaint();
}

void Snake::pause() {

    if (core -> getStatus() == SnakeCore::Running) {
        timer_move -> stop();
        core -> pause();
    } else if (core -> getStatus() == SnakeCore::Pause) {
        timer_move -> start();
        core -> continuee();
    }

    updateButton();
}

void Snake::restart() {
    assert(core -> getStatus() == SnakeCore::Pause ||
           core -> getStatus() == SnakeCore::Over);

    core = ui -> board -> newCore();
    start();
}

void Snake::updateButton() {
    ui -> startButton -> setEnabled( isButtonEnable[core -> getStatus()][b_start] );
    ui -> actionStart -> setEnabled( isButtonEnable[core -> getStatus()][b_start] );

    ui -> pauseButton -> setEnabled( isButtonEnable[core -> getStatus()][b_pause] );
    ui -> actionPause -> setEnabled( isButtonEnable[core -> getStatus()][b_pause] );

    ui -> restartButton -> setEnabled( isButtonEnable[core -> getStatus()][b_restart] );
    ui -> actionRestart -> setEnabled( isButtonEnable[core -> getStatus()][b_restart] );

    ui -> loadButton -> setEnabled( isButtonEnable[core -> getStatus()][b_load] );
    ui -> actionLoad -> setEnabled( isButtonEnable[core -> getStatus()][b_load] );

    ui -> exitButton -> setEnabled( isButtonEnable[core -> getStatus()][b_exit] );
    ui -> actionExit -> setEnabled( isButtonEnable[core -> getStatus()][b_exit] );

    ui -> saveButton -> setEnabled( isButtonEnable[core -> getStatus()][b_save] );
    ui -> actionSave -> setEnabled( isButtonEnable[core -> getStatus()][b_save] );

    if (core -> getStatus() == SnakeCore::Pause) {
        ui -> pauseButton -> setText("Continue");
        ui -> actionPause -> setText("Continue");
    } else {
        ui -> pauseButton -> setText("Pause");
        ui -> actionPause -> setText("Pause");
    }

    if (core -> getStatus() == SnakeCore::Pause ||
        core -> getStatus() == SnakeCore::Origin)
        ui -> speedSpinBox -> setEnabled(true);
    else
        ui -> speedSpinBox -> setEnabled(false);
}

void Snake::changeSpeed(int speed) {
    timePerStep = 1000 / speed;
    timer_move -> setInterval(timePerStep);
}

void Snake::save() {
    assert(core -> getStatus() == SnakeCore::Pause);

    QString json = core -> toJSON();
    QString filename = QFileDialog::getSaveFileName(this, tr("Save As"));

    QFile io(filename);

    if (!io.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Can not open file"), tr("Sorry, we can not open %1 .").arg(filename));
        return;
    }

    QTextStream stream(&io);
    stream << json;

    io.close();
}

void Snake::load() {
    assert(core -> getStatus() != SnakeCore::Running);

    QString filename = QFileDialog::getOpenFileName(this, tr("Load From"));

    if (filename.length() == 0)
        return;

    QFile io(filename);
    if (!io.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Can not open file"), tr("Sorry, we can not open %1 .").arg(filename));
        return;
    }

    QTextStream stream(&io);
    QString json = stream.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());

    if (doc.isNull()) {
        QMessageBox::warning(this, tr("Can not load archive"), tr("Null Archive file"));
        return;
    }

    if (!doc.isObject()) {
        QMessageBox::warning(this, tr("Can not load archive"), tr("Invaild Archive format"));
        return;
    }

    qDebug() << "json parse done";

    if (core != nullptr)
        delete core;

    core = new SnakeCore(doc.object());
    ui -> board -> replaceCore(core);
    ui -> scoreLCD -> display(core -> getScore());
    ui -> timeLCD -> display(core -> getTime());
    updateButton();
}

void Snake::exit() {
    QApplication::quit();
}
