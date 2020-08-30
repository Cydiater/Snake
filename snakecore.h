#ifndef SNAKECORE_H
#define SNAKECORE_H

#include <vector>
#include <chrono>
#include <random>
#include <queue>

#include <QJsonObject>

#include "utils.h"

class SnakeCore {

public:
    enum GameStatus {Origin, Running, Pause, Over};

private:
    int width, height;
    std::vector<Coordinate> body;
    std::vector<Coordinate> barriers;
    int timeFromStart, bonusCnt;

    GameStatus status;
    Direction direction;

    Coordinate bonus;

    std::queue<Direction> dir_queue;

    const static int dx[4];
    const static int dy[4];

    int lrand(int l, int r) const;

    void init();
    int getRandX() const;
    int getRandY() const;
    Coordinate getRandXY() const;
    Coordinate getSafeXY() const;

    bool inBody(const Coordinate &chunk) const;
    bool genBonus();
    void updateDirection();

public:
    SnakeCore();
    SnakeCore(int _width, int _height);
    SnakeCore(const QJsonObject &obj);

    ~SnakeCore() = default;

    typedef std::vector<Coordinate>::iterator iterator;

	int getWidth() const;
	int getHeight() const;

    std::string getBodyInfo() const;

    bool inBarrier(const Coordinate &chunk) const;
    bool inBoard(const Coordinate &chunk);

    void start();
    bool move();
    void pause();
    void continuee(); // -ee for keyword
    void over();
    void clear();

    int getTime() const;
    int getScore() const;

    QString toJSON() const;

    void changeDirection(Direction _direction);

    iterator begin();
    iterator end();

    iterator barrier_begin();
    iterator barrier_end();

    GameStatus getStatus() const;
    Coordinate getBonus() const;
    void addBarrier(const Coordinate &barrier);
    void eraseBarrier(const Coordinate &barrier);
};

#endif // SNAKECORE_H
