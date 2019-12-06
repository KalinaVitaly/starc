#include "text_field.h"

#include <custom_events.h>

#include <ui/design_system/design_system.h>

#include <utils/helpers/image_helper.h>
#include <utils/helpers/text_helper.h>

#include <QEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QTextFrame>
#include <QVariantAnimation>


class TextField::Implementation
{
public:
    Implementation();

    /**
     * @brief Переконфигурировать виджет при смене параметров дизайн системы
     */
    void reconfigure(TextField* _textField);

    /**
     * @brief Анимировать лейбл
     */
    /** @{ */
    void animateLabelToTop();
    void animateLabelToBottom();
    /** @} */

    /**
     * @brief Закончить анимации, если виджет скрыт
     */
    void finishAnimationIfInvisible(TextField* _textField);

    /**
     * @brief Определить область декорирования для заданного размера
     */
    /** @{ */
    QRectF decorationRect(const QSize& _textFieldSize) const;
    QRectF decorationRectInFocus(const QSize& _textFieldSize) const;
    /** @} */

    QString label;
    QString helper;
    QString error;

    QString trailingIcon;
    QColor trailingIconColor;

    bool isPasswordModeEnabled = false;
    bool isEnterMakesNewLine = false;

    QVariantAnimation labelColorAnimation;
    QVariantAnimation labelFontSizeAnimation;
    QVariantAnimation labelTopLeftAnimation;
    QVariantAnimation decorationAnimation;
};

TextField::Implementation::Implementation()
{
    labelColorAnimation.setDuration(200);
    labelColorAnimation.setEasingCurve(QEasingCurve::OutQuad);
    labelFontSizeAnimation.setDuration(200);
    labelFontSizeAnimation.setEasingCurve(QEasingCurve::OutQuad);
    labelTopLeftAnimation.setDuration(200);
    labelTopLeftAnimation.setEasingCurve(QEasingCurve::OutQuad);
    decorationAnimation.setDuration(200);
    decorationAnimation.setEasingCurve(QEasingCurve::OutQuad);
}

void TextField::Implementation::reconfigure(TextField* _textField)
{
    QSignalBlocker signalBlocker(_textField);

    _textField->setFont(Ui::DesignSystem::font().body1());

    QPalette palette = _textField->palette();
    palette.setColor(QPalette::Base, Qt::transparent);
    QColor textColor = Ui::DesignSystem::color().onSurface();
    palette.setColor(QPalette::Normal, QPalette::Text, textColor);
    textColor.setAlphaF(Ui::DesignSystem::disabledTextOpacity());
    palette.setColor(QPalette::Disabled, QPalette::Text, textColor);
    _textField->setPalette(palette);

    QTextFrameFormat frameFormat = _textField->document()->rootFrame()->frameFormat();
    frameFormat.setLeftMargin(Ui::DesignSystem::textField().contentsMargins().left()
                              + Ui::DesignSystem::textField().margins().left());
    frameFormat.setTopMargin(Ui::DesignSystem::textField().contentsMargins().top()
                             + Ui::DesignSystem::textField().margins().top());
    frameFormat.setRightMargin(Ui::DesignSystem::textField().contentsMargins().right()
                               + Ui::DesignSystem::textField().margins().right());
    frameFormat.setBottomMargin(Ui::DesignSystem::textField().contentsMargins().bottom()
                                + Ui::DesignSystem::textField().margins().bottom());
    _textField->document()->rootFrame()->setFrameFormat(frameFormat);

    if (_textField->text().isEmpty()) {
        animateLabelToBottom();
    } else {
        animateLabelToTop();
    }
    labelFontSizeAnimation.setCurrentTime(labelFontSizeAnimation.duration());
    labelTopLeftAnimation.setCurrentTime(labelTopLeftAnimation.duration());
}

void TextField::Implementation::animateLabelToTop()
{
    labelFontSizeAnimation.setStartValue(Ui::DesignSystem::font().body1().pixelSize());
    labelFontSizeAnimation.setEndValue(Ui::DesignSystem::font().caption().pixelSize());
    labelFontSizeAnimation.start();

    labelTopLeftAnimation.stop();
    const QPointF labelNewTopLeft = Ui::DesignSystem::textField().labelTopLeft()
                                    + QPointF(Ui::DesignSystem::textField().contentsMargins().left(),
                                              Ui::DesignSystem::textField().contentsMargins().top());
    const QPointF labelCurrentTopLeft(labelNewTopLeft.x(),
                                      Ui::DesignSystem::textField().contentsMargins().top()
                                      + Ui::DesignSystem::textField().margins().top());
    labelTopLeftAnimation.setStartValue(labelCurrentTopLeft);
    labelTopLeftAnimation.setEndValue(labelNewTopLeft);
    labelTopLeftAnimation.start();
}

void TextField::Implementation::animateLabelToBottom()
{
    labelFontSizeAnimation.setStartValue(Ui::DesignSystem::font().caption().pixelSize());
    labelFontSizeAnimation.setEndValue(Ui::DesignSystem::font().body1().pixelSize());
    labelFontSizeAnimation.start();

    const QPointF labelCurrentTopLeft = Ui::DesignSystem::textField().labelTopLeft()
                                        + QPointF(Ui::DesignSystem::textField().contentsMargins().left(),
                                                  Ui::DesignSystem::textField().contentsMargins().top());
    const QPointF labelNewTopLeft(labelCurrentTopLeft.x(),
                                  Ui::DesignSystem::textField().contentsMargins().top()
                                  + Ui::DesignSystem::textField().margins().top());
    labelTopLeftAnimation.setStartValue(labelCurrentTopLeft);
    labelTopLeftAnimation.setEndValue(labelNewTopLeft);
    labelTopLeftAnimation.start();
}

void TextField::Implementation::finishAnimationIfInvisible(TextField* _textField)
{
    //
    // Если лейбл скрыт, то выполнять всю анимацию ни к чему
    //
    if (!_textField->isVisible()) {
        labelFontSizeAnimation.setCurrentTime(labelFontSizeAnimation.duration());
        labelTopLeftAnimation.setCurrentTime(labelTopLeftAnimation.duration());
    }
}

QRectF TextField::Implementation::decorationRect(const QSize& _textFieldSize) const
{
    qreal top = _textFieldSize.height()
                - Ui::DesignSystem::textField().underlineHeight()
                - Ui::DesignSystem::textField().contentsMargins().top()
                - Ui::DesignSystem::textField().contentsMargins().bottom();
    if (!helper.isEmpty() || !error.isEmpty()) {
        top -= Ui::DesignSystem::textField().helperHeight();
    }
    return QRectF(Ui::DesignSystem::textField().contentsMargins().left(),
                  top,
                  _textFieldSize.width()
                  - Ui::DesignSystem::textField().contentsMargins().left()
                  - Ui::DesignSystem::textField().contentsMargins().right(),
                  Ui::DesignSystem::textField().underlineHeight());
}

QRectF TextField::Implementation::decorationRectInFocus(const QSize& _textFieldSize) const
{
    qreal top = _textFieldSize.height()
                - Ui::DesignSystem::textField().underlineHeight()
                - Ui::DesignSystem::textField().contentsMargins().top()
                - Ui::DesignSystem::textField().contentsMargins().bottom();
    if (!helper.isEmpty() || !error.isEmpty()) {
        top -= Ui::DesignSystem::textField().helperHeight();
    }
    return QRectF(Ui::DesignSystem::textField().contentsMargins().left(),
                  top,
                  _textFieldSize.width()
                  - Ui::DesignSystem::textField().contentsMargins().left()
                  - Ui::DesignSystem::textField().contentsMargins().right(),
                  Ui::DesignSystem::textField().underlineHeightInFocus());
}


// ****


TextField::TextField(QWidget* _parent)
    : QTextEdit(_parent),
      d(new Implementation)
{
    setFrameShape(QFrame::NoFrame);
    setWordWrapMode(QTextOption::WordWrap);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setTabChangesFocus(true);

    QSizePolicy policy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    policy.setHeightForWidth(true);
    setSizePolicy(policy);

    d->reconfigure(this);

    connect(&d->labelColorAnimation, &QVariantAnimation::valueChanged, this, [this] { update(); });
    connect(&d->labelFontSizeAnimation, &QVariantAnimation::valueChanged, this, [this] { update(); });
    connect(&d->labelTopLeftAnimation, &QVariantAnimation::valueChanged, this, [this] { update(); });
    connect(&d->decorationAnimation, &QVariantAnimation::valueChanged, this, [this] { update(); });
    connect(document(), &QTextDocument::contentsChange, this, &TextField::updateGeometry);
}

void TextField::setLabel(const QString& _text)
{
    if (d->label == _text) {
        return;
    }

    d->label = _text;
    update();
}

void TextField::setHelper(const QString& _text)
{
    if (d->helper == _text) {
        return;
    }

    d->helper = _text;
    updateGeometry();
    update();
}

void TextField::setError(const QString& _text)
{
    if (d->error == _text) {
        return;
    }

    d->error = _text;
    updateGeometry();
    update();
}

QString TextField::text() const
{
    return toPlainText();
}

void TextField::setText(const QString& _text)
{
    if (_text.isEmpty()) {
        clear();
        return;
    }

    const bool needAnimate = text().isEmpty();

    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();
    cursor.select(QTextCursor::Document);
    cursor.insertText(_text);
    cursor.endEditBlock();

    //
    // Анимируем только в случае, когда текста не было и его установили
    //
    if (needAnimate) {
        d->animateLabelToTop();
        d->finishAnimationIfInvisible(this);
    }
}

void TextField::setTrailingIcon(const QString& _icon)
{
    if (d->trailingIcon == _icon) {
        return;
    }

    d->trailingIcon = _icon;
    update();
}

void TextField::setPasswordModeEnabled(bool _enable)
{
    if (d->isPasswordModeEnabled == _enable) {
        return;
    }

    d->isPasswordModeEnabled = _enable;
    update();
}

bool TextField::isPasswordModeEnabled() const
{
    return d->isPasswordModeEnabled;
}

void TextField::setEnterMakesNewLine(bool _make)
{
    if (d->isEnterMakesNewLine == _make) {
        return;
    }

    d->isEnterMakesNewLine = _make;
}

void TextField::clear()
{
    if (text().isEmpty()) {
        return;
    }

    QTextCursor cursor = textCursor();
    cursor.select(QTextCursor::Document);
    cursor.deleteChar();

    d->animateLabelToBottom();
    d->finishAnimationIfInvisible(this);
}

QSize TextField::minimumSizeHint() const
{
    return QSize();
}

QSize TextField::sizeHint() const
{
    const QFontMetrics fontMetrics(Ui::DesignSystem::font().body1());
    QSize size(TextHelper::fineTextWidth(text(), Ui::DesignSystem::font().body1()),
               fontMetrics.height());
    size += QSizeF(Ui::DesignSystem::textField().margins().left()
                   + Ui::DesignSystem::textField().margins().right(),
                   Ui::DesignSystem::textField().margins().top()
                   + Ui::DesignSystem::textField().margins().bottom()).toSize();
    size += QSizeF(Ui::DesignSystem::textField().contentsMargins().left()
                   + Ui::DesignSystem::textField().contentsMargins().right(),
                   Ui::DesignSystem::textField().contentsMargins().top()
                   + Ui::DesignSystem::textField().contentsMargins().bottom()).toSize();
    return size;
}

int TextField::heightForWidth(int _width) const
{
    qreal width = _width;
    width -= Ui::DesignSystem::textField().margins().left()
             + Ui::DesignSystem::textField().margins().right();
    qreal height = TextHelper::heightForWidth(text(), Ui::DesignSystem::font().body1(), static_cast<int>(width));
    height += Ui::DesignSystem::textField().margins().top()
              + Ui::DesignSystem::textField().margins().bottom();
    height += Ui::DesignSystem::textField().contentsMargins().top()
              + Ui::DesignSystem::textField().contentsMargins().bottom();
    if (!d->helper.isEmpty() || !d->error.isEmpty()) {
        height += Ui::DesignSystem::textField().helperHeight();
    }
    return static_cast<int>(height);
}

void TextField::setTrailingIconColor(const QColor& _color)
{
    if (d->trailingIconColor == _color) {
        return;
    }

    d->trailingIconColor = _color;
    update();
}

bool TextField::event(QEvent* _event)
{
    switch (static_cast<int>(_event->type())) {
        case static_cast<QEvent::Type>(EventType::DesignSystemChangeEvent): {
            d->reconfigure(this);
            updateGeometry();
            update();
            return true;
        }

        default: {
            return QTextEdit::event(_event);
        }
    }
}

