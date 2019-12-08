#include "design_system.h"

#include <utils/helpers/color_helper.h>

#include <QApplication>
#include <QColor>
#include <QEvent>
#include <QFont>
#include <QFontMetricsF>
#include <QJsonDocument>
#include <QJsonObject>
#include <QIcon>
#include <QMarginsF>
#include <QPixmap>
#include <QPointF>
#include <QSizeF>

#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#else
#include <QScreen>
#endif


namespace Ui
{

class DesignSystem::AppBarPrivate
{
public:
    explicit AppBarPrivate(qreal _scaleFactor);

    QMarginsF margins = {16.0, 16.0, 16.0, 16.0};
    QMarginsF bigIconMargins = {0.0, 8.0, 16.0, 8.0};
    qreal heightRegular = 56.0;
    QSizeF iconSize = {24.0, 24.0};
    QSizeF bigIconSize = {40.0, 40.0};
    qreal iconsSpacing = 24.0;
    qreal leftTitleMargin = 72.0;
    qreal shadowRadius = 12.0;
    QPointF shadowOffset = {0.0, 3.0};
    QFont font;
};

DesignSystem::AppBarPrivate::AppBarPrivate(qreal _scaleFactor)
{
    margins *= _scaleFactor;
    bigIconMargins *= _scaleFactor;
    heightRegular *= _scaleFactor;
    iconSize *= _scaleFactor;
    bigIconSize *= _scaleFactor;
    iconsSpacing *= _scaleFactor;
    leftTitleMargin *= _scaleFactor;
    shadowRadius *= _scaleFactor;
    shadowOffset *= _scaleFactor;
    font = QFont("Roboto");
    font.setPixelSize(static_cast<int>(20.0 * _scaleFactor));
    font.setWeight(QFont::Normal);
}

// **

DesignSystem::AppBar::~AppBar() = default;

QMarginsF DesignSystem::AppBar::margins() const
{
    return d->margins;
}

QMarginsF DesignSystem::AppBar::bigIconMargins() const
{
    return d->bigIconMargins;
}

qreal DesignSystem::AppBar::heightRegular() const
{
    return d->heightRegular;
}

QSizeF DesignSystem::AppBar::iconSize() const
{
    return d->iconSize;
}

QSizeF DesignSystem::AppBar::bigIconSize() const
{
    return d->bigIconSize;
}

qreal DesignSystem::AppBar::iconsSpacing() const
{
    return d->iconsSpacing;
}

qreal DesignSystem::AppBar::leftTitleMargin() const
{
    return d->leftTitleMargin;
}

qreal DesignSystem::AppBar::shadowRadius() const
{
    return d->shadowRadius;
}

QPointF DesignSystem::AppBar::shadowOffset() const
{
    return d->shadowOffset;
}

QFont DesignSystem::AppBar::font() const
{
    return d->font;
}

DesignSystem::AppBar::AppBar(qreal _scaleFactor)
    : d(new AppBarPrivate(_scaleFactor))
{
}


// ****


class DesignSystem::TabPrivate
{
public:
    explicit TabPrivate(qreal _scaleFactor);

    qreal minimumWidth = 90.0;
    qreal heightWithText = 48.0;
    qreal heightWithIcon = 48.0;
    qreal heightWithTextAndIcon = 72.0;
    QMarginsF margins = {16.0, 12.0, 16.0, 12.0};
    QSizeF iconSize = {24.0, 24.0};
};

DesignSystem::TabPrivate::TabPrivate(qreal _scaleFactor)
{
    minimumWidth *= _scaleFactor;
    heightWithText *= _scaleFactor;
    heightWithIcon *= _scaleFactor;
    heightWithTextAndIcon *= _scaleFactor;
    margins *= _scaleFactor;
    iconSize *= _scaleFactor;
}

// **

DesignSystem::Tab::~Tab() = default;

qreal DesignSystem::Tab::minimumWidth() const
{
    return d->minimumWidth;
}

qreal DesignSystem::Tab::heightWithText() const
{
    return d->heightWithText;
}

qreal DesignSystem::Tab::heightWithIcon() const
{
    return d->heightWithIcon;
}

qreal DesignSystem::Tab::heightWithTextAndIcon() const
{
    return d->heightWithTextAndIcon;
}

QMarginsF DesignSystem::Tab::margins() const
{
    return d->margins;
}

QSizeF DesignSystem::Tab::iconSize() const
{
    return d->iconSize;
}

DesignSystem::Tab::Tab(qreal _scaleFactor)
    : d(new TabPrivate(_scaleFactor))
{
}


// ****


class DesignSystem::TabsPrivate
{
public:
    explicit TabsPrivate(qreal _scaleFactor);

    qreal scrollableLeftMargin = 52.0;
    qreal underlineHeight = 2.0;
    QFont font;
};

DesignSystem::TabsPrivate::TabsPrivate(qreal _scaleFactor)
{
    scrollableLeftMargin *= _scaleFactor;
    underlineHeight *= _scaleFactor;
    font = QFont("Roboto");
    font.setPixelSize(static_cast<int>(14.0 * _scaleFactor));
    font.setWeight(QFont::Medium);
}

// **

DesignSystem::Tabs::~Tabs() = default;

qreal DesignSystem::Tabs::scrollableLeftMargin() const
{
    return d->scrollableLeftMargin;
}

qreal DesignSystem::Tabs::underlineHeight() const
{
    return d->underlineHeight;
}

QFont DesignSystem::Tabs::font() const
{
    return d->font;
}

DesignSystem::Tabs::Tabs(qreal _scaleFactor)
    : d(new TabsPrivate(_scaleFactor))
{
}


// ****


class DesignSystem::ColorPickerPrivate
{
public:
    explicit ColorPickerPrivate(qreal _scaleFactor);

    QMarginsF margins = {16.0, 16.0, 16.0, 16.0};
    qreal height = 72.0;
    QSizeF iconSize = {40.0, 40.0};
    qreal iconsSpacing = 32.0;
    qreal iconBorderWidth = 1.2;
};

DesignSystem::ColorPickerPrivate::ColorPickerPrivate(qreal _scaleFactor)
{
    margins *= _scaleFactor;
    height *= _scaleFactor;
    iconSize *= _scaleFactor;
    iconsSpacing *= _scaleFactor;
    iconBorderWidth *= _scaleFactor;
}

// **

DesignSystem::ColorPicker::~ColorPicker() = default;

QMarginsF DesignSystem::ColorPicker::margins() const
{
    return d->margins;
}

qreal DesignSystem::ColorPicker::height() const
{
    return d->height;
}

QSizeF DesignSystem::ColorPicker::iconSize() const
{
    return d->iconSize;
}

qreal DesignSystem::ColorPicker::iconsSpacing() const
{
    return d->iconsSpacing;
}

qreal DesignSystem::ColorPicker::iconBorderWidth() const
{
    return d->iconBorderWidth;
}

DesignSystem::ColorPicker::ColorPicker(qreal _scaleFactor)
    : d(new ColorPickerPrivate(_scaleFactor))
{
}


// ****


class DesignSystem::TextTogglePrivate
{
public:
    explicit TextTogglePrivate(qreal _scaleFactor);

