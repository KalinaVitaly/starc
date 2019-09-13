#include "floating_tool_bar.h"

#include <ui/design_system/design_system.h>

#include <utils/helpers/image_helper.h>

#include <QAction>
#include <QPainter>
#include <QPaintEvent>
#include <QVariantAnimation>


class FloatingToolBar::Implementation
{
public:
    Implementation();

    /**
     * @brief Анимировать клик
     */
    void animateClick();

    /**
     * @brief Анимировать тень
     */
    void animateHoverIn();
    void animateHoverOut();

    /**
     * @brief Получить пункт меню по координате
     */
    QAction* pressedAction(const QPoint& _coordinate, const QList<QAction*>& _actions) const;


    /**
     * @brief Иконка на которой кликнули последней
     */
    QAction* lastPressedAction = nullptr;

    /**
     * @brief  Декорации иконки при клике
     */
    QVariantAnimation decorationRadiusAnimation;
    QVariantAnimation decorationOpacityAnimation;

    /**
     * @brief  Декорации тени при наведении
     */
    QVariantAnimation shadowHeightAnimation;
};

FloatingToolBar::Implementation::Implementation()
{
    decorationRadiusAnimation.setEasingCurve(QEasingCurve::OutQuad);
    decorationRadiusAnimation.setDuration(160);

    decorationOpacityAnimation.setEasingCurve(QEasingCurve::InQuad);
    decorationOpacityAnimation.setStartValue(0.5);
    decorationOpacityAnimation.setEndValue(0.0);
    decorationOpacityAnimation.setDuration(160);

    shadowHeightAnimation.setEasingCurve(QEasingCurve::OutQuad);
    shadowHeightAnimation.setDuration(160);
}

void FloatingToolBar::Implementation::animateClick()
{
    decorationOpacityAnimation.setCurrentTime(0);
    decorationRadiusAnimation.start();
    decorationOpacityAnimation.start();
}

void FloatingToolBar::Implementation::animateHoverIn()
{
    shadowHeightAnimation.setDirection(QVariantAnimation::Forward);
    shadowHeightAnimation.start();
}

void FloatingToolBar::Implementation::animateHoverOut()
{
    shadowHeightAnimation.setDirection(QVariantAnimation::Backward);
    shadowHeightAnimation.start();
}

QAction* FloatingToolBar::Implementation::pressedAction(const QPoint& _coordinate, const QList<QAction*>& _actions) const
{
    qreal actionLeft = Ui::DesignSystem::floatingToolBar().shadowMargins().left()
                       + Ui::DesignSystem::floatingToolBar().margins().left()
                       - (Ui::DesignSystem::floatingToolBar().spacing() / 2.0);
    //
    // Берём увеличенный регион для удобства клика в области кнопки
    //
    const qreal actionWidth = Ui::DesignSystem::floatingToolBar().iconSize().width()
                              + Ui::DesignSystem::floatingToolBar().spacing();
    for (QAction* action : _actions) {
        const qreal actionRight = actionLeft + actionWidth;

        if (actionLeft < _coordinate.x() && _coordinate.x() < actionRight) {
            return action;
        }

        actionLeft = actionRight;
    }

    return nullptr;
}


// ****


FloatingToolBar::FloatingToolBar(QWidget* _parent)
    : Widget(_parent),
      d(new Implementation)
{
    connect(&d->decorationRadiusAnimation, &QVariantAnimation::valueChanged, this, [this] { update(); });
    connect(&d->decorationOpacityAnimation, &QVariantAnimation::valueChanged, this, [this] { update(); });
    connect(&d->shadowHeightAnimation, &QVariantAnimation::valueChanged, this, [this] { update(); });

    designSystemChangeEvent(nullptr);
}

QSize FloatingToolBar::sizeHint() const
{
    const qreal width = Ui::DesignSystem::floatingToolBar().shadowMargins().left()
                        + Ui::DesignSystem::floatingToolBar().margins().left()
                        + Ui::DesignSystem::floatingToolBar().iconSize().width() * actions().size()
                        + Ui::DesignSystem::floatingToolBar().spacing() * (actions().size() - 1)
                        + Ui::DesignSystem::floatingToolBar().margins().right()
                        + Ui::DesignSystem::floatingToolBar().shadowMargins().right();
    const qreal height = Ui::DesignSystem::floatingToolBar().shadowMargins().top()
                         + Ui::DesignSystem::floatingToolBar().height()
                         + Ui::DesignSystem::floatingToolBar().shadowMargins().bottom();
    return QSize(static_cast<int>(width), static_cast<int>(height));
}

FloatingToolBar::~FloatingToolBar() = default;