void TextField::paintEvent(QPaintEvent* _event)
{
    QPainter painter;

    //
    // Рисуем фон
    //
    painter.begin(viewport());
    QRectF backgroundRect = rect().marginsRemoved(Ui::DesignSystem::textField().contentsMargins().toMargins());
    if (!d->helper.isEmpty() || !d->error.isEmpty()) {
        backgroundRect.moveBottom(backgroundRect.bottom()
                                  - Ui::DesignSystem::textField().helperHeight());
    }
    painter.fillRect(backgroundRect,
                     hasFocus() || underMouse()
                     ? Ui::DesignSystem::textField().backgroundActiveColor()
                     : Ui::DesignSystem::textField().backgroundInactiveColor());
    painter.end();

    //
    // Если не включён режим отображения пароля, то отрисовкой текста и курсора занимается сам QTextEdit
    //
    if (!d->isPasswordModeEnabled) {
        QTextEdit::paintEvent(_event);
    }
    //
    // В противном случае, самостоятельно рисуем звёздочки вместо букв
    //
    else if (!text().isEmpty()) {
        painter.begin(viewport());
        painter.setFont(Ui::DesignSystem::font().body1());
        painter.setPen(Ui::DesignSystem::color().onSurface());

        QPointF labelTopLeft = Ui::DesignSystem::textField().labelTopLeft()
                               + QPointF(Ui::DesignSystem::textField().contentsMargins().left(),
                                         Ui::DesignSystem::textField().contentsMargins().top());
        labelTopLeft.setY(Ui::DesignSystem::textField().margins().top());
        const QRectF labelRect(labelTopLeft,
                               QSizeF(width() - labelTopLeft.x(),
                                      QFontMetricsF(Ui::DesignSystem::font().body1()).lineSpacing()));
        painter.drawText(labelRect, Qt::AlignLeft | Qt::AlignBottom,
                         QString(text().length(), QString("●").front()));
        painter.end();
    }


    //
    // Рисуем декорации
    //
    painter.begin(viewport());
    //
    // ... лейбл
    //
    if (!d->label.isEmpty()) {
        QFont labelFont = Ui::DesignSystem::font().caption();
        if (!d->labelFontSizeAnimation.currentValue().isNull()) {
            labelFont.setPixelSize(d->labelFontSizeAnimation.currentValue().toInt());
        } else if (!hasFocus() && text().isEmpty() && placeholderText().isEmpty()) {
            labelFont.setPixelSize(Ui::DesignSystem::font().body1().pixelSize());
        }
        painter.setFont(labelFont);

        QColor labelColor = Ui::DesignSystem::textField().foregroundColor();
        if (!d->error.isEmpty()) {
            labelColor = Ui::DesignSystem::color().error();
        } else if (!d->labelColorAnimation.currentValue().isNull()) {
            labelColor = d->labelColorAnimation.currentValue().value<QColor>();
        }
        painter.setPen(labelColor);

        QPointF labelTopLeft = Ui::DesignSystem::textField().labelTopLeft()
                               + QPointF(Ui::DesignSystem::textField().contentsMargins().left(),
                                         Ui::DesignSystem::textField().contentsMargins().top());
        if (!d->labelTopLeftAnimation.currentValue().isNull()) {
            labelTopLeft = d->labelTopLeftAnimation.currentValue().toPointF();
        } else if (!hasFocus() && text().isEmpty() && placeholderText().isEmpty()) {
            labelTopLeft.setY(Ui::DesignSystem::textField().margins().top());
        }
        const QRectF labelRect(labelTopLeft, QSizeF(width() - labelTopLeft.x(), QFontMetricsF(labelFont).lineSpacing()));
        painter.drawText(labelRect, Qt::AlignLeft | Qt::AlignVCenter, d->label);
    }
    //
    // ... иконка действия
    //
    if (!d->trailingIcon.isEmpty()) {
        painter.setFont(Ui::DesignSystem::font().iconsMid());
        painter.setPen(d->trailingIconColor.isValid()
                       ? d->trailingIconColor
                       : palette().color(QPalette::Text));
        const QRectF iconRect(QPointF(width()
                                      - Ui::DesignSystem::textField().contentsMargins().right()
                                      - Ui::DesignSystem::textField().margins().right()
                                      - Ui::DesignSystem::textField().iconSize().width(),
                                      Ui::DesignSystem::textField().iconTop()),
                              Ui::DesignSystem::textField().iconSize());
        painter.drawText(iconRect.toRect(), Qt::AlignCenter, d->trailingIcon);
    }
    //
    // ... подчёркивание
    // ...... в обычном состоянии
    //
    {
        const QRectF decorationRect = d->decorationRect(size());
        const QColor decorationColor = !d->error.isEmpty() ? Ui::DesignSystem::color().error()
                                                           : Ui::DesignSystem::textField().foregroundColor();
        painter.fillRect(decorationRect, decorationColor);
    }
    //
    // ...... в выделенном
    //
    if (d->decorationAnimation.currentValue().isValid()) {
        QRectF decorationRect = d->decorationAnimation.currentValue().toRectF();
        decorationRect.moveTop(d->decorationRectInFocus(size()).top());
        const QColor decorationColor = !d->error.isEmpty() ? Ui::DesignSystem::color().error()
                                                           : Ui::DesignSystem::color().secondary();
        painter.fillRect(decorationRect, decorationColor);
    }
    //
    // ... ошибка или подсказка
    //
    if (!d->error.isEmpty() || !d->helper.isEmpty()) {
        painter.setFont(Ui::DesignSystem::font().caption());
        const QColor color = !d->error.isEmpty() ? Ui::DesignSystem::color().error()
                                                 : Ui::DesignSystem::textField().foregroundColor();
        painter.setPen(color);
        const QRectF textRect(Ui::DesignSystem::textField().contentsMargins().left()
                              + Ui::DesignSystem::textField().margins().left(),
                              height() - Ui::DesignSystem::textField().helperHeight(),
                              width()
                              - Ui::DesignSystem::textField().contentsMargins().left()
                              - Ui::DesignSystem::textField().margins().left()
                              - Ui::DesignSystem::textField().margins().right()
                              - Ui::DesignSystem::textField().contentsMargins().right(),
                              Ui::DesignSystem::textField().helperHeight());
        const QString text = !d->error.isEmpty() ? d->error : d->helper;
        painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text);
    }
    //
    painter.end();
}

