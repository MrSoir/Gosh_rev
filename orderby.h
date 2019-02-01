#ifndef ORDERBY_H
#define ORDERBY_H

#include <QString>

//namespace ORDERED_BY
//{
    enum ORDERED_BY{NAME, MOD_DATE, SIZE, TYPE};

    struct Order
    {
    public:
        Order();
        Order(const Order& order);
        Order& operator=(const Order& order);

        ORDERED_BY ordered_by;
        bool reversedOrdered;        

        bool operator==(const Order& order);
        bool operator!=(const Order& order);

        QString toString() const;
    };
//}

#endif // ORDERBY_H
