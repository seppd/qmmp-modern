#ifndef QUERYLIST_H
#define QUERYLIST_H

#include "guiobject.h"

class QueryList : public GuiObject
{
    Q_OBJECT
public:
    explicit QueryList(QWidget *parent = 0);
    ~QueryList() {}
};

#endif // QUERYLIST_H