    QMarginsF margins = {4.0, 4.0, 4.0, 4.0};
    QSizeF toggleSize = {64.0, 36.0};
    qreal toggleRadius = 17.0;
    qreal spacing = 12.0;
    qreal toggleSpacing = 3.0;
    QFont font;
};

DesignSystem::TextTogglePrivate::TextTogglePrivate(qreal _scaleFactor)
{
    margins *= _scaleFactor;
    toggleSize *= _scaleFactor;
    toggleRadius *= _scaleFactor;
    spacing *= _scaleFactor;
    toggleSpacing *= _scaleFactor;
    font = QFont("Roboto");
    font.setPixelSize(static_cast<int>(14.0 * _scaleFactor));
    font.setWeight(QFont::Medium);
}


DesignSystem::TextToggle::~TextToggle() = default;

QMarginsF DesignSystem::TextToggle::margins() const
{
    return d->margins;
}

QSizeF DesignSystem::TextToggle::toggleSize() const
{
    return d->toggleSize;
}

qreal DesignSystem::TextToggle::toggleRadius() const
{
    return d->toggleRadius;
}

qreal DesignSystem::TextToggle::spacing() const
{
    return d->spacing;
}

qreal DesignSystem::TextToggle::toggleSpacing() const
{
    return d->toggleSpacing;
}

QFont DesignSystem::TextToggle::font() const
{
    return d->font;
}

DesignSystem::TextToggle::TextToggle(qreal _scaleFactor)
    : d(new TextTogglePrivate(_scaleFactor))
{
}


// ****


class DesignSystem::Color::Implementation
{
public:
    Implementation();

    QColor primary;
    QColor primaryDark;
    QColor secondary;
    QColor background;
    QColor surface;
    QColor error;
    QColor shadow;
    QColor onPrimary;
    QColor onSecondary;
    QColor onBackground;
    QColor onSurface;
    QColor onError;
};

DesignSystem::Color::Implementation::Implementation()
{
}


// **


DesignSystem::Color::Color(const DesignSystem::Color& _rhs)
    : d(new Implementation(*_rhs.d))
{
}

DesignSystem::Color::Color(const QString& _color)
    : d(new Implementation)
{
    const auto colorJson = _color;
    int startIndex = 0;
    auto nextColor = [_color, &startIndex] {
        const int length = 6;
        const auto color = _color.mid(startIndex, length);
        startIndex += length;
        return "#" + color;
    };
    setPrimary(nextColor());
    setOnPrimary(nextColor());
    setSecondary(nextColor());
    setOnSecondary(nextColor());
    setBackground(nextColor());
    setOnBackground(nextColor());
    setSurface(nextColor());
    setOnSurface(nextColor());
    setError(nextColor());
    setOnError(nextColor());
    setShadow(nextColor());
    setPrimaryDark(nextColor());
}

DesignSystem::Color& DesignSystem::Color::operator=(const DesignSystem::Color& _rhs)
{
    if (&_rhs == this) {
        return *this;
    }

    d.reset(new Implementation(*_rhs.d));
    return *this;
}

QString DesignSystem::Color::toString() const
{
    QString  colorsString;
    colorsString += d->primary.name();
    colorsString += d->onPrimary.name();
    colorsString += d->secondary.name();
    colorsString += d->onSecondary.name();
    colorsString += d->background.name();
    colorsString += d->onBackground.name();
    colorsString += d->surface.name();
    colorsString += d->onSurface.name();
    colorsString += d->error.name();
    colorsString += d->onError.name();
    colorsString += d->shadow.name();
    colorsString += d->primaryDark.name();
    return colorsString.remove('#');
}

DesignSystem::Color::~Color() = default;

const QColor& DesignSystem::Color::primary() const
{
    return d->primary;
}

const QColor& DesignSystem::Color::primaryDark() const
{
    return d->primaryDark;
}

const QColor& DesignSystem::Color::secondary() const
{
    return d->secondary;
}

const QColor& DesignSystem::Color::background() const
{
    return d->background;
}

const QColor& DesignSystem::Color::surface() const
{
    return d->surface;
}

const QColor& DesignSystem::Color::error() const
{
    return d->error;
}

const QColor& DesignSystem::Color::shadow() const
{
    return d->shadow;
}

const QColor& DesignSystem::Color::onPrimary() const
{
    return d->onPrimary;
}

const QColor& DesignSystem::Color::onSecondary() const
{
    return d->onSecondary;
}

const QColor& DesignSystem::Color::onBackground() const
{
    return d->onBackground;
}

const QColor& DesignSystem::Color::onSurface() const
{
    return d->onSurface;
}

const QColor& DesignSystem::Color::onError() const
{
    return d->onError;
}

void DesignSystem::Color::setPrimary(const QColor& _color)
{
    d->primary = _color;
}

void DesignSystem::Color::setPrimaryDark(const QColor& _color)
{
    d->primaryDark = _color;
}

void DesignSystem::Color::setSecondary(const QColor& _color)
{
    d->secondary = _color;
}

void DesignSystem::Color::setBackground(const QColor& _color)
{
    d->background = _color;
}

void DesignSystem::Color::setSurface(const QColor& _color)
{
    d->surface = _color;
}

void DesignSystem::Color::setError(const QColor& _color)
{
    d->error = _color;
}

void DesignSystem::Color::setShadow(const QColor& _color)
{
    d->shadow = _color;

    if (d->shadow.alphaF() < 1.0) {
        return;
    }

    //
    // Если прозрачность не задана, настроим её вручную в зависимости от цвета фона темы
    //
    d->shadow.setAlphaF(ColorHelper::isColorLight(d->surface) ? 0.36 : 0.68);
}

void DesignSystem::Color::setOnPrimary(const QColor& _color)
{
    d->onPrimary = _color;
}

void DesignSystem::Color::setOnSecondary(const QColor& _color)
{
    d->onSecondary = _color;
}

void DesignSystem::Color::setOnBackground(const QColor& _color)
{
    d->onBackground = _color;
}

void DesignSystem::Color::setOnSurface(const QColor& _color)
{
    d->onSurface = _color;
}

void DesignSystem::Color::setOnError(const QColor& _color)
{
    d->onError = _color;
}

DesignSystem::Color::Color()
    : d(new Implementation)
{
}


// ****


class DesignSystem::Font::Implementation
{
public:
    explicit Implementation(qreal _scaleFactor);

