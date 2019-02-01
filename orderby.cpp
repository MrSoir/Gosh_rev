#include "orderby.h"


Order::Order()
    : ordered_by(ORDERED_BY::NAME),
      reversedOrdered(false)
{
}

Order::Order(const Order& order)
    : ordered_by(order.ordered_by),
      reversedOrdered(order.reversedOrdered)
{
}

Order& Order::operator=(const Order& order)
{
    this->ordered_by = order.ordered_by;
    this->reversedOrdered = order.reversedOrdered;
    return *this;
}

bool Order::operator==(const Order &order)
{
    return order.ordered_by      == this->ordered_by &&
           order.reversedOrdered == this->reversedOrdered;
}

bool Order::operator!=(const Order &order)
{
    return order.ordered_by      != this->ordered_by ||
           order.reversedOrdered != this->reversedOrdered;
}

QString Order::toString() const
{
    QString order_str;
    if(this->ordered_by == ORDERED_BY::NAME)
        order_str = "NAME";
    else if(this->ordered_by == ORDERED_BY::MOD_DATE)
        order_str = "MOD_DATE";
    else if(this->ordered_by == ORDERED_BY::SIZE)
        order_str = "SIZE";
    else if(this->ordered_by == ORDERED_BY::TYPE)
        order_str = "TYPE";

    QString reverse_str = reversedOrdered ? "true" : "false";

    return QString("Order: %1 - reverse: %2").arg(order_str).arg(reverse_str);
}