void TextField::enterEvent(QEvent* _event)
{
    QTextEdit::enterEvent(_event);
    update();
}

void TextField::leaveEvent(QEvent* _event)
{
    QTextEdit::leaveEvent(_event);
    update();
}

void TextField::focusInEvent(QFocusEvent* _event)
{
    QTextEdit::focusInEvent(_event);

    d->labelColorAnimation.setStartValue(Ui::DesignSystem::textField().foregroundColor());
    d->labelColorAnimation.setEndValue(Ui::DesignSystem::color().secondary());
    d->labelColorAnimation.start();

    if (text().isEmpty() && placeholderText().isEmpty()) {
        d->animateLabelToTop();
    }

    const QRectF decorationRect = d->decorationRectInFocus(size());
    const qreal contentsWidth = (width()
                                 - Ui::DesignSystem::textField().contentsMargins().left()
                                 - Ui::DesignSystem::textField().contentsMargins().right()) / 2;
    d->decorationAnimation.setStartValue(decorationRect.adjusted(contentsWidth, 0, -1 * contentsWidth, 0));
    d->decorationAnimation.setEndValue(decorationRect);
    d->decorationAnimation.start();
}

void TextField::focusOutEvent(QFocusEvent* _event)
{
    QTextEdit::focusOutEvent(_event);

    d->labelColorAnimation.setStartValue(Ui::DesignSystem::color().secondary());
    d->labelColorAnimation.setEndValue(Ui::DesignSystem::textField().foregroundColor());
    d->labelColorAnimation.start();

    if (text().isEmpty() && placeholderText().isEmpty()) {
        d->animateLabelToBottom();
    }

    const QRectF decorationRect = d->decorationRectInFocus(size());
    d->decorationAnimation.setStartValue(decorationRect);
    const qreal contentsWidth = (width()
                                 - Ui::DesignSystem::textField().contentsMargins().left()
                                 - Ui::DesignSystem::textField().contentsMargins().right()) / 2;
    d->decorationAnimation.setEndValue(decorationRect.adjusted(contentsWidth, 0, -1 * contentsWidth, 0));
    d->decorationAnimation.start();
}

void TextField::mouseReleaseEvent(QMouseEvent* _event)
{
    const QRectF iconRect(QPointF(width()
                                  - Ui::DesignSystem::textField().contentsMargins().right()
                                  - Ui::DesignSystem::textField().margins().right()
                                  - Ui::DesignSystem::textField().iconSize().width(),
                                  Ui::DesignSystem::textField().iconTop()),
                          Ui::DesignSystem::textField().iconSize());
    if (iconRect.contains(_event->pos())) {
        emit trailingIconPressed();
        _event->accept();
    } else {
        QTextEdit::mouseReleaseEvent(_event);
    }
}

void TextField::keyPressEvent(QKeyEvent* _event)
{
    if ((_event->key() == Qt::Key_Enter
            || _event->key() == Qt::Key_Return)
            && !d->isEnterMakesNewLine) {
        _event->ignore();
        return;
    }

    QTextEdit::keyPressEvent(_event);
}

TextField::~TextField() = default;