    QFont h1 = QFont("Roboto");
    QFont h2 = QFont("Roboto");
    QFont h3 = QFont("Roboto");
    QFont h4 = QFont("Roboto");
    QFont h5 = QFont("Roboto");
    QFont h6 = QFont("Roboto");
    QFont subtitle1 = QFont("Roboto");
    QFont subtitle2 = QFont("Roboto");
    QFont body1 = QFont("Roboto");
    QFont body2 = QFont("Roboto");
    QFont button = QFont("Roboto");
    QFont caption = QFont("Roboto");
    QFont overline = QFont("Roboto");

    QFont iconsSmall = QFont("Material Design Icons");
    QFont iconsMid = QFont("Material Design Icons");
};

DesignSystem::Font::Implementation::Implementation(qreal _scaleFactor)
{
    auto initFont = [_scaleFactor] (QFont::Weight _weight, QFont::Capitalization _capitalization,
            int _pixelSize, qreal _letterSpacing, QFont& _font) {
        _font.setWeight(_weight);
        _font.setCapitalization(_capitalization);
        _font.setPixelSize(static_cast<int>(_pixelSize * _scaleFactor));
        _font.setLetterSpacing(QFont::AbsoluteSpacing, _letterSpacing * _scaleFactor);
    };

    initFont(QFont::Light, QFont::Capitalization::MixedCase, 96, -1.5, h1);
    initFont(QFont::Light, QFont::Capitalization::MixedCase, 60, -0.5, h2);
    initFont(QFont::Normal, QFont::Capitalization::MixedCase, 48, 0.0, h3);
    initFont(QFont::Normal, QFont::Capitalization::MixedCase, 34, 0.25, h4);
    initFont(QFont::Normal, QFont::Capitalization::MixedCase, 24, 0.0, h5);
    initFont(QFont::Medium, QFont::Capitalization::MixedCase, 20, 0.15, h6);
    initFont(QFont::Normal, QFont::Capitalization::MixedCase, 16, 0.15, subtitle1);
    initFont(QFont::Medium, QFont::Capitalization::MixedCase, 14, 0.1, subtitle2);
    initFont(QFont::Normal, QFont::Capitalization::MixedCase, 16, 0.5, body1);
    initFont(QFont::Normal, QFont::Capitalization::MixedCase, 14, 0.25, body2);
    initFont(QFont::Medium, QFont::Capitalization::AllUppercase, 14, 1.25, button);
    initFont(QFont::Normal, QFont::Capitalization::MixedCase, 12, 0.4, caption);
    initFont(QFont::Normal, QFont::Capitalization::AllUppercase, 10, 1.5, overline);

    iconsSmall.setPixelSize(static_cast<int>(16 * _scaleFactor));
    iconsMid.setPixelSize(static_cast<int>(24 * _scaleFactor));
}

// **

DesignSystem::Font::~Font() = default;

const QFont& DesignSystem::Font::h1() const
{
    return d->h1;
}

const QFont& DesignSystem::Font::h2() const
{
    return d->h2;
}

const QFont& DesignSystem::Font::h3() const
{
    return d->h3;
}

const QFont& DesignSystem::Font::h4() const
{
    return d->h4;
}

const QFont& DesignSystem::Font::h5() const
{
    return d->h5;
}

const QFont&DesignSystem::Font::h6() const
{
    return d->h6;
}

const QFont& DesignSystem::Font::subtitle1() const
{
    return d->subtitle1;
}

const QFont& DesignSystem::Font::subtitle2() const
{
    return d->subtitle2;
}

const QFont& DesignSystem::Font::body1() const
{
    return d->body1;
}

const QFont& DesignSystem::Font::body2() const
{
    return d->body2;
}

const QFont& DesignSystem::Font::button() const
{
    return d->button;
}

const QFont& DesignSystem::Font::caption() const
{
    return d->caption;
}

const QFont& DesignSystem::Font::overline() const
{
    return d->overline;
}

const QFont& DesignSystem::Font::iconsSmall() const
{
    return d->iconsSmall;
}

const QFont& DesignSystem::Font::iconsMid() const
{
    return d->iconsMid;
}

DesignSystem::Font::Font(qreal _scaleFactor)
    : d(new Implementation(_scaleFactor))
{
}


// ****


class DesignSystem::Layout::Implementation
{
public:
    explicit Implementation(qreal _scaleFactor);

    qreal px2 = 2.0;
    qreal px4 = 4.0;
    qreal px8 = 8.0;
    qreal px12 = 12.0;
    qreal px16 = 16.0;
    qreal px24 = 24.0;
    qreal px62 = 62.0;
    qreal topContentMargin = 116.0;
    qreal buttonsSpacing = 8.0;
};

DesignSystem::Layout::Implementation::Implementation(qreal _scaleFactor)
{
    px2 *= _scaleFactor;
    px4 *= _scaleFactor;
    px8 *= _scaleFactor;
    px12 *= _scaleFactor;
    px16 *= _scaleFactor;
    px24 *= _scaleFactor;
    px62 *= _scaleFactor;
    topContentMargin *= _scaleFactor;
    buttonsSpacing *= _scaleFactor;
}


// **


DesignSystem::Layout::~Layout() = default;

qreal DesignSystem::Layout::px2() const
{
    return d->px2;
}

qreal DesignSystem::Layout::px4() const
{
    return d->px4;
}

qreal DesignSystem::Layout::px8() const
{
    return d->px8;
}

qreal DesignSystem::Layout::px12() const
{
    return d->px12;
}

qreal DesignSystem::Layout::px16() const
{
    return d->px16;
}

qreal DesignSystem::Layout::px24() const
{
    return d->px24;
}

qreal DesignSystem::Layout::px62() const
{
    return d->px62;
}

qreal DesignSystem::Layout::topContentMargin() const
{
    return d->topContentMargin;
}

qreal DesignSystem::Layout::buttonsSpacing() const
{
    return d->buttonsSpacing;
}

DesignSystem::Layout::Layout(qreal _scaleFactor)
    : d(new Implementation(_scaleFactor))
{
}


// ****


class DesignSystem::Label::Implementation
{
public:
    explicit Implementation(qreal _scaleFactor);

    QMarginsF margins = {24.0, 24.0, 24.0, 24.0};
};

DesignSystem::Label::Implementation::Implementation(qreal _scaleFactor)
{
    margins *= _scaleFactor;
}


// **


DesignSystem::Label::~Label() = default;

const QMarginsF& DesignSystem::Label::margins() const
{
    return d->margins;
}

DesignSystem::Label::Label(qreal _scaleFactor)
    : d(new Implementation(_scaleFactor))
{
}


// ****


class DesignSystem::Button::Implementation
{
public:
    explicit Implementation(qreal _scaleFactor);

