#include "projects_tool_bar.h"

#include <ui/design_system/design_system.h>

#include <QAction>


namespace Ui
{

ProjectsToolBar::ProjectsToolBar(QWidget* _parent)
    : AppBar(_parent)
{
    QAction* menuAction = new QAction(this);
    menuAction->setText("\uf35c");
    addAction(menuAction);
    connect(menuAction, &QAction::triggered, this, &ProjectsToolBar::menuPressed);

    designSystemChangeEvent(nullptr);
}

void ProjectsToolBar::designSystemChangeEvent(DesignSystemChangeEvent* _event)
{
    AppBar::designSystemChangeEvent(_event);

    setBackgroundColor(DesignSystem::color().primary());
    setTextColor(DesignSystem::color().onPrimary());
}

} // namespace Ui
