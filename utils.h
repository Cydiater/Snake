#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <QPoint>
#include <QString>

enum Direction {Left = 0, Right, Up, Down};

class Coordinate : public QPoint {

    static const int dx[4];
    static const int dy[4];

public:

    int x, y;
    Coordinate() = delete;
    Coordinate(int _x, int _y);
    Coordinate(const QPoint &rhs);
    Coordinate(const QString &str);

    Coordinate(const Coordinate &rhs);
    Coordinate* operator=(const Coordinate &rhs);

    ~Coordinate() = default;

    bool operator<= (const Coordinate &rhs) const;
    bool operator==(const Coordinate &rhs) const;
    bool operator!=(const Coordinate &rhs) const;
    Coordinate operator-(const Coordinate &rhs) const;
    Coordinate operator-=(const Coordinate &rhs);

    Coordinate next(Direction dir) const;
    std::string toString() const;
    QString toQString() const;

    Direction calcDirection(const Coordinate &rhs) const;
};

#endif // UTILS_H