    qreal height = 38.0;
    qreal minimumWidth = 64.0;
    QMarginsF margins = {16.0, 0.0, 16.0, 0.0};
    qreal spacing = 16.0;
    QMarginsF shadowMargins = {8.0, 8.0, 8.0, 10.0};
    qreal minimumShadowBlurRadius = 8.0;
    qreal maximumShadowBlurRadius = 22.0;
    qreal borderRadius = 4.0;
    QSizeF iconSize = {22.0, 22.0};
};

DesignSystem::Button::Implementation::Implementation(qreal _scaleFactor)
{
    height *= _scaleFactor;
    minimumWidth *= _scaleFactor;
    margins *= _scaleFactor;
    spacing *= _scaleFactor;
    shadowMargins *= _scaleFactor;
    minimumShadowBlurRadius *= _scaleFactor;
    maximumShadowBlurRadius *= _scaleFactor;
    borderRadius *= _scaleFactor;
    iconSize *= _scaleFactor;
}


// **


DesignSystem::Button::~Button() = default;

qreal DesignSystem::Button::height() const
{
    return d->height;
}

qreal DesignSystem::Button::minimumWidth() const
{
    return d->minimumWidth;
}

const QMarginsF& DesignSystem::Button::margins() const
{
    return d->margins;
}

qreal DesignSystem::Button::spacing() const
{
    return d->spacing;
}

const QMarginsF& DesignSystem::Button::shadowMargins() const
{
    return d->shadowMargins;
}

qreal DesignSystem::Button::minimumShadowBlurRadius() const
{
    return  d->minimumShadowBlurRadius;
}

qreal DesignSystem::Button::maximumShadowBlurRadius() const
{
    return d->maximumShadowBlurRadius;
}

qreal DesignSystem::Button::borderRadius() const
{
    return d->borderRadius;
}

const QSizeF& DesignSystem::Button::iconSize() const
{
    return d->iconSize;
}

DesignSystem::Button::Button(qreal _scaleFactor)
    : d(new Implementation(_scaleFactor))
{
}


// ****


class DesignSystem::ToggleButton::Implementation
{
public:
    explicit Implementation(qreal _scaleFactor);

    QSizeF size = {48.0, 48.0};
    QMarginsF margins = {12.0, 12.0, 12.0, 12.0};
    QSizeF iconSize = {24.0, 24.0};
};

DesignSystem::ToggleButton::Implementation::Implementation(qreal _scaleFactor)
{
    size *= _scaleFactor;
    margins *= _scaleFactor;
    iconSize *= _scaleFactor;
}


// **


DesignSystem::ToggleButton::~ToggleButton() = default;

const QSizeF& DesignSystem::ToggleButton::size() const
{
    return d->size;
}

const QMarginsF& DesignSystem::ToggleButton::margins() const
{
    return d->margins;
}

const QSizeF& DesignSystem::ToggleButton::iconSize() const
{
    return d->iconSize;
}

DesignSystem::ToggleButton::ToggleButton(qreal _scaleFactor)
    : d(new Implementation(_scaleFactor))
{
}


// ****


class DesignSystem::RadioButton::Implementation
{
public:
    explicit Implementation(qreal _scaleFactor);

    qreal height = 48.0;
    QMarginsF margins = {24.0, 13.0, 24.0, 13.0};
    QSizeF iconSize = {22.0, 22.0};
    qreal spacing = 16.0;

};

DesignSystem::RadioButton::Implementation::Implementation(qreal _scaleFactor)
{
    height *= _scaleFactor;
    margins *= _scaleFactor;
    iconSize *= _scaleFactor;
    spacing *= _scaleFactor;
}

// **

DesignSystem::RadioButton::~RadioButton() = default;

qreal DesignSystem::RadioButton::height() const
{
    return d->height;
}

const QMarginsF& DesignSystem::RadioButton::margins() const
{
    return d->margins;
}

const QSizeF& DesignSystem::RadioButton::iconSize() const
{
    return d->iconSize;
}

qreal DesignSystem::RadioButton::spacing() const
{
    return d->spacing;
}

DesignSystem::RadioButton::RadioButton(qreal _scaleFactor)
    : d(new Implementation(_scaleFactor))
{
}


// ****


class DesignSystem::CheckBox::Implementation
{
public:
    explicit Implementation(qreal _scaleFactor);

    qreal height = 48.0;
    QMarginsF margins = {24.0, 13.0, 24.0, 13.0};
    QSizeF iconSize = {22.0, 22.0};
    qreal spacing = 16.0;

};

DesignSystem::CheckBox::Implementation::Implementation(qreal _scaleFactor)
{
    height *= _scaleFactor;
    margins *= _scaleFactor;
    iconSize *= _scaleFactor;
    spacing *= _scaleFactor;
}

// **

DesignSystem::CheckBox::~CheckBox() = default;

qreal DesignSystem::CheckBox::height() const
{
    return d->height;
}

const QMarginsF& DesignSystem::CheckBox::margins() const
{
    return d->margins;
}

const QSizeF& DesignSystem::CheckBox::iconSize() const
{
    return d->iconSize;
}

qreal DesignSystem::CheckBox::spacing() const
{
    return d->spacing;
}

DesignSystem::CheckBox::CheckBox(qreal _scaleFactor)
    : d(new Implementation(_scaleFactor))
{
}


// ****


class DesignSystem::Slider::Implementation
{
public:
    explicit Implementation(qreal _scaleFactor);

    qreal height = 48.0;
    qreal thumbRadius = 6.0;
    qreal trackHeight = 2.0;
    qreal unfilledPartOpacity = 0.24;
};

DesignSystem::Slider::Implementation::Implementation(qreal _scaleFactor)
{
    height *= _scaleFactor;
    thumbRadius *= _scaleFactor;
    trackHeight *= _scaleFactor;
}


// **


DesignSystem::Slider::~Slider() = default;

qreal DesignSystem::Slider::height() const
{
    return d->height;
}

qreal DesignSystem::Slider::thumbRadius() const
{
    return d->thumbRadius;
}

qreal DesignSystem::Slider::trackHeight() const
{
    return d->trackHeight;
}

qreal DesignSystem::Slider::unfilledPartOpacity() const
{
    return d->unfilledPartOpacity;
}

DesignSystem::Slider::Slider(qreal _scaleFactor)
    : d(new Implementation(_scaleFactor))
{
}


// ****


class DesignSystem::ProgressBar::Implementation
{
public:
    explicit Implementation(qreal _scaleFactor);

    qreal linearTrackHeight = 2.0;
    qreal circularTrackHeight = 8.0;
    qreal unfilledPartOpacity = 0.24;
};

DesignSystem::ProgressBar::Implementation::Implementation(qreal _scaleFactor)
{
    linearTrackHeight *= _scaleFactor;
    circularTrackHeight *= _scaleFactor;
}


// **


DesignSystem::ProgressBar::~ProgressBar() = default;

qreal DesignSystem::ProgressBar::linearTrackHeight() const
{
    return d->linearTrackHeight;
}

qreal DesignSystem::ProgressBar::circularTrackHeight() const
{
    return d->circularTrackHeight;
}

qreal DesignSystem::ProgressBar::unfilledPartOpacity() const
{
    return d->unfilledPartOpacity;
}

DesignSystem::ProgressBar::ProgressBar(qreal _scaleFactor)
    : d(new Implementation(_scaleFactor))
{
}


// ****


class DesignSystem::TextField::Implementation
{
public:
    explicit Implementation(qreal _scaleFactor, const Color& _color);

    QColor backgroundInactiveColor;
    QColor backgroundActiveColor;
    QColor foregroundColor;
    QMarginsF contentsMargins = {24.0, 0.0, 24.0, 0.0};
    QMarginsF margins = {12.0, 26.0, 12.0, 12.0};
    QPointF labelTopLeft = {12.0, 6.0};
    qreal iconTop = 16.0;
    QSizeF iconSize = {24.0, 24.0};
    qreal underlineHeight = 1.0;
    qreal underlineHeightInFocus = 2.0;
    qreal helperHeight = 20.0;
};

DesignSystem::TextField::Implementation::Implementation(qreal _scaleFactor, const Color& _color)
{
    backgroundInactiveColor = _color.onSurface();
    backgroundInactiveColor.setAlphaF(0.06);
    backgroundActiveColor = _color.onSurface();
    backgroundActiveColor.setAlphaF(0.10);
    foregroundColor = _color.onSurface();
    foregroundColor.setAlphaF(0.4);
    contentsMargins *= _scaleFactor;
    margins *= _scaleFactor;
    labelTopLeft *= _scaleFactor;
    iconTop *= _scaleFactor;
    iconSize *= _scaleFactor;
    underlineHeight *= _scaleFactor;
    underlineHeightInFocus *= _scaleFactor;
    helperHeight *= _scaleFactor;
}

// **

DesignSystem::TextField::~TextField() = default;

QColor DesignSystem::TextField::backgroundInactiveColor() const
{
    return d->backgroundInactiveColor;
}

QColor DesignSystem::TextField::backgroundActiveColor() const
{
    return d->backgroundActiveColor;
}

QColor DesignSystem::TextField::foregroundColor() const
{
    return d->foregroundColor;
}

QMarginsF DesignSystem::TextField::contentsMargins() const
{
    return d->contentsMargins;
}

QMarginsF DesignSystem::TextField::margins() const
{
    return d->margins;
}

QPointF DesignSystem::TextField::labelTopLeft() const
{
    return d->labelTopLeft;
}

qreal DesignSystem::TextField::iconTop() const
{
    return d->iconTop;
}

QSizeF DesignSystem::TextField::iconSize() const
{
    return d->iconSize;
}

qreal DesignSystem::TextField::underlineHeight() const
{
    return d->underlineHeight;
}

qreal DesignSystem::TextField::underlineHeightInFocus() const
{
    return d->underlineHeightInFocus;
}

qreal DesignSystem::TextField::helperHeight() const
{
    return d->helperHeight;
}

DesignSystem::TextField::TextField(qreal _scaleFactor, const Color& _color)
    : d(new Implementation(_scaleFactor, _color))
{
}


// ****


class DesignSystem::ScrollBar::Implementation
{
public:
    explicit Implementation(qreal _scaleFactor, const Color& _color);

    QColor backgroundColor;
    QColor handleColor;
    QMarginsF margins = {2.0, 2.0, 2.0, 2.0};
    qreal minimumSize = 6.0;
    qreal maximumSize = 18.0;
    qreal minimumHandleLength = 26.0;
};

DesignSystem::ScrollBar::Implementation::Implementation(qreal _scaleFactor, const Color& _color)
{
    backgroundColor = _color.onSurface();
    backgroundColor.setAlphaF(0.08);
    handleColor = _color.onSurface();
    handleColor.setAlphaF(0.24);
    margins *= _scaleFactor;
    minimumSize *= _scaleFactor;
    maximumSize *= _scaleFactor;
}

// **

DesignSystem::ScrollBar::~ScrollBar() = default;

QColor DesignSystem::ScrollBar::backgroundColor() const
{
    return d->backgroundColor;
}

QColor DesignSystem::ScrollBar::handleColor() const
{
    return d->handleColor;
}

QMarginsF DesignSystem::ScrollBar::margins() const
{
    return d->margins;
}

qreal DesignSystem::ScrollBar::minimumSize() const
{
    return d->minimumSize;
}

qreal DesignSystem::ScrollBar::maximumSize() const
{
    return d->maximumSize;
}

qreal DesignSystem::ScrollBar::minimumHandleLength() const
{
    return d->minimumHandleLength;
}

DesignSystem::ScrollBar::ScrollBar(qreal _scaleFactor, const Color& _color)
    : d(new Implementation(_scaleFactor, _color))
{
}


// ****


class DesignSystem::FloatingToolBar::Implementation
{
public:
    explicit Implementation(qreal _scaleFactor);

    QMarginsF margins = {16.0, 16.0, 16.0, 16.0};
    QMarginsF shadowMargins = {14.0, 14.0, 14.0, 16.0};
    qreal minimumShadowBlurRadius = 8.0;
    qreal maximumShadowBlurRadius = 32.0;
    qreal borderRadius = 4.0;
    qreal height = 56.0;
    QSizeF iconSize = {24.0, 24.0};
    qreal spacing = 24.0;
};

DesignSystem::FloatingToolBar::Implementation::Implementation(qreal _scaleFactor)
{
    margins *= _scaleFactor;
    shadowMargins *= _scaleFactor;
    minimumShadowBlurRadius *= _scaleFactor;
    maximumShadowBlurRadius *= _scaleFactor;
    height *= _scaleFactor;
    iconSize *= _scaleFactor;
    spacing *= _scaleFactor;
}


// **


DesignSystem::FloatingToolBar::~FloatingToolBar() = default;

const QMarginsF& DesignSystem::FloatingToolBar::margins() const
{
    return d->margins;
}

const QMarginsF& DesignSystem::FloatingToolBar::shadowMargins() const
{
    return d->shadowMargins;
}

qreal DesignSystem::FloatingToolBar::minimumShadowBlurRadius() const
{
    return d->minimumShadowBlurRadius;
}

qreal DesignSystem::FloatingToolBar::maximumShadowBlurRadius() const
{
    return d->maximumShadowBlurRadius;
}

qreal DesignSystem::FloatingToolBar::height() const
{
    return d->height;
}

const QSizeF& DesignSystem::FloatingToolBar::iconSize() const
{
    return d->iconSize;
}

qreal DesignSystem::FloatingToolBar::spacing() const
{
    return d->spacing;
}

DesignSystem::FloatingToolBar::FloatingToolBar(qreal _scaleFactor)
    : d(new Implementation(_scaleFactor))
{
}


// ****


class DesignSystem::Stepper::Implementation
{
public:
    explicit Implementation(qreal _scaleFactor);

