#include "snakecore.h"

#include <QRandomGenerator>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

const int SnakeCore::dx[4] = {0, 1, 0, -1};
const int SnakeCore::dy[4] = {-1, 0, 1, 0};

int SnakeCore::lrand(int l, int r) const {
    return QRandomGenerator::global() -> bounded(l, r);
}

int SnakeCore::getRandX() const {
    return lrand(0, width - 1);
}

int SnakeCore::getRandY() const {
    return lrand(0, height - 1);
}

Coordinate SnakeCore::getRandXY() const {
    return Coordinate(getRandX(), getRandY());
}

Coordinate SnakeCore::getSafeXY() const {
    int d_width = width / 5, d_height = height / 5;
    return Coordinate( 	lrand(d_width, width - d_height),
                        lrand(d_height, height - d_height));
}

SnakeCore::GameStatus SnakeCore::getStatus() const {
    return status;
}

SnakeCore::iterator SnakeCore::begin() {
    return body.begin();
}

SnakeCore::iterator SnakeCore::end() {
    return body.end();
}

SnakeCore::iterator SnakeCore::barrier_begin() {
    return barriers.begin();
}

SnakeCore::iterator SnakeCore::barrier_end() {
    return barriers.end();
}

void SnakeCore::init() {
    body.push_back( getSafeXY() );
    body.push_back( body.back().next(static_cast<Direction>(lrand(0, 3))) );
    direction = body.back().calcDirection(body.front());
    genBonus();
}

SnakeCore::SnakeCore(int _width, int _height) :
    width {_width},
    height {_height},
    timeFromStart {0},
    bonusCnt {0},
    status {Origin},
    bonus {-1, -1} {
    init();
};

SnakeCore::SnakeCore() :
    timeFromStart {0},
    bonusCnt {0},
    status {Origin},
    bonus {-1, -1} {
    init();
};

SnakeCore::SnakeCore(const QJsonObject &obj) :
    width {obj["width"].toInt()},
    height {obj["height"].toInt()},
    timeFromStart {obj["timeFromStart"].toInt()},
    bonusCnt {obj["bonusCnt"].toInt()},
    status {static_cast<GameStatus>(obj["status"].toInt())},
    direction {static_cast<Direction>(obj["direction"].toInt())},
    bonus {Coordinate(obj["bonus"].toString())} {

    QJsonArray json_body = obj["body"].toArray();
    for (auto chunk: json_body)
        body.push_back(Coordinate(chunk.toString()));

    QJsonArray json_barriers = obj["barriers"].toArray();
    for (auto chunk: json_barriers)
        barriers.push_back(Coordinate(chunk.toString()));
}

int SnakeCore::getWidth() const {
	return width;
}

int SnakeCore::getHeight() const {
	return height;
}

std::string SnakeCore::getBodyInfo() const {
    std::string info;
    for (auto &chunk: body)
        info += chunk.toString() + " - ";
    info.pop_back();
    info.pop_back();
    info.pop_back();
    return info;
}

bool SnakeCore::move() {
    assert(status == Running);

    qDebug() << "snakecore moving";

    updateDirection();

    int length = static_cast<int>(body.size());

    if (!inBoard(body.front().next(direction))) {
        status = Over;
        return false;
    }

    if (inBarrier(body.front().next(direction))) {
        status = Over;
        return false;
    }

    if (inBody(body.front().next(direction))) {
        status = Over;
        return false;
    }

    timeFromStart++;

    if (body.front().next(direction) == bonus) {
        bonusCnt += 3;
        genBonus();
    }

    Coordinate tmp = body.back();
    for (int i = length - 1; i > 0; i--)
        body[i] = body[i - 1];
    body.front() = body.front().next(direction);

    if (bonusCnt > 0) {
        body.push_back(tmp);
        bonusCnt--;
    }

    return true;
}

bool SnakeCore::inBoard(const Coordinate &chunk) {
    return Coordinate(0, 0) <= chunk && chunk <= Coordinate(width - 1, height - 1);
}

void SnakeCore::start() {
    status = Running;
}

bool SnakeCore::inBody(const Coordinate &chunk) const {
    return std::find(body.begin(), body.end(), chunk) != body.end();
}

bool SnakeCore::inBarrier(const Coordinate &chunk) const {
    return std::find(barriers.begin(), barriers.end(), chunk) != barriers.end();
}

bool SnakeCore::genBonus() {
    int genCnt = 30;

    do {
        bonus = getSafeXY();
        genCnt--;
    } while (inBody(bonus) && genCnt > 0);

    return !inBody(bonus);
}

QString SnakeCore::toJSON() const {
    assert(status == Pause);

    QJsonObject obj;
    QJsonArray json_body, json_barriers;
    for (auto &chunk: body)
        json_body.push_back(chunk.toQString());

    for (auto &chunk: barriers)
        json_barriers.push_back(chunk.toQString());

    obj.insert("width", width);
    obj.insert("height", height);
    obj.insert("body", json_body);
    obj.insert("barriers", json_barriers);
    obj.insert("bonus", bonus.toQString());
    obj.insert("timeFromStart", timeFromStart);
    obj.insert("direction", direction);
    obj.insert("status", status);
    obj.insert("bonusCnt", bonusCnt);

    QJsonDocument doc(obj);
    return QString(doc.toJson(QJsonDocument::Compact).toStdString().c_str());
}

void SnakeCore::changeDirection(Direction _direction) {
    dir_queue.push(_direction);
}

void SnakeCore::updateDirection() {
    while (!dir_queue.empty()) {
        Direction _direction = dir_queue.front();
        dir_queue.pop();
        if ((static_cast<int>(direction) ^ 1) == static_cast<int>(_direction))
            continue;
        direction = _direction;
        break;
    }
}

void SnakeCore::pause() {
    status = Pause;
}

int SnakeCore::getTime() const {
    return timeFromStart;
}

int SnakeCore::getScore() const {
    return static_cast<int>(body.size());
}

void SnakeCore::continuee() {
    status = Running;
}

void SnakeCore::over() {
    status = Over;
}

void SnakeCore::clear() {
    status = Origin;
    body.clear();
    barriers.clear();
	timeFromStart = 0;
    bonus = Coordinate(-1, -1);
}

void SnakeCore::addBarrier(const Coordinate &barrier) {
    barriers.push_back(barrier);
}

void SnakeCore::eraseBarrier(const Coordinate &barrier) {
    barriers.erase(std::find(barriers.begin(), barriers.end(), barrier));
}

Coordinate SnakeCore::getBonus() const {
    return bonus;
}