void FloatingToolBar::paintEvent(QPaintEvent* _event)
{
    Q_UNUSED(_event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QRect backgroundRect = rect().marginsRemoved(Ui::DesignSystem::floatingToolBar().shadowMargins().toMargins());
    if (!backgroundRect.isValid()) {
        return;
    }

    //
    // Заливаем фон
    //
    QPixmap backgroundImage(backgroundRect.size());
    backgroundImage.fill(Qt::transparent);
    QPainter backgroundImagePainter(&backgroundImage);
    backgroundImagePainter.setPen(Qt::NoPen);
    backgroundImagePainter.setBrush(backgroundColor());
    const qreal radius = Ui::DesignSystem::floatingToolBar().height() / 2.0;
    backgroundImagePainter.drawRoundedRect(QRect({0,0}, backgroundImage.size()), radius, radius);
    //
    // ... рисуем тень
    //
    const qreal shadowHeight = std::max(Ui::DesignSystem::floatingToolBar().minimumShadowHeight(),
                                        d->shadowHeightAnimation.currentValue().toReal());
    const QPixmap shadow
            = ImageHelper::dropShadow(backgroundImage,
                                      QMarginsF(Ui::DesignSystem::floatingToolBar().shadowMargins().left(),
                                                Ui::DesignSystem::floatingToolBar().shadowMargins().top(),
                                                Ui::DesignSystem::floatingToolBar().shadowMargins().right(),
                                                shadowHeight),
                                      Ui::DesignSystem::floatingToolBar().shadowBlurRadius() + shadowHeight,
                                      Ui::DesignSystem::color().shadow());
    painter.drawPixmap(0, 0, shadow);
    //
    // ... рисуем сам фон
    //
    painter.setPen(Qt::NoPen);
    painter.setBrush(backgroundColor());
    painter.drawRoundedRect(backgroundRect, radius, radius);
    painter.drawPixmap(backgroundRect.topLeft(), backgroundImage);

    //
    // Рисуем иконки
    //
    painter.setFont(Ui::DesignSystem::font().iconsMid());
    qreal actionX = Ui::DesignSystem::floatingToolBar().shadowMargins().left()
                    + Ui::DesignSystem::floatingToolBar().margins().left();
    const qreal actionY = Ui::DesignSystem::floatingToolBar().shadowMargins().top()
                          + Ui::DesignSystem::floatingToolBar().margins().top();
    const QSizeF actionSize = Ui::DesignSystem::floatingToolBar().iconSize();
    for (const QAction* action : actions()) {
        //
        // ... сама иконка
        //
        const QRectF actionRect(QPointF(actionX, actionY), actionSize);
        painter.setPen(action->isChecked() ? Ui::DesignSystem::color().secondary() : textColor());
        painter.drawText(actionRect, Qt::AlignCenter, action->text());

        //
        // ... декорация
        //
        if (action == d->lastPressedAction
            && (d->decorationRadiusAnimation.state() == QVariantAnimation::Running
                || d->decorationOpacityAnimation.state() == QVariantAnimation::Running)) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(Ui::DesignSystem::color().secondary());
            painter.setOpacity(d->decorationOpacityAnimation.currentValue().toReal());
            painter.drawEllipse(actionRect.center(), d->decorationRadiusAnimation.currentValue().toReal(),
                                d->decorationRadiusAnimation.currentValue().toReal());
            painter.setOpacity(1.0);
        }

        actionX += actionRect.width() + Ui::DesignSystem::floatingToolBar().spacing();
    }
}

void FloatingToolBar::enterEvent(QEvent* _event)
{
    Q_UNUSED(_event);
    d->animateHoverIn();
}

void FloatingToolBar::leaveEvent(QEvent* _event)
{
    Q_UNUSED(_event);
    d->animateHoverOut();
}

void FloatingToolBar::mousePressEvent(QMouseEvent* _event)
{
    QAction* pressedAction = d->pressedAction(_event->pos(), actions());
    if (pressedAction == nullptr) {
        return;
    }

    d->lastPressedAction = pressedAction;
    d->animateClick();
}

void FloatingToolBar::mouseReleaseEvent(QMouseEvent* _event)
{
    QAction* pressedAction = d->pressedAction(_event->pos(), actions());
    if (pressedAction == nullptr) {
        return;
    }

    if (!pressedAction->isCheckable()) {
        pressedAction->trigger();
        return;
    }

    if (pressedAction->isChecked()) {
        return;
    }

    for (auto action : actions()) {
        if (action->isCheckable() && action != pressedAction) {
            action->setChecked(false);
        }
    }
    pressedAction->setChecked(true);
    update();
}

void FloatingToolBar::designSystemChangeEvent(DesignSystemChangeEvent* _event)
{
    Q_UNUSED(_event);

    d->decorationRadiusAnimation.setStartValue(Ui::DesignSystem::floatingToolBar().iconSize().height() / 2.0);
    d->decorationRadiusAnimation.setEndValue(Ui::DesignSystem::floatingToolBar().height() / 2.5);
    d->shadowHeightAnimation.setStartValue(Ui::DesignSystem::floatingToolBar().minimumShadowHeight());
    d->shadowHeightAnimation.setEndValue(Ui::DesignSystem::floatingToolBar().maximumShadowHeight());

    updateGeometry();
    update();
}
