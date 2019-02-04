#include "widgetcloser.h"

WidgetCloserCoordinator* WIDGET_CLOSER_COORDINATOR = new WidgetCloserCoordinator();

//-----------------------------------------------------------------------

WidgetCloser::WidgetCloser(QWidget *parent)
    : QWidget(parent)
{
    connect(this, &WidgetCloser::registerWidget, WIDGET_CLOSER_COORDINATOR, &WidgetCloserCoordinator::registerWidget);
    emit registerWidget(this);
}

WidgetCloser::~WidgetCloser()
{
}

void WidgetCloser::closeWidget()
{
//    qDebug() << "WidgetCloser::closeWidget";
    m_closed = true;
    emit widgetClosed_SGNL();
}


//-----------------------------------------------------------------------


WidgetCloserCoordinator::WidgetCloserCoordinator()
    : QObject(nullptr)
{
}

WidgetCloserCoordinator::~WidgetCloserCoordinator()
{
}

#include "window_coordinator/windowcoordinatorpane.h"

void WidgetCloserCoordinator::registerWidget(WidgetCloser* closer)
{
//    qDebug() << "registering Widget:";
    connect(this, &WidgetCloserCoordinator::closeWidgets_SGNL, closer, &WidgetCloser::closeWidget);
}

void WidgetCloserCoordinator::closeWidgets()
{
//    qDebug() << "WidgetCloserCoordinator::closeWidgets";
    emit closeWidgets_SGNL();
//    qDebug() << "   -> WidgetCloserCoordinator - Widgets closed";
}
