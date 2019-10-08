#include "application_view.h"

#include <ui/design_system/design_system.h>
#include <ui/widgets/shadow/shadow.h>
#include <ui/widgets/splitter/splitter.h>
#include <ui/widgets/stack_widget/stack_widget.h>

#include <QCloseEvent>
#include <QPainter>
#include <QVBoxLayout>


namespace Ui
{

namespace {
    const QString kSplitterState = "splitter/state";
    const QString kViewGeometry = "view/geometry";
}

class ApplicationView::Implementation
{
public:
    explicit Implementation(QWidget* _parent);

    Widget* navigationWidget = nullptr;
    StackWidget* toolBar = nullptr;
    StackWidget* navigator = nullptr;
    StackWidget* view = nullptr;

    Splitter* splitter = nullptr;
    Shadow* splitterShadow = nullptr;
};

ApplicationView::Implementation::Implementation(QWidget* _parent)
    : navigationWidget(new Widget(_parent)),
      toolBar(new StackWidget(_parent)),
      navigator(new StackWidget(_parent)),
      view(new StackWidget(_parent)),
      splitter(new Splitter(_parent)),
      splitterShadow(new Shadow(view))
{
}


// ****


ApplicationView::ApplicationView(QWidget* _parent)
    : Widget(_parent),
      d(new Implementation(this))
{
    QVBoxLayout* navigationLayout = new QVBoxLayout(d->navigationWidget);
    navigationLayout->setContentsMargins({});
    navigationLayout->setSpacing(0);
    navigationLayout->addWidget(d->toolBar);
    navigationLayout->addWidget(d->navigator);

    d->splitter->addWidget(d->navigationWidget);
    d->splitter->addWidget(d->view);
    d->splitter->setSizes({2, 7});

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins({});
    layout->setSpacing(0);
    layout->addWidget(d->splitter);

    designSystemChangeEvent(nullptr);
}

QVariantMap ApplicationView::saveState() const
{
    QVariantMap state;
    state[kSplitterState] = d->splitter->saveState();
    state[kViewGeometry] = saveGeometry();
    return state;
}

void ApplicationView::restoreState(const QVariantMap& _state)
{
    if (_state.contains(kSplitterState)) {
        d->splitter->restoreState(_state[kSplitterState].toByteArray());
    }
    if (_state.contains(kViewGeometry)) {
        restoreGeometry(_state[kViewGeometry].toByteArray());
    }
}

ApplicationView::~ApplicationView() = default;

void ApplicationView::showContent(QWidget* _toolbar, QWidget* _navigator, QWidget* _view)
{
    d->toolBar->setCurrentWidget(_toolbar);
    d->navigator->setCurrentWidget(_navigator);
    d->view->setCurrentWidget(_view);

    d->splitterShadow->raise();
}

int ApplicationView::navigationPanelWidth() const
{
    return d->toolBar->width() + d->splitter->handleWidth();
}

void ApplicationView::closeEvent(QCloseEvent* _event)
{
    //
    // Вместо реального закрытия формы сигнализируем об этом намерении
    //

    _event->ignore();
    emit closeRequested();
}

void ApplicationView::resizeEvent(QResizeEvent* _event)
{
    Widget::resizeEvent(_event);

    d->splitterShadow->resize(static_cast<int>(Ui::DesignSystem::layout().px24()),
                              _event->size().height());
}

void ApplicationView::designSystemChangeEvent(DesignSystemChangeEvent* _event)
{
    Q_UNUSED(_event)

    d->navigationWidget->setBackgroundColor(DesignSystem::color().primary());

    d->toolBar->setBackgroundColor(DesignSystem::color().primary());
    d->toolBar->setFixedHeight(static_cast<int>(DesignSystem::appBar().heightRegular()));

    d->navigator->setBackgroundColor(DesignSystem::color().primary());

    d->view->setBackgroundColor(DesignSystem::color().surface());

    d->splitter->setHandleColor(DesignSystem::color().primary());

    d->splitterShadow->move(-1 * d->splitterShadow->width() * 2 / 3, 0);
}

} // namespace Ui
