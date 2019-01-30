#ifndef ORDERBY_H
#define ORDERBY_H

//namespace ORDERED_BY
//{
    enum ORDERED_BY{NAME, MOD_DATE, SIZE, TYPE};

    struct Order
    {
    public:
        Order();
        ORDERED_BY ordered_by;
        bool reversedOrdered;

        bool operator==(const Order& order)
        {
            return order.ordered_by      == this->ordered_by &&
                   order.reversedOrdered == this->reversedOrdered;
        }
        bool operator!=(const Order& order)
        {
            return order.ordered_by      != this->ordered_by ||
                   order.reversedOrdered != this->reversedOrdered;
        }
    };
//}

#endif // ORDERBY_H