    qreal stepHeight = 72.0;
    QMarginsF margins = {24.0, 24.0, 24.0, 24.0};
    QSizeF iconSize = {24.0, 24.0};
    qreal spacing = 12.0;
    qreal pathSpacing = 8.0;
    qreal pathWidth = 2.0;
};

DesignSystem::Stepper::Implementation::Implementation(qreal _scaleFactor)
{
    stepHeight *= _scaleFactor;
    margins *= _scaleFactor;
    iconSize *= _scaleFactor;
    spacing *= _scaleFactor;
    pathSpacing *= _scaleFactor;
    pathWidth *= _scaleFactor;
}

// **

DesignSystem::Stepper::~Stepper() = default;

qreal DesignSystem::Stepper::height() const
{
    return d->stepHeight;
}

QMarginsF DesignSystem::Stepper::margins() const
{
    return d->margins;
}

QSizeF DesignSystem::Stepper::iconSize() const
{
    return d->iconSize;
}

qreal DesignSystem::Stepper::spacing() const
{
    return d->spacing;
}

qreal DesignSystem::Stepper::pathSpacing() const
{
    return d->pathSpacing;
}

qreal DesignSystem::Stepper::pathWidth() const
{
    return d->pathWidth;
}

DesignSystem::Stepper::Stepper(qreal _scaleFactor)
    : d(new Implementation(_scaleFactor))
{
}


// ****


class DesignSystem::Drawer::Implementation
{
public:
    explicit Implementation(qreal _scaleFactor, const Color& _color);

    QMarginsF margins = {16.0, 16.0, 16.0, 16.0};
    QMarginsF actionMargins = {16.0, 12.0, 12.0, 12.0};
    QMarginsF selectionMargins = {8.0, 4.0, 8.0, 4.0};
    qreal subtitleBottomMargin = 18.0;
    qreal iconRightMargin = 32.0;
    qreal width = 304.0;
    qreal titleHeight = 26.0;
    qreal subtitleHeight = 20.0;
    qreal actionHeight = 48.0;
    QSizeF iconSize = {24.0, 24.0};
    qreal separatorHeight = 0.5;
    qreal separatorSpacing = 8.0;
    QColor selectionColor;
};

DesignSystem::Drawer::Implementation::Implementation(qreal _scaleFactor, const Color& _color)
{
    margins *= _scaleFactor;
    actionMargins *= _scaleFactor;
    selectionMargins *= _scaleFactor;
    subtitleBottomMargin *= _scaleFactor;
    iconRightMargin *= _scaleFactor;
    width *= _scaleFactor;
    titleHeight *= _scaleFactor;
    subtitleHeight *= _scaleFactor;
    actionHeight *= _scaleFactor;
    iconSize *= _scaleFactor;
    separatorHeight *= _scaleFactor;
    separatorSpacing *= _scaleFactor;
    selectionColor = _color.secondary();
    selectionColor.setAlphaF(0.14);
}

// **

DesignSystem::Drawer::~Drawer() = default;

QMarginsF DesignSystem::Drawer::margins() const
{
    return d->margins;
}

QMarginsF DesignSystem::Drawer::actionMargins() const
{
    return d->actionMargins;
}

QMarginsF DesignSystem::Drawer::selectionMargins() const
{
    return d->selectionMargins;
}

qreal DesignSystem::Drawer::subtitleBottomMargin() const
{
    return d->subtitleBottomMargin;
}

qreal DesignSystem::Drawer::iconRightMargin() const
{
    return d->iconRightMargin;
}

qreal DesignSystem::Drawer::width() const
{
    return d->width;
}

qreal DesignSystem::Drawer::titleHeight() const
{
    return d->titleHeight;
}

qreal DesignSystem::Drawer::subtitleHeight() const
{
    return d->subtitleHeight;
}

qreal DesignSystem::Drawer::actionHeight() const
{
    return d->actionHeight;
}

QSizeF DesignSystem::Drawer::iconSize() const
{
    return d->iconSize;
}

qreal DesignSystem::Drawer::separatorHeight() const
{
    return d->separatorHeight;
}

qreal DesignSystem::Drawer::separatorSpacing() const
{
    return d->separatorSpacing;
}

QColor DesignSystem::Drawer::selectionColor() const
{
    return d->selectionColor;
}

DesignSystem::Drawer::Drawer(qreal _scaleFactor, const Color& _color)
    : d(new Implementation(_scaleFactor, _color))
{
}


// ****


class DesignSystem::TreeOneLineItem::Implementation
{
public:
    explicit Implementation(qreal _scaleFactor);

    QMarginsF margins = {16.0, 16.0, 16.0, 16.0};
    qreal height = 48.0;
    qreal spacing = 16.0;
    QSizeF iconSize = {24.0, 24.0};
};

DesignSystem::TreeOneLineItem::Implementation::Implementation(qreal _scaleFactor)
{
    margins *= _scaleFactor;
    height *= _scaleFactor;
    spacing *= _scaleFactor;
    iconSize *= _scaleFactor;
}


// **


DesignSystem::TreeOneLineItem::~TreeOneLineItem() = default;

const QMarginsF& DesignSystem::TreeOneLineItem::margins() const
{
    return d->margins;
}

qreal DesignSystem::TreeOneLineItem::height() const
{
    return d->height;
}

qreal DesignSystem::TreeOneLineItem::spacing() const
{
    return d->spacing;
}

const QSizeF& DesignSystem::TreeOneLineItem::iconSize() const
{
    return d->iconSize;
}

DesignSystem::TreeOneLineItem::TreeOneLineItem(qreal _scaleFactor)
    : d(new Implementation(_scaleFactor))
{
}


// ****


class DesignSystem::Tree::Implementation
{
public:
    explicit Implementation(qreal _scaleFactor, const Color& _color);

    QMarginsF margins = {0.0, 24.0, 0.0, 24.0};
    qreal indicatorWidth = 40.0;
    qreal arrowHeight = 5.0;
    qreal arrowHalfWidth = 4.0;
    QColor selectionColor;
};

DesignSystem::Tree::Implementation::Implementation(qreal _scaleFactor, const Color& _color)
{
    margins *= _scaleFactor;
    indicatorWidth *= _scaleFactor;
    arrowHeight *= _scaleFactor;
    arrowHalfWidth *= _scaleFactor;
    selectionColor = _color.secondary();
    selectionColor.setAlphaF(0.14);
}

// **

DesignSystem::Tree::~Tree() = default;

const QMarginsF& DesignSystem::Tree::margins() const
{
    return d->margins;
}

qreal DesignSystem::Tree::indicatorWidth() const
{
    return d->indicatorWidth;
}

qreal DesignSystem::Tree::arrowHeight() const
{
    return d->arrowHeight;
}

qreal DesignSystem::Tree::arrowHalfWidth() const
{
    return d->arrowHalfWidth;
}

QColor DesignSystem::Tree::selectionColor() const
{
    return d->selectionColor;
}

DesignSystem::Tree::Tree(qreal _scaleFactor, const Color& _color)
    : d(new Implementation(_scaleFactor, _color))
{
}


// ****


class DesignSystem::Card::Implementation
{
public:
    explicit Implementation(qreal _scaleFactor);

