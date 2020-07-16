#include "screenplay_text_comments_toolbar.h"

#include <QAction>
#include <QPainter>
#include <QPaintEvent>
#include <QTimer>
#include <QVariantAnimation>


namespace Ui
{

class ScreenplayTextCommentsToolbar::Implementation
{
public:
    Implementation();

    /**
     * @brief Анимировать отображение
     */
    void animateShow();

    /**
     * @brief Анимировать скрытие
     */
    void animateHide();

    /**
     * @brief Анимировать смещение
     */
    void animateMove(const QPoint& _from, const QPoint& _to);


    QAction* textColorAction = nullptr;
    QAction* backgroundColorAction = nullptr;
    QAction* commentAction = nullptr;
    QAction* colorAction = nullptr;

    QVariantAnimation opacityAnimation;
    QTimer hideTimer;
    QPixmap contentPixmap;
    QVariantAnimation moveAnimation;
};

ScreenplayTextCommentsToolbar::Implementation::Implementation()
    : textColorAction(new QAction),
      backgroundColorAction(new QAction),
      commentAction(new QAction),
      colorAction(new QAction)
{
    opacityAnimation.setDuration(220);
    opacityAnimation.setEasingCurve(QEasingCurve::OutQuad);
    hideTimer.setSingleShot(true);
    hideTimer.setInterval(opacityAnimation.duration());
    moveAnimation.setDuration(420);
    moveAnimation.setEasingCurve(QEasingCurve::OutQuad);
}

void ScreenplayTextCommentsToolbar::Implementation::animateShow()
{
    hideTimer.stop();

    opacityAnimation.setStartValue(0.0);
    opacityAnimation.setEndValue(1.0);
    opacityAnimation.start();
}

void ScreenplayTextCommentsToolbar::Implementation::animateHide()
{
    opacityAnimation.setStartValue(1.0);
    opacityAnimation.setEndValue(0.0);
    opacityAnimation.start();

    hideTimer.start();
}

void ScreenplayTextCommentsToolbar::Implementation::animateMove(const QPoint& _from, const QPoint& _to)
{
    if (moveAnimation.state() == QVariantAnimation::Running) {
        if (moveAnimation.endValue().toPoint() == _to) {
            return;
        } else {
            moveAnimation.stop();
        }
    }

    moveAnimation.setStartValue(_from);
    moveAnimation.setEndValue(_to);
    moveAnimation.start();
}


// ****


ScreenplayTextCommentsToolbar::ScreenplayTextCommentsToolbar(QWidget* _parent)
    : FloatingToolBar(_parent),
      d(new Implementation)
{
    setOrientation(Qt::Vertical);

    d->textColorAction->setIconText(u8"\U000f069e");
    addAction(d->textColorAction);

    d->backgroundColorAction->setIconText(u8"\U000f0266");
    addAction(d->backgroundColorAction);

    d->commentAction->setIconText(u8"\U000f0188");
    addAction(d->commentAction);

    d->colorAction->setIconText(u8"\U000f0765");
    addAction(d->colorAction);

    connect(&d->opacityAnimation, &QVariantAnimation::valueChanged, this, [this] { update(); });
    connect(&d->hideTimer, &QTimer::timeout, this, &Widget::hide);
    connect(&d->moveAnimation, &QVariantAnimation::valueChanged, this,
            [this] (const QVariant& _value) { move(_value.toPoint()); });

    updateTranslations();
    designSystemChangeEvent(nullptr);
}

void ScreenplayTextCommentsToolbar::showToolbar()
{
    if (parentWidget() == nullptr) {
        return;
    }

    if (isVisible() && d->opacityAnimation.endValue().toReal() > 0.0) {
        return;
    }

    //
    // Сохраняем изображение контента и прячем сам виджет
    //
    d->contentPixmap = grab();

    //
    // Запускаем отображение
    //
    d->animateShow();
    show();
}

void ScreenplayTextCommentsToolbar::hideToolbar()
{
    if (isHidden()) {
        return;
    }

    if (d->opacityAnimation.endValue().toReal() < 1.0) {
        return;
    }

    d->contentPixmap = grab();
    d->animateHide();
}

void ScreenplayTextCommentsToolbar::moveToolbar(const QPoint& _position)
{
    if (isHidden()) {
        move(_position);
        return;
    }

    d->animateMove(pos(), _position);
}

void ScreenplayTextCommentsToolbar::paintEvent(QPaintEvent* _event)
{
    //
    // Если надо, анимируем появление
    //
    if (d->opacityAnimation.state() == QVariantAnimation::Running) {
        QPainter painter(this);
        painter.setOpacity(d->opacityAnimation.currentValue().toReal());
        painter.drawPixmap(QPointF{}, d->contentPixmap);
        return;
    }

    FloatingToolBar::paintEvent(_event);
}

ScreenplayTextCommentsToolbar::~ScreenplayTextCommentsToolbar() = default;

void ScreenplayTextCommentsToolbar::updateTranslations()
{
    d->textColorAction->setToolTip(tr("Change text color"));
    d->backgroundColorAction->setToolTip(tr("Change text highlight color"));
    d->commentAction->setToolTip(tr("Add comment"));
    d->colorAction->setToolTip(tr("Choose color for the action below"));
}

void ScreenplayTextCommentsToolbar::designSystemChangeEvent(DesignSystemChangeEvent* _event)
{
    FloatingToolBar::designSystemChangeEvent(_event);

    resize(sizeHint());
}

} // namespace Ui