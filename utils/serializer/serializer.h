#pragma once
#include <QtGui/QWidget>

class Serializer
{
public:
    static void serialize(QWidget const & widget);
    static void deserialize(QWidget * widget);
private:
    static QString getPath(QWidget const & widget);
};