    qreal borderRadius = 4.0;
    QMarginsF shadowMargins = {14.0, 14.0, 14.0, 16.0};
    qreal minimumShadowBlurRadius = 8.0;
    qreal maximumShadowBlurRadius = 32.0;
};

DesignSystem::Card::Implementation::Implementation(qreal _scaleFactor)
{
    borderRadius *= _scaleFactor;
    shadowMargins *= _scaleFactor;
    minimumShadowBlurRadius *= _scaleFactor;
    maximumShadowBlurRadius *= _scaleFactor;
}


// **


DesignSystem::Card::~Card() = default;

qreal DesignSystem::Card::borderRadius() const
{
    return d->borderRadius;
}

const QMarginsF& DesignSystem::Card::shadowMargins() const
{
    return d->shadowMargins;
}

qreal DesignSystem::Card::minimumShadowBlurRadius() const
{
    return d->minimumShadowBlurRadius;
}

qreal DesignSystem::Card::maximumShadowBlurRadius() const
{
    return d->maximumShadowBlurRadius;
}

DesignSystem::Card::Card(qreal _scaleFactor)
    : d(new Implementation(_scaleFactor))
{
}


// ****


class DesignSystem::Dialog::Implementation
{
public:
    explicit Implementation(qreal _scaleFactor);

    QMarginsF margins = {24.0, 24.0, 24.0, 12.0};
    qreal minimumWidth = 420.0;
};

DesignSystem::Dialog::Implementation::Implementation(qreal _scaleFactor)
{
    margins *= _scaleFactor;
    minimumWidth *= _scaleFactor;
}

// **

DesignSystem::Dialog::~Dialog() = default;

const QMarginsF& DesignSystem::Dialog::margins() const
{
    return d->margins;
}

qreal DesignSystem::Dialog::minimumWidth() const
{
    return d->minimumWidth;
}

DesignSystem::Dialog::Dialog(qreal _scaleFactor)
    : d(new Implementation(_scaleFactor))
{
}


// ****

class DesignSystem::ProjectCard::Implementation
{
public:
    explicit Implementation(qreal _scaleFactor);

    QSizeF size = {400, 230};
    QMarginsF margins = {24, 116, 24, 24};
    qreal spacing = 10;
};

DesignSystem::ProjectCard::Implementation::Implementation(qreal _scaleFactor)
{
    size *= _scaleFactor;
    margins *= _scaleFactor;
    spacing *= _scaleFactor;
}


// **


DesignSystem::ProjectCard::~ProjectCard() = default;

const QSizeF& DesignSystem::ProjectCard::size() const
{
    return d->size;
}

const QMarginsF& DesignSystem::ProjectCard::margins() const
{
    return d->margins;
}

qreal DesignSystem::ProjectCard::spacing() const
{
    return d->spacing;
}

DesignSystem::ProjectCard::ProjectCard(qreal _scaleFactor)
    : d(new Implementation(_scaleFactor))
{
}


// ****


class DesignSystemPrivate
{
public:
    explicit DesignSystemPrivate(ApplicationTheme _theme = ApplicationTheme::DarkAndLight,
        qreal _scaleFactor = 1.0, const DesignSystem::Color& _color = {});

    ApplicationTheme theme = ApplicationTheme::DarkAndLight;
    qreal scaleFactor = 1.0;

    QMarginsF pageMargins = {16.0, 26.0, 16.0, 16.0};
    qreal pageSpacing = 16.0;
    qreal inactiveTextOpacity = 0.68;
    qreal disabledTextOpacity = 0.46;
    qreal hoverBackgroundOpacity = 0.06;
    qreal elevationStartOpacity = 0.04;
    qreal elevationEndOpacity = 0.08;

    DesignSystem::AppBar appBar;
    DesignSystem::Tab tab;
    DesignSystem::Tabs tabs;
    DesignSystem::ColorPicker colorPicker;
    DesignSystem::TextToggle textToggle;



