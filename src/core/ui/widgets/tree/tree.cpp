#include "tree.h"
#include "tree_delegate.h"
#include "tree_view.h"

#include <ui/design_system/design_system.h>
#include <ui/widgets/scroll_bar/scroll_bar.h>

#include <QVBoxLayout>


class Tree::Implementation
{
public:
    explicit Implementation(QWidget* _parent);

    TreeView* tree = nullptr;
    TreeDelegate* delegate = nullptr;
};

Tree::Implementation::Implementation(QWidget* _parent)
    : tree(new TreeView(_parent)),
      delegate(new TreeDelegate(_parent))
{
    tree->setHeaderHidden(true);
    tree->setAnimated(true);
    tree->setMouseTracking(true);
    tree->setFrameShape(QFrame::NoFrame);
    tree->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    tree->setSelectionMode(QAbstractItemView::SingleSelection);
    tree->setSelectionBehavior(QAbstractItemView::SelectRows);
    tree->setVerticalScrollBar(new ScrollBar(tree));
    tree->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tree->setItemDelegate(delegate);
}


// ****


Tree::Tree(QWidget* _parent)
    : Widget(_parent),
      d(new Implementation(this))
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins({});
    layout->setSpacing(0);
    layout->addWidget(d->tree);

    designSystemChangeEvent(nullptr);
}

Tree::~Tree() = default;

void Tree::setModel(QAbstractItemModel* _model)
{
    if (model() != nullptr) {
        disconnect(d->tree->selectionModel(), &QItemSelectionModel::currentChanged, this, &Tree::currentIndexChanged);
    }

    d->tree->setModel(_model);

    if (model() != nullptr) {
        connect(d->tree->selectionModel(), &QItemSelectionModel::currentChanged, this, &Tree::currentIndexChanged);
    }
}

QAbstractItemModel* Tree::model() const
{
    return d->tree->model();
}

void Tree::setRootIsDecorated(bool _decorated)
{
    d->tree->setRootIsDecorated(_decorated);
}

void Tree::setDragDropEnabled(bool _enabled)
{
    d->tree->setAcceptDrops(_enabled);
    d->tree->setDragEnabled(_enabled);
    d->tree->setDragDropMode(_enabled ? QAbstractItemView::DragDrop : QAbstractItemView::NoDragDrop);
    d->tree->setDropIndicatorShown(_enabled);
    d->tree->setSelectionMode(_enabled ? QAbstractItemView::ExtendedSelection : QAbstractItemView::SingleSelection);
}

void Tree::setCurrentIndex(const QModelIndex& _index)
{
    d->tree->setCurrentIndex(_index);
}

void Tree::expandAll()
{
    d->tree->expandAll();
}

void Tree::restoreState(const QVariant& _state)
{
    d->tree->restoreState(_state);
}

QVariant Tree::saveState() const
{
    return d->tree->saveState();
}

void Tree::designSystemChangeEvent(DesignSystemChangeEvent* _event)
{
    Widget::designSystemChangeEvent(_event);

    QPalette palette = d->tree->palette();
    palette.setColor(QPalette::Base, backgroundColor());
    auto alternateBaseColor = textColor();
    alternateBaseColor.setAlphaF(Ui::DesignSystem::hoverBackgroundOpacity());
    palette.setColor(QPalette::AlternateBase, alternateBaseColor);
    palette.setColor(QPalette::Text, textColor());
    palette.setColor(QPalette::Highlight, Ui::DesignSystem::tree().selectionColor());
    palette.setColor(QPalette::HighlightedText, Ui::DesignSystem::color().secondary());
    d->tree->setPalette(palette);
    d->tree->setIndentation(static_cast<int>(Ui::DesignSystem::tree().indicatorWidth()));
    d->tree->setItemDelegate(nullptr);
    d->tree->setItemDelegate(d->delegate);
}