#ifndef WAC_H
#define WAC_H

#include <QObject>

#include "object.h"

class Wac : public QObject
{
    Q_OBJECT
    M_OBJECT
public:
    explicit Wac(QObject *parent = 0);
    ~Wac() {}
};

#endif // WAC_H
