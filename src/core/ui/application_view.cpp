#include "application_view.h"

#include <ui/design_system/design_system.h>
#include <ui/widgets/floating_tool_bar/floating_tool_bar.h>
#include <ui/widgets/shadow/shadow.h>
#include <ui/widgets/splitter/splitter.h>
#include <ui/widgets/stack_widget/stack_widget.h>

#include <QAction>
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

    FloatingToolBar* accountBar = nullptr;
};

ApplicationView::Implementation::Implementation(QWidget* _parent)
    : navigationWidget(new Widget(_parent)),
      toolBar(new StackWidget(_parent)),
      navigator(new StackWidget(_parent)),
      view(new StackWidget(_parent)),
      splitter(new Splitter(_parent)),
      splitterShadow(new Shadow(view)),
      accountBar(new FloatingToolBar(_parent))
{
}


// ****


ApplicationView::ApplicationView(QWidget* _parent)
    : Widget(_parent),
      d(new Implementation(this))
{
    d->view->installEventFilter(this);

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

    QAction* accountAction = new QAction("\uf004");
    d->accountBar->addAction(accountAction);
    connect(accountAction, &QAction::triggered, this, &ApplicationView::accountPressed);

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

void ApplicationView::setAccountVisible(bool _visible)
{
    d->accountBar->setVisible(_visible);
}

bool ApplicationView::eventFilter(QObject* _target, QEvent* _event)
{
    if (_target == d->view
        && _event->type() == QEvent::Resize) {
        QResizeEvent* event = static_cast<QResizeEvent*>(_event);
        d->accountBar->move(d->view->mapTo(this, QPoint())
                            + QPointF(event->size().width()
                                      - d->accountBar->width()
                                      - Ui::DesignSystem::layout().px24(),
                                      Ui::DesignSystem::layout().px24()).toPoint());
    }

    return Widget::eventFilter(_target, _event);
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

    d->accountBar->resize(d->accountBar->sizeHint());
    d->accountBar->move(QPointF(size().width()
                                  - d->accountBar->width()
                                  - Ui::DesignSystem::layout().px24(),
                                  Ui::DesignSystem::layout().px24()).toPoint());
    d->accountBar->setBackgroundColor(Ui::DesignSystem::color().primary());
    d->accountBar->setTextColor(Ui::DesignSystem::color().onPrimary());
    d->accountBar->raise();
}

} // namespace Ui
