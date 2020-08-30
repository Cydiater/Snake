#include <stdexcept>

#include <QStringList>

#include "utils.h"

const int Coordinate::dx[4] = {-1, 1, 0, 0};
const int Coordinate::dy[4] = {0, 0, -1, 1};

Coordinate::Coordinate(int _x, int _y) :
    QPoint(_x, _y),
    x {_x},
    y {_y} { };

Coordinate Coordinate::next(Direction dir) const {
    return Coordinate( x + dx[dir],
                       y + dy[dir] );
}

Coordinate::Coordinate(const QString &str) {
    QString noBracket = str.mid(1, str.length() - 2);
    QStringList list = noBracket.split(QLatin1Char(','));
    x = list[0].trimmed().toInt();
    y = list[1].trimmed().toInt();
    setX(x);
    setY(y);
}

Coordinate::Coordinate(const Coordinate &rhs) : QPoint(rhs.x, rhs.y), x {rhs.x}, y {rhs.y} { };

Coordinate* Coordinate::operator=(const Coordinate &rhs) {
    x = rhs.x;
    y = rhs.y;
    return this;
}

Coordinate::Coordinate(const QPoint &rhs) :
    x {rhs.x()},
    y {rhs.y()} { };

std::string Coordinate::toString() const {
    return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
}

QString Coordinate::toQString() const {
    return QString(toString().c_str());
}

bool Coordinate::operator==(const Coordinate &rhs) const {
    return x == rhs.x && y == rhs.y;
}

bool Coordinate::operator<=(const Coordinate &rhs) const {
    return x <= rhs.x && y <= rhs.y;
}

bool Coordinate::operator!=(const Coordinate &rhs) const {
    return !(*this == rhs);
}

Coordinate Coordinate::operator-(const Coordinate &rhs) const {
    return Coordinate(x - rhs.x, y - rhs.y);
}

Coordinate Coordinate::operator-=(const Coordinate &rhs) {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

Direction Coordinate::calcDirection(const Coordinate &rhs) const {
    for (int i = 0; i < 4; i++) if (next(static_cast<Direction>(i)) == rhs)
        return static_cast<Direction>(i);
    throw std::runtime_error("bad direction");
}
