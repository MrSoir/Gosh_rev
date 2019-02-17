#include "widgetcloser.h"

WidgetCloserCoordinator* WIDGET_CLOSER_COORDINATOR = new WidgetCloserCoordinator();

//-----------------------------------------------------------------------

WidgetCloser::WidgetCloser(QWidget *parent)
    : QWidget(parent),
      m_closed(false)
{
    connect(this, &WidgetCloser::registerWidget, WIDGET_CLOSER_COORDINATOR, &WidgetCloserCoordinator::registerWidget);
    emit registerWidget(this);
}

WidgetCloser::~WidgetCloser()
{
}

void WidgetCloser::closeWidget()
{
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

void WidgetCloserCoordinator::registerWidget(WidgetCloser* closer)
{
    connect(this, &WidgetCloserCoordinator::closeWidgets_SGNL, closer, &WidgetCloser::closeWidget);
}

void WidgetCloserCoordinator::closeWidgets()
{
    emit closeWidgets_SGNL();
}
