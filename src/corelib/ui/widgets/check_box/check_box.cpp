#include "check_box.h"

#include <ui/design_system/design_system.h>

#include <utils/helpers/text_helper.h>

#include <QPainter>
#include <QPaintEvent>
#include <QVariantAnimation>


class CheckBox::Implementation
{
public:
    Implementation();

    /**
     * @brief Анимировать клик
     */
    void animateClick();


    bool isChecked = false;
    QString text;

    /**
     * @brief  Декорации переключателя при клике
     */
    QVariantAnimation decorationRadiusAnimation;
    QVariantAnimation decorationOpacityAnimation;
};

CheckBox::Implementation::Implementation()
{
    decorationRadiusAnimation.setEasingCurve(QEasingCurve::OutQuad);
    decorationRadiusAnimation.setDuration(160);

    decorationOpacityAnimation.setEasingCurve(QEasingCurve::InQuad);
    decorationOpacityAnimation.setStartValue(0.5);
    decorationOpacityAnimation.setEndValue(0.0);
    decorationOpacityAnimation.setDuration(160);
}

void CheckBox::Implementation::animateClick()
{
    decorationOpacityAnimation.setCurrentTime(0);
    decorationRadiusAnimation.start();
    decorationOpacityAnimation.start();
}


// ****


CheckBox::CheckBox(QWidget* _parent)
    : Widget(_parent),
      d(new Implementation)
{
    setFocusPolicy(Qt::StrongFocus);

    connect(&d->decorationRadiusAnimation, &QVariantAnimation::valueChanged, this, [this] { update(); });
    connect(&d->decorationOpacityAnimation, &QVariantAnimation::valueChanged, this, [this] { update(); });

    designSystemChangeEvent(nullptr);
}

CheckBox::~CheckBox() = default;

bool CheckBox::isChecked() const
{
    return d->isChecked;
}

void CheckBox::setChecked(bool _checked)
{
    if (d->isChecked == _checked) {
        return;
    }

    d->isChecked = _checked;
    emit checkedChanged(d->isChecked);
    update();
}

void CheckBox::setText(const QString& _text)
{
    if (d->text == _text) {
        return;
    }

    d->text = _text;
    updateGeometry();
    update();
}

QSize CheckBox::sizeHint() const
{
    return QSize(static_cast<int>(Ui::DesignSystem::checkBox().margins().left()
                                  + Ui::DesignSystem::checkBox().iconSize().width()
                                  + Ui::DesignSystem::checkBox().spacing()
                                  + TextHelper::fineTextWidth(d->text, Ui::DesignSystem::font().subtitle1())
                                  + Ui::DesignSystem::checkBox().margins().right()),
                 static_cast<int>(Ui::DesignSystem::checkBox().height()));
}

void CheckBox::paintEvent(QPaintEvent* _event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    //
    // Заливаем фон
    //
    painter.fillRect(_event->rect(), backgroundColor());

    //
    // Рисуем декорацию переключателя
    //
    const QRectF iconRect(QPointF(Ui::DesignSystem::checkBox().margins().left(),
                                  Ui::DesignSystem::checkBox().margins().top()),
                          Ui::DesignSystem::checkBox().iconSize());
    if (d->decorationRadiusAnimation.state() == QVariantAnimation::Running
        || d->decorationOpacityAnimation.state() == QVariantAnimation::Running) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Ui::DesignSystem::color().secondary());
        painter.setOpacity(d->decorationOpacityAnimation.currentValue().toReal());
        painter.drawEllipse(iconRect.center(), d->decorationRadiusAnimation.currentValue().toReal(),
                            d->decorationRadiusAnimation.currentValue().toReal());
        painter.setOpacity(1.0);
    }

    //
    // Рисуем сам переключатель
    //
    painter.setFont(Ui::DesignSystem::font().iconsMid());
    painter.setPen(d->isChecked ? Ui::DesignSystem::color().secondary() : textColor());
    painter.drawText(iconRect, Qt::AlignCenter, d->isChecked ? "\ufc2e" : "\uf131");

    //
    // Рисуем текст
    //
    painter.setFont(Ui::DesignSystem::font().subtitle1());
    painter.setPen(textColor());
    const qreal textRectX = iconRect.right() + Ui::DesignSystem::checkBox().spacing();
    const QRectF textRect(textRectX, 0, width() - textRectX, sizeHint().height());
    painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, d->text);
}

void CheckBox::mouseReleaseEvent(QMouseEvent* _event)
{
    Q_UNUSED(_event)

    if (!rect().contains(_event->pos())) {
        return;
    }

    setChecked(!d->isChecked);
    d->animateClick();
}

void CheckBox::designSystemChangeEvent(DesignSystemChangeEvent* _event)
{
    Q_UNUSED(_event)

    d->decorationRadiusAnimation.setStartValue(Ui::DesignSystem::checkBox().iconSize().height() / 2.0);
    d->decorationRadiusAnimation.setEndValue(Ui::DesignSystem::checkBox().height() / 2.5);

    updateGeometry();
    update();
}