    DesignSystem::Color color;
    DesignSystem::Font font;
    DesignSystem::Layout layout;
    DesignSystem::Label label;
    DesignSystem::Button button;
    DesignSystem::ToggleButton toggleButton;
    DesignSystem::RadioButton radioButton;
    DesignSystem::CheckBox checkBox;
    DesignSystem::Slider slider;
    DesignSystem::ProgressBar progressBar;
    DesignSystem::TextField textField;
    DesignSystem::ScrollBar scrollBar;
    DesignSystem::FloatingToolBar floatingAppBar;
    DesignSystem::Stepper stepper;
    DesignSystem::Drawer drawer;
    DesignSystem::TreeOneLineItem treeOneLineItem;
    DesignSystem::Tree tree;
    DesignSystem::Card card;
    DesignSystem::Dialog dialog;
    DesignSystem::ProjectCard projectCard;
};

DesignSystemPrivate::DesignSystemPrivate(ApplicationTheme _theme, qreal _scaleFactor,
    const DesignSystem::Color& _color)
    : theme(_theme),
      scaleFactor(_scaleFactor),
      appBar(_scaleFactor),
      tab(_scaleFactor),
      tabs(_scaleFactor),
      colorPicker(_scaleFactor),
      textToggle(_scaleFactor),
      color(_color),
      font(_scaleFactor),
      layout(_scaleFactor),
      label(_scaleFactor),
      button(_scaleFactor),
      toggleButton(_scaleFactor),
      radioButton(_scaleFactor),
      checkBox(_scaleFactor),
      slider(_scaleFactor),
      progressBar(_scaleFactor),
      textField(_scaleFactor, _color),
      scrollBar(_scaleFactor, _color),
      floatingAppBar(_scaleFactor),
      stepper(_scaleFactor),
      drawer(_scaleFactor, _color),
      treeOneLineItem(_scaleFactor),
      tree(_scaleFactor, _color),
      card(_scaleFactor),
      dialog(_scaleFactor),
      projectCard(_scaleFactor)
{
    pageMargins *= _scaleFactor;
    pageSpacing *= _scaleFactor;
}

// **

ApplicationTheme DesignSystem::theme()
{
    return instance()->d->theme;
}

void DesignSystem::setTheme(ApplicationTheme _theme)
{
    if (instance()->d->theme == _theme) {
        return;
    }

    instance()->d->theme = _theme;

    QColor primary;
    QColor primaryDark;
    QColor secondary;
    QColor background;
    QColor surface;
    QColor error;
    QColor shadow;
    QColor onPrimary;
    QColor onSecondary;
    QColor onBackground;
    QColor onSurface;
    QColor onError;

    switch (_theme) {
        case Ui::ApplicationTheme::DarkAndLight: {
            primary = "#323740";
            primaryDark = "#22252b";
            secondary = "#448AFF";
            background = "#FFFFFF";
            surface = "#FFFFFF";
            error = "#B00020";
            shadow = [] { QColor color = "#000000";
                          color.setAlphaF(0.36);
                          return color; } ();
            onPrimary = "#FFFFFF";
            onSecondary = "#FFFFFF";
            onBackground = "#000000";
            onSurface = "#000000";
            onError = "#FFFFFF";
            break;
        }

        case Ui::ApplicationTheme::Dark: {
            primary = "#1F1F1F";
            primaryDark = "#0A0A0A";
            secondary = "#448AFF";
            background = "#1F1F1F";
            surface = "#121212";
            error = "#CF6679";
            shadow = [] { QColor color = "#000000";
                          color.setAlphaF(0.68);
                          return color; } ();
            onPrimary = "#FFFFFF";
            onSecondary = "#FFFFFF";
            onBackground = "#FFFFFF";
            onSurface = "#FFFFFF";
            onError = "#000000";
            break;
        }

        case Ui::ApplicationTheme::Light: {
            primary = "#E4E4E4";
            primaryDark = "#C8C8C8";
            secondary = "#448AFF";
            background = "#FFFFFF";
            surface = "#FFFFFF";
            error = "#B00020";
            shadow = [] { QColor color = "#000000";
                          color.setAlphaF(0.36);
                          return color; } ();
            onPrimary = "#38393A";
            onSecondary = "#FFFFFF";
            onBackground = "#000000";
            onSurface = "#000000";
            onError = "#FFFFFF";
            break;
        }

        case Ui::ApplicationTheme::Custom: {
            //
            // Кастомная палитра должна следом загрузиться из параметров приложения
            //
            return;
        }
    }

    auto newColor(instance()->d->color);
    newColor.setPrimary(primary);
    newColor.setPrimaryDark(primaryDark);
    newColor.setSecondary(secondary);
    newColor.setBackground(background);
    newColor.setSurface(surface);
    newColor.setError(error);
    newColor.setShadow(shadow);
    newColor.setOnPrimary(onPrimary);
    newColor.setOnSecondary(onSecondary);
    newColor.setOnBackground(onBackground);
    newColor.setOnSurface(onSurface);
    newColor.setOnError(onError);
    setColor(newColor);
}

qreal DesignSystem::scaleFactor()
{
    return instance()->d->scaleFactor;
}

void DesignSystem::setScaleFactor(qreal _scaleFactor)
{
    if (qFuzzyCompare(instance()->d->scaleFactor, _scaleFactor)) {
        return;
    }

    instance()->d.reset(new DesignSystemPrivate(theme(), _scaleFactor, color()));
}

QMarginsF DesignSystem::pageMargins()
{
    return instance()->d->pageMargins;
}

qreal DesignSystem::pageSpacing()
{
    return instance()->d->pageSpacing;
}

qreal DesignSystem::inactiveTextOpacity()
{
    return instance()->d->inactiveTextOpacity;
}

qreal DesignSystem::disabledTextOpacity()
{
    return instance()->d->disabledTextOpacity;
}

qreal DesignSystem::hoverBackgroundOpacity()
{
    return instance()->d->hoverBackgroundOpacity;
}

qreal DesignSystem::elevationStartOpacity()
{
    return instance()->d->elevationStartOpacity;
}

qreal DesignSystem::elevationEndOpacity()
{
    return instance()->d->elevationEndOpacity;
}

const DesignSystem::AppBar& DesignSystem::appBar()
{
    return instance()->d->appBar;
}

const DesignSystem::Tab& DesignSystem::tab()
{
    return instance()->d->tab;
}

const DesignSystem::Tabs& DesignSystem::tabs()
{
    return instance()->d->tabs;
}

const DesignSystem::ColorPicker& DesignSystem::colorPicker()
{
    return instance()->d->colorPicker;
}

const DesignSystem::TextToggle&DesignSystem::textToggle()
{
    return instance()->d->textToggle;
}

const DesignSystem::Color& DesignSystem::color()
{
    return instance()->d->color;
}

void DesignSystem::setColor(const DesignSystem::Color& _color)
{
    instance()->d.reset(new DesignSystemPrivate(theme(), scaleFactor(), _color));
}

const DesignSystem::Font& DesignSystem::font()
{
    return instance()->d->font;
}

const DesignSystem::Layout& DesignSystem::layout()
{
    return instance()->d->layout;
}

const DesignSystem::Label& DesignSystem::label()
{
    return instance()->d->label;
}

const DesignSystem::Button&DesignSystem::button()
{
    return instance()->d->button;
}

const DesignSystem::ToggleButton& DesignSystem::toggleButton()
{
    return instance()->d->toggleButton;
}

const DesignSystem::RadioButton& DesignSystem::radioButton()
{
    return instance()->d->radioButton;
}

const DesignSystem::CheckBox& DesignSystem::checkBox()
{
    return instance()->d->checkBox;
}

const DesignSystem::Slider& DesignSystem::slider()
{
    return instance()->d->slider;
}

const DesignSystem::ProgressBar& DesignSystem::progressBar()
{
    return instance()->d->progressBar;
}

const DesignSystem::TextField& DesignSystem::textField()
{
    return instance()->d->textField;
}

const DesignSystem::ScrollBar& DesignSystem::scrollBar()
{
    return instance()->d->scrollBar;
}

const DesignSystem::FloatingToolBar& DesignSystem::floatingToolBar()
{
    return instance()->d->floatingAppBar;
}

const DesignSystem::Stepper& DesignSystem::stepper()
{
    return instance()->d->stepper;
}

const DesignSystem::Drawer& DesignSystem::drawer()
{
    return instance()->d->drawer;
}

const DesignSystem::TreeOneLineItem& DesignSystem::treeOneLineItem()
{
    return instance()->d->treeOneLineItem;
}

const DesignSystem::Tree& DesignSystem::tree()
{
    return instance()->d->tree;
}

const DesignSystem::Card& DesignSystem::card()
{
    return instance()->d->card;
}

const DesignSystem::Dialog& DesignSystem::dialog()
{
    return instance()->d->dialog;
}

const DesignSystem::ProjectCard& DesignSystem::projectCard()
{
    return instance()->d->projectCard;
}

DesignSystem::~DesignSystem() = default;

DesignSystem::DesignSystem()
    : d(new DesignSystemPrivate)
{
}

DesignSystem* DesignSystem::instance()
{
    static DesignSystem* s_designSystem = new DesignSystem;
    return s_designSystem;
}

} // namespace Ui
