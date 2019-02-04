#ifndef WIDGETCLOSER_H
#define WIDGETCLOSER_H

#include <QObject>
#include <QDebug>
#include <QWidget>

#include <vector>


//-----------------------------------------------------------------------

class WidgetCloser : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetCloser(QWidget* parent);
    virtual ~WidgetCloser();

signals:
    void registerWidget(WidgetCloser*);
    void widgetClosed_SGNL();
public slots:
    void closeWidget();

protected:
    bool m_closed;
};

//-----------------------------------------------------------------------

class WidgetCloserCoordinator : public QObject
{
    Q_OBJECT
public:
    explicit WidgetCloserCoordinator();
    virtual ~WidgetCloserCoordinator() override;

signals:
    void closeWidgets_SGNL();
public slots:
    void registerWidget(WidgetCloser* closer);

    void closeWidgets();

private:
};

extern WidgetCloserCoordinator* WIDGET_CLOSER_COORDINATOR;

#endif // WIDGETCLOSER_H
