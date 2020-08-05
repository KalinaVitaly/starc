#include "settings_view.h"

#include <ui/design_system/design_system.h>
#include <ui/widgets/button/button.h>
#include <ui/widgets/card/card.h>
#include <ui/widgets/check_box/check_box.h>
#include <ui/widgets/combo_box/combo_box.h>
#include <ui/widgets/label/label.h>
#include <ui/widgets/scroll_bar/scroll_bar.h>
#include <ui/widgets/slider/slider.h>
#include <ui/widgets/text_field/text_field.h>

#include <QFileDialog>
#include <QGridLayout>
#include <QLocale>
#include <QScrollArea>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QVariantAnimation>


namespace Ui
{

namespace {

/**
 * @brief Сформиовать компоновщик для строки настроек
 */
QHBoxLayout* makeLayout() {
    auto layout = new QHBoxLayout;
    layout->setContentsMargins({});
    layout->setSpacing(0);
    return layout;
};

/**
 * @brief Карта соответствия названий языков для проверки орфографии с их кодами
 * @link https://github.com/wooorm/dictionaries/
 * @link https://cgit.freedesktop.org/libreoffice/dictionaries/tree/
 * @link https://www.softmaker.com/en/dictionaries
 */
const QVector<QString> kSpellCheckerLanguagesNameToCode = {
    "af",
    "an",
    "ar",
    "az",
    "be",
    "bg",
    "bn",
    "bo",
    "br",
    "bs",
    "ca-valencia",
    "ca",
    "cs",
    "cy",
    "da",
    "de-AT",
    "de-CH",
    "de",
    "el-polyton",
    "el",
    "en-AU",
    "en-CA",
    "en-GB",
    "en-NZ",
    "en-ZA",
    "en",
    "eo",
    "es-AR",
    "es-BO",
    "es-CL",
    "es-CO",
    "es-CR",
    "es-CU",
    "es-DO",
    "es-EC",
    "es-GT",
    "es-HN",
    "es-MX",
    "es-NI",
    "es-PA",
    "es-PE",
    "es-PH",
    "es-PR",
    "es-PY",
    "es-SV",
    "es-US",
    "es-UY",
    "es-VE",
    "es",
    "et",
    "eu",
    "fa",
    "fo",
    "fr",
    "fur",
    "fy",
    "ga",
    "gd",
    "gl",
    "gu",
    "gug",
    "he",
    "hi",
    "hr",
    "hu",
    "hy",
    "hyw",
    "ia",
    "id",
    "is",
    "it",
    "ka",
    "kk",
    "kmr",
    "ko",
    "la",
    "lb",
    "lo",
    "lt",
    "ltg",
    "lv",
    "mk",
    "mn",
    "mt",
    "nb",
    "nds",
    "ne",
    "nl",
    "nn",
    "oc",
    "pl",
    "pt-BR",
    "pt",
    "qu",
    "ro",
    "ru-yo",
    "ru",
    "rw",
    "si",
    "sk",
    "sl",
    "sq",
    "sr-Latn",
    "sr",
    "sv-FI",
    "sv",
    "sw",
    "te",
    "th",
    "tk",
    "tlh-Latn",
    "tlh",
    "tr",
    "uk",
    "vi"
};

/**
 * @brief Индекс для сохранения в модели информации о коде языка
 */
const int kSpellCheckerLanguageCodeRole = Qt::UserRole + 1;

/**
 * @brief Построить модель для всех доступных справочников проверки орфографии
 */
QStandardItemModel* buildSpellCheckerLanguagesModel(QObject* _parent) {
    auto model = new QStandardItemModel(_parent);
    for (const auto& language : kSpellCheckerLanguagesNameToCode) {
        auto item = new QStandardItem(language);
        item->setData(language, kSpellCheckerLanguageCodeRole);
        model->appendRow(item);
    }
    return model;
}

}

class SettingsView::Implementation
{
public:
    explicit Implementation(QWidget* _parent);

    /**
     * @brief Настроить карточку параметров приложения
     */
    void initApplicationCard();

    /**
     * @brief Настроить карточку параметров компонентов
     */
    void initScreenplayCard();

    /**
     * @brief Настроить карточку горячих клавиш
     */
    void initShortcutsCard();

    /**
     * @brief Проскролить представление до заданного виджета
     */
    void scrollToWidget(QWidget* _widget);


    QScrollArea* content = nullptr;
    QVariantAnimation scrollAnimation;

    //
    // Application
    //
    Card* applicationCard = nullptr;
    QGridLayout* applicationCardLayout = nullptr;
    H5Label* applicationTitle = nullptr;
    //
    // ... Common
    //
    Body1Label* language = nullptr;
    Button* changeLanuage = nullptr;
    CheckBox* useTypewriterSound = nullptr;
    CheckBox* useSpellChecker = nullptr;
    ComboBox* spellCheckerLanguage = nullptr;
    QStandardItemModel* spellCheckerLanguagesModel = nullptr;
    //
    // ... User interface
    //
    H6Label* applicationUserInterfaceTitle = nullptr;
    Body1Label* theme = nullptr;
    Button* changeTheme = nullptr;
    Body1Label* scaleFactorTitle = nullptr;
    Slider* scaleFactor = nullptr;
    Body2Label* scaleFactorSmallInfo = nullptr;
    Body2Label* scaleFactorBigInfo = nullptr;
    //
    // ... Save changes & backups
    //
    H6Label* applicationSaveAndBackupTitle = nullptr;
    CheckBox* autoSave = nullptr;
    CheckBox* saveBackups = nullptr;
    TextField* backupsFolderPath = nullptr;
    //
    int applicationCardBottomSpacerIndex = 0;

    H4Label* componentsTitle = nullptr;

    //
    // Comonents
    //
    Card* screenplayCard = nullptr;
    QGridLayout* screenplayCardLayout = nullptr;
    H5Label* screenplayTitle = nullptr;
    //
    // ... Screenplay editor
    //
    H6Label* screenplayEditorTitle = nullptr;
    CheckBox* screenplayEditorShowSceneNumber = nullptr;
    CheckBox* screenplayEditorShowSceneNumberAtLeft = nullptr;
    CheckBox* screenplayEditorShowSceneNumberAtRight = nullptr;
    CheckBox* screenplayEditorShowDialogueNumber = nullptr;
    CheckBox* screenplayEditorHighlightCurrentLine = nullptr;
    //
    // ... Screenplay navigator
    //
    //
    int screenplayCardBottomSpacerIndex = 0;

    Card* shortcutsCard = nullptr;
    QGridLayout* shortcutsCardLayout = nullptr;
    //
    H5Label* shortcutsTitle = nullptr;
    //
    int shortcutsCardBottomSpacerIndex = 0;
};

SettingsView::Implementation::Implementation(QWidget* _parent)
    : content(new QScrollArea(_parent)),
      //
      applicationCard(new Card(content)),
      applicationCardLayout(new QGridLayout),
      applicationTitle(new H5Label(applicationCard)),
      language(new Body1Label(applicationCard)),
      changeLanuage(new Button(applicationCard)),
      useTypewriterSound(new CheckBox(applicationCard)),
      useSpellChecker(new CheckBox(applicationCard)),
      spellCheckerLanguage(new ComboBox(applicationCard)),
      spellCheckerLanguagesModel(buildSpellCheckerLanguagesModel(spellCheckerLanguage)),
      applicationUserInterfaceTitle(new H6Label(applicationCard)),
      theme(new Body1Label(applicationCard)),
      changeTheme(new Button(applicationCard)),
      scaleFactorTitle(new Body1Label(applicationCard)),
      scaleFactor(new Slider(applicationCard)),
      scaleFactorSmallInfo(new Body2Label(applicationCard)),
      scaleFactorBigInfo(new Body2Label(applicationCard)),
      applicationSaveAndBackupTitle(new H6Label(applicationCard)),
      autoSave(new CheckBox(applicationCard)),
      saveBackups(new CheckBox(applicationCard)),
      backupsFolderPath(new TextField(applicationCard)),
      //
      componentsTitle(new H4Label(content)),
      //
      screenplayCard(new Card(content)),
      screenplayCardLayout(new QGridLayout),
      screenplayTitle(new H5Label(screenplayCard)),
      screenplayEditorTitle(new H6Label(screenplayCard)),
      screenplayEditorShowSceneNumber(new CheckBox(screenplayCard)),
      screenplayEditorShowSceneNumberAtLeft(new CheckBox(screenplayCard)),
      screenplayEditorShowSceneNumberAtRight(new CheckBox(screenplayCard)),
      screenplayEditorShowDialogueNumber(new CheckBox(screenplayCard)),
      screenplayEditorHighlightCurrentLine(new CheckBox(screenplayCard)),
      //
      shortcutsCard(new Card(content)),
      shortcutsCardLayout(new QGridLayout),
      shortcutsTitle(new H5Label(shortcutsCard))
{
    QPalette palette;
    palette.setColor(QPalette::Base, Qt::transparent);
    palette.setColor(QPalette::Window, Qt::transparent);
    content->setPalette(palette);
    content->setFrameShape(QFrame::NoFrame);
    content->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    content->setVerticalScrollBar(new ScrollBar);
    scrollAnimation.setEasingCurve(QEasingCurve::OutQuad);
    scrollAnimation.setDuration(180);

    initApplicationCard();
    initScreenplayCard();
    initShortcutsCard();

    QWidget* contentWidget = new QWidget;
    content->setWidget(contentWidget);
    content->setWidgetResizable(true);
    QVBoxLayout* layout = new QVBoxLayout(contentWidget);
    layout->setContentsMargins({});
    layout->setSpacing(0);
    layout->addWidget(applicationCard);
    layout->addWidget(componentsTitle);
    layout->addWidget(screenplayCard);
    layout->addWidget(shortcutsCard);
    layout->addStretch();
}

void SettingsView::Implementation::initApplicationCard()
{
    spellCheckerLanguage->setEnabled(false);
    spellCheckerLanguage->setModel(spellCheckerLanguagesModel);
    scaleFactor->setMaximumValue(4000);
    scaleFactor->setValue(1000);
    backupsFolderPath->setEnabled(false);
    backupsFolderPath->setTrailingIcon(u8"\U000f0256");

    //
    // Компоновка
    //
    applicationCardLayout->setContentsMargins({});
    applicationCardLayout->setSpacing(0);
    int itemIndex = 0;
    applicationCardLayout->addWidget(applicationTitle, itemIndex++, 0);
    {
        auto layout = makeLayout();
        layout->addWidget(language, 0, Qt::AlignCenter);
        layout->addWidget(changeLanuage);
        layout->addStretch();
        applicationCardLayout->addLayout(layout, itemIndex++, 0);
    }
    applicationCardLayout->addWidget(useTypewriterSound, itemIndex++, 0);
    {
        spellCheckerLanguage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        auto layout = makeLayout();
        layout->addWidget(useSpellChecker, 0, Qt::AlignCenter);
        layout->addWidget(spellCheckerLanguage);
        applicationCardLayout->addLayout(layout, itemIndex++, 0);
    }
    //
    // ... интерфейс
    //
    applicationCardLayout->addWidget(applicationUserInterfaceTitle, itemIndex++, 0);
    {
        auto layout = makeLayout();
        layout->addWidget(theme, 0, Qt::AlignCenter);
        layout->addWidget(changeTheme);
        layout->addStretch();
        applicationCardLayout->addLayout(layout, itemIndex++, 0);
    }
    applicationCardLayout->addWidget(scaleFactorTitle, itemIndex++, 0);
    applicationCardLayout->addWidget(scaleFactor, itemIndex++, 0);
    {
        auto layout = makeLayout();
        layout->addWidget(scaleFactorSmallInfo);
        layout->addStretch();
        layout->addWidget(scaleFactorBigInfo);
        applicationCardLayout->addLayout(layout, itemIndex++, 0);
    }
    //
    // ... сохранение и бэкапы
    //
    applicationCardLayout->addWidget(applicationSaveAndBackupTitle, itemIndex++, 0);
    applicationCardLayout->addWidget(autoSave, itemIndex++, 0);
    {
        QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        policy.setHeightForWidth(true);
        backupsFolderPath->setSizePolicy(policy);

        auto layout = makeLayout();
        layout->addWidget(saveBackups, 0, Qt::AlignCenter);
        layout->addWidget(backupsFolderPath);
        applicationCardLayout->addLayout(layout, itemIndex++, 0);
    }
    applicationCardBottomSpacerIndex = itemIndex;
    applicationCard->setLayoutReimpl(applicationCardLayout);
}

void SettingsView::Implementation::initScreenplayCard()
{
    screenplayEditorShowSceneNumberAtLeft->setEnabled(false);
    screenplayEditorShowSceneNumberAtLeft->setChecked(true);
    screenplayEditorShowSceneNumberAtRight->setEnabled(false);

    //
    // Компоновка
    //
    screenplayCardLayout->setContentsMargins({});
    screenplayCardLayout->setSpacing(0);
    int itemIndex = 0;
    screenplayCardLayout->addWidget(screenplayTitle, itemIndex++, 0);
    //
    // ... редактор сценария
    //
    screenplayCardLayout->addWidget(screenplayEditorTitle, itemIndex++, 0);
    {
        auto layout = makeLayout();
        layout->addWidget(screenplayEditorShowSceneNumber);
        layout->addWidget(screenplayEditorShowSceneNumberAtLeft);
        layout->addWidget(screenplayEditorShowSceneNumberAtRight);
        layout->addStretch();
        screenplayCardLayout->addLayout(layout, itemIndex++, 0);
    }
    screenplayCardLayout->addWidget(screenplayEditorShowDialogueNumber, itemIndex++, 0);
    screenplayCardLayout->addWidget(screenplayEditorHighlightCurrentLine, itemIndex++, 0);
    //
    screenplayCardBottomSpacerIndex = itemIndex;
    screenplayCard->setLayoutReimpl(screenplayCardLayout);
}

void SettingsView::Implementation::initShortcutsCard()
{
    shortcutsCardLayout->setContentsMargins({});
    shortcutsCardLayout->setSpacing(0);
    int itemIndex = 0;
    shortcutsCardLayout->addWidget(shortcutsTitle, itemIndex++, 0);
    shortcutsCardBottomSpacerIndex = itemIndex;
    shortcutsCard->setLayoutReimpl(shortcutsCardLayout);
}

void SettingsView::Implementation::scrollToWidget(QWidget* childWidget)
{
    const QRect microFocus = childWidget->inputMethodQuery(Qt::ImCursorRectangle).toRect();
    const QRect defaultMicroFocus =
        childWidget->QWidget::inputMethodQuery(Qt::ImCursorRectangle).toRect();
    QRect focusRect = (microFocus != defaultMicroFocus)
        ? QRect(childWidget->mapTo(content->widget(), microFocus.topLeft()), microFocus.size())
        : QRect(childWidget->mapTo(content->widget(), QPoint(0,0)), childWidget->size());
    const QRect visibleRect(-content->widget()->pos(), content->viewport()->size());

    focusRect.adjust(-50, -50, 50, 50);

    scrollAnimation.setStartValue(content->verticalScrollBar()->value());
    scrollAnimation.setEndValue(focusRect.top());
    scrollAnimation.start();
}


// ****


SettingsView::SettingsView(QWidget* _parent)
    : Widget(_parent),
      d(new Implementation(this))
{
    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins({});
    layout->setSpacing(0);
    layout->addWidget(d->content);
    setLayout(layout);

    connect(&d->scrollAnimation, &QVariantAnimation::valueChanged, this, [this] (const QVariant& _value) {
        d->content->verticalScrollBar()->setValue(_value.toInt());
    });
    //
    // Приложение
    //
    connect(d->useSpellChecker, &CheckBox::checkedChanged, d->spellCheckerLanguage, &ComboBox::setEnabled);
    connect(d->saveBackups, &CheckBox::checkedChanged, d->backupsFolderPath, &ComboBox::setEnabled);
    connect(d->backupsFolderPath, &TextField::trailingIconPressed, this, [this] {
        const auto path =
                QFileDialog::getExistingDirectory(
                    this, tr("Choose the folder where backups will be saved"), d->backupsFolderPath->text());
        if (!path.isEmpty()) {
            d->backupsFolderPath->setText(path);
        }
    });
    //
    connect(d->changeLanuage, &Button::clicked, this, &SettingsView::applicationLanguagePressed);
    connect(d->useTypewriterSound, &CheckBox::checkedChanged, this, &SettingsView::applicationUseTypewriterSoundChanged);
    connect(d->useSpellChecker, &CheckBox::checkedChanged, this, [this] (bool _checked) {
        emit applicationUseSpellCheckerChanged(_checked);
        if (_checked) {
            emit applicationSpellCheckerLanguageChanged(
                d->spellCheckerLanguage->currentIndex().data(kSpellCheckerLanguageCodeRole).toString());
        }
    });
    connect(d->spellCheckerLanguage, &ComboBox::currentIndexChanged, this, [this] (const QModelIndex& _index) {
        emit applicationSpellCheckerLanguageChanged(_index.data(kSpellCheckerLanguageCodeRole).toString());
    });
    connect(d->changeTheme, &Button::clicked, this, &SettingsView::applicationThemePressed);
    connect(d->scaleFactor, &Slider::valueChanged, this, [this] (int _value) {
        emit applicationScaleFactorChanged(static_cast<qreal>(std::max(1, _value)) / 1000.0);
    });
    connect(d->autoSave, &CheckBox::checkedChanged, this, &SettingsView::applicationUseAutoSaveChanged);
    connect(d->saveBackups, &CheckBox::checkedChanged, this, &SettingsView::applicationSaveBackupsChanged);
    connect(d->backupsFolderPath, &TextField::textChanged, this, [this] {
        emit applicationBackupsFolderChanged(d->backupsFolderPath->text());
    });

    //
    // Компоненты
    //
    // ... Редактор сценария
    //
    connect(d->screenplayEditorShowSceneNumber, &CheckBox::checkedChanged, d->screenplayEditorShowSceneNumberAtLeft, &CheckBox::setEnabled);
    connect(d->screenplayEditorShowSceneNumber, &CheckBox::checkedChanged, d->screenplayEditorShowSceneNumberAtRight, &CheckBox::setEnabled);
    auto screenplayEditorCorrectShownSceneNumber = [this] {
        if (!d->screenplayEditorShowSceneNumberAtLeft->isChecked()
            && !d->screenplayEditorShowSceneNumberAtRight->isChecked()) {
            d->screenplayEditorShowSceneNumberAtLeft->setChecked(true);
        }
    };
    connect(d->screenplayEditorShowSceneNumberAtLeft, &CheckBox::checkedChanged, this, screenplayEditorCorrectShownSceneNumber);
    connect(d->screenplayEditorShowSceneNumberAtRight, &CheckBox::checkedChanged, this, screenplayEditorCorrectShownSceneNumber);
    //
    auto notifyScreenplayEditorShowSceneNumbersChanged = [this] {
        emit screenplayEditorShowSceneNumberChanged(d->screenplayEditorShowSceneNumber->isChecked(),
                                                    d->screenplayEditorShowSceneNumberAtLeft->isChecked(),
                                                    d->screenplayEditorShowSceneNumberAtRight->isChecked());
    };
    connect(d->screenplayEditorShowSceneNumber, &CheckBox::checkedChanged, this, notifyScreenplayEditorShowSceneNumbersChanged);
    connect(d->screenplayEditorShowSceneNumberAtLeft, &CheckBox::checkedChanged, this, notifyScreenplayEditorShowSceneNumbersChanged);
    connect(d->screenplayEditorShowSceneNumberAtRight, &CheckBox::checkedChanged, this, notifyScreenplayEditorShowSceneNumbersChanged);
    connect(d->screenplayEditorShowDialogueNumber, &CheckBox::checkedChanged, this, &SettingsView::screenplayEditorShowDialogueNumberChanged);
    connect(d->screenplayEditorHighlightCurrentLine, &CheckBox::checkedChanged, this, &SettingsView::screenplayEditorHighlightCurrentLineChanged);

    designSystemChangeEvent(nullptr);
}

void SettingsView::showApplication()
{
    d->scrollToWidget(d->applicationTitle);
}

void SettingsView::showApplicationUserInterface()
{
    d->scrollToWidget(d->applicationUserInterfaceTitle);
}

void SettingsView::showApplicationSaveAndBackups()
{
    d->scrollToWidget(d->applicationSaveAndBackupTitle);
}

void SettingsView::showComponents()
{
    d->scrollToWidget(d->componentsTitle);
}

void SettingsView::showComponentsScreenplay()
{
    d->scrollToWidget(d->screenplayTitle);
}

void SettingsView::showShortcuts()
{
    d->scrollToWidget(d->shortcutsTitle);
}

void SettingsView::setApplicationLanguage(int _language)
{
    auto languageString = [_language] () -> QString {
        switch (_language) {
            case QLocale::English: {
                return "English";
            }
            case QLocale::Russian: {
                return "Русский";
            }
            default: {
                return QLocale::languageToString(static_cast<QLocale::Language>(_language));
            }
        }
    };
    d->changeLanuage->setText(languageString());
}

void SettingsView::setApplicationUseTypewriterSound(bool _use)
{
    d->useTypewriterSound->setChecked(_use);
}

void SettingsView::setApplicationUseSpellChecker(bool _use)
{
    d->useSpellChecker->setChecked(_use);
}

void SettingsView::setApplicationSpellCheckerLanguage(const QString& _languageCode)
{
    for (int row = 0; row < d->spellCheckerLanguagesModel->rowCount(); ++row) {
        auto item = d->spellCheckerLanguagesModel->item(row);
        if (item->data(kSpellCheckerLanguageCodeRole).toString() != _languageCode) {
            continue;
        }

        d->spellCheckerLanguage->setCurrentIndex(item->index());
        break;
    }
}

void SettingsView::setApplicationTheme(int _theme)
{
    auto themeString = [_theme] {
        switch (static_cast<Ui::ApplicationTheme>(_theme)) {
            case Ui::ApplicationTheme::Dark: {
                return tr("Dark", "Theme, will be used in case \"Theme: Dark\"");
            }
            case Ui::ApplicationTheme::Light: {
                return tr("Light", "Theme, will be used in case \"Theme: Light\"");
            }
            case Ui::ApplicationTheme::DarkAndLight: {
                return tr("Dark and light", "Theme, will be used in case \"Theme: Dark and light\"");
            }
            default: {
                return tr("Custom", "Theme, will be used in case \"Theme: Custom\"");
            }
        }
    };
    d->changeTheme->setText(themeString());
}

void SettingsView::setApplicationScaleFactor(qreal _scaleFactor)
{
    d->scaleFactor->setValue(std::max(1, static_cast<int>(_scaleFactor * 1000)));
}

void SettingsView::setApplicationUseAutoSave(bool _use)
{
    d->autoSave->setChecked(_use);
}

void SettingsView::setApplicationSaveBackups(bool _save)
{
    d->saveBackups->setChecked(_save);
}

void SettingsView::setApplicationBackupsFolder(const QString& _path)
{
    d->backupsFolderPath->setText(_path);
}

void SettingsView::setScreenplayEditorShowSceneNumber(bool _show, bool _atLeft, bool _atRight)
{
    QSignalBlocker blocker(d->screenplayEditorShowSceneNumberAtLeft);

    d->screenplayEditorShowSceneNumber->setChecked(_show);
    d->screenplayEditorShowSceneNumberAtLeft->setChecked(_atLeft);
    d->screenplayEditorShowSceneNumberAtRight->setChecked(_atRight);
}

void SettingsView::setScreenplayEditorShowDialogueNumber(bool _show)
{
    d->screenplayEditorShowDialogueNumber->setChecked(_show);
}

void SettingsView::setScreenplayEditorHighlightCurrentLine(bool _highlight)
{
    d->screenplayEditorHighlightCurrentLine->setChecked(_highlight);
}

void SettingsView::updateTranslations()
{
    d->applicationTitle->setText(tr("Application settings"));
    d->language->setText(tr("Language"));
    d->useTypewriterSound->setText(tr("Use typewriter sound for keys pressing"));
    d->useSpellChecker->setText(tr("Spell check"));
    d->spellCheckerLanguage->setLabel(tr("Spelling dictionary"));
    {
        int index = 0;
        for (const auto& language : { tr("Afrikaans"),
                                      tr("Aragonese"),
                                      tr("Arabic"),
                                      tr("Azerbaijani"),
                                      tr("Belarusian"),
                                      tr("Bulgarian"),
                                      tr("Bengali"),
                                      tr("Tibetan"),
                                      tr("Breton"),
                                      tr("Bosnian"),
                                      tr("Catalan (Valencian)"),
                                      tr("Catalan"),
                                      tr("Czech"),
                                      tr("Welsh"),
                                      tr("Danish"),
                                      tr("German (Austria)"),
                                      tr("German (Switzerland)"),
                                      tr("German"),
                                      tr("Greek (Polytonic)"),
                                      tr("Greek"),
                                      tr("English (Australia)"),
                                      tr("English (Canada)"),
                                      tr("English (United Kingdom)"),
                                      tr("English (New Zealand)"),
                                      tr("English (South Africa)"),
                                      tr("English (United States)"),
                                      tr("Esperanto"),
                                      tr("Spanish (Argentina)"),
                                      tr("Spanish (Bolivia)"),
                                      tr("Spanish (Chile)"),
                                      tr("Spanish (Colombia)"),
                                      tr("Spanish (Costa Rica)"),
                                      tr("Spanish (Cuba)"),
                                      tr("Spanish (Dominican Republic)"),
                                      tr("Spanish (Ecuador)"),
                                      tr("Spanish (Guatemala)"),
                                      tr("Spanish (Honduras)"),
                                      tr("Spanish (Mexico)"),
                                      tr("Spanish (Nicaragua)"),
                                      tr("Spanish (Panama)"),
                                      tr("Spanish (Peru)"),
                                      tr("Spanish (Philippines)"),
                                      tr("Spanish (Puerto Rico)"),
                                      tr("Spanish (Paraguay)"),
                                      tr("Spanish (El Salvador)"),
                                      tr("Spanish (United States)"),
                                      tr("Spanish (Uruguay)"),
                                      tr("Spanish (Venezuela)"),
                                      tr("Spanish"),
                                      tr("Estonian"),
                                      tr("Basque"),
                                      tr("Persian"),
                                      tr("Faroese"),
                                      tr("French"),
                                      tr("Friulian"),
                                      tr("Western Frisian"),
                                      tr("Irish"),
                                      tr("Gaelic"),
                                      tr("Galician"),
                                      tr("Gujarati"),
                                      tr("Guarani"),
                                      tr("Hebrew"),
                                      tr("Hindi"),
                                      tr("Croatian"),
                                      tr("Hungarian"),
                                      tr("Armenian"),
                                      tr("Armenian (Western)"),
                                      tr("Interlingua"),
                                      tr("Indonesian"),
                                      tr("Icelandic"),
                                      tr("Italian"),
                                      tr("Georgian"),
                                      tr("Kazakh"),
                                      tr("Kurdish"),
                                      tr("Korean"),
                                      tr("Latin"),
                                      tr("Luxembourgish"),
                                      tr("Lao"),
                                      tr("Lithuanian"),
                                      tr("Latgalian"),
                                      tr("Latvian"),
                                      tr("Macedonian"),
                                      tr("Mongolian"),
                                      tr("Maltese"),
                                      tr("Norwegian"),
                                      tr("Low German"),
                                      tr("Nepali"),
                                      tr("Dutch"),
                                      tr("Norwegian"),
                                      tr("Occitan"),
                                      tr("Polish"),
                                      tr("Portuguese (Brazilian)"),
                                      tr("Portuguese"),
                                      tr("Quechua"),
                                      tr("Romanian"),
                                      tr("Russian (with Yo)"),
                                      tr("Russian"),
                                      tr("Kinyarwanda"),
                                      tr("Sinhala"),
                                      tr("Slovak"),
                                      tr("Slovenian"),
                                      tr("Albanian"),
                                      tr("Serbian (Latin)"),
                                      tr("Serbian"),
                                      tr("Swedish (Finland)"),
                                      tr("Swedish"),
                                      tr("Swahili"),
                                      tr("Telugu"),
                                      tr("Thai"),
                                      tr("Turkmen"),
                                      tr("Klingon (Latin)"),
                                      tr("Klingon"),
                                      tr("Turkish"),
                                      tr("Ukrainian"),
                                      tr("Vietnamese") }) {
            d->spellCheckerLanguagesModel->item(index++)->setText(language);
        }
        d->spellCheckerLanguagesModel->sort(0);
    }
    d->applicationUserInterfaceTitle->setText(tr("User interface"));
    d->theme->setText(tr("Theme"));
    d->scaleFactorTitle->setText(tr("Size of the user interface elements:"));
    d->scaleFactorSmallInfo->setText(tr("small"));
    d->scaleFactorBigInfo->setText(tr("big"));
    d->applicationSaveAndBackupTitle->setText(tr("Save changes & backups"));
    d->autoSave->setText(tr("Automatically save changes as soon as possible"));
    d->autoSave->setToolTip(tr("Autosave works very accurately.\n"
                               "It saves the project every 3 seconds if you do not use your mouse or keyboard.\n"
                               "If you work with no interruptions it saves the project every 3 minutes."));
    d->saveBackups->setText(tr("Save backups"));
    d->backupsFolderPath->setLabel(tr("Backups folder path"));

    d->componentsTitle->setText(tr("Components"));
    //
    d->screenplayTitle->setText(tr("Screenplay"));
    d->screenplayEditorTitle->setText(tr("Text editor"));
    d->screenplayEditorShowSceneNumber->setText(tr("Show scene number"));
    d->screenplayEditorShowSceneNumberAtLeft->setText(tr("on the left"));
    d->screenplayEditorShowSceneNumberAtRight->setText(tr("on the right"));
    d->screenplayEditorShowDialogueNumber->setText(tr("Show dialogue number"));
    d->screenplayEditorHighlightCurrentLine->setText(tr("Highlight current line"));

    d->shortcutsTitle->setText(tr("Shortcuts"));
}

SettingsView::~SettingsView() = default;

void SettingsView::designSystemChangeEvent(DesignSystemChangeEvent* _event)
{
    Widget::designSystemChangeEvent(_event);

    setBackgroundColor(DesignSystem::color().surface());
    d->content->widget()->layout()->setContentsMargins(QMarginsF(Ui::DesignSystem::layout().px24(), Ui::DesignSystem::layout().topContentMargin(),
                                                                 Ui::DesignSystem::layout().px24(), Ui::DesignSystem::layout().px24()).toMargins());

    for (auto card : { d->applicationCard, d->screenplayCard, d->shortcutsCard }) {
        card->setBackgroundColor(DesignSystem::color().background());
    }

    auto titleColor = DesignSystem::color().onBackground();
    titleColor.setAlphaF(DesignSystem::inactiveTextOpacity());
    for (auto cardTitle : QVector<Widget*>{ d->applicationTitle,
                                        d->applicationUserInterfaceTitle,
                                        d->applicationSaveAndBackupTitle,
                                        d->screenplayTitle,
                                        d->screenplayEditorTitle,
                                        d->shortcutsTitle }) {
        cardTitle->setBackgroundColor(DesignSystem::color().background());
        cardTitle->setTextColor(titleColor);
        cardTitle->setContentsMargins(Ui::DesignSystem::label().margins().toMargins());
    }
    for (auto title : QVector<Widget*>{ d->componentsTitle } ) {
        title->setBackgroundColor(DesignSystem::color().surface());
        title->setTextColor(DesignSystem::color().onSurface());
        title->setContentsMargins(Ui::DesignSystem::label().margins().toMargins());
    }

    auto labelMargins = Ui::DesignSystem::label().margins().toMargins();
    labelMargins.setTop(static_cast<int>(Ui::DesignSystem::button().shadowMargins().top()));
    labelMargins.setBottom(static_cast<int>(Ui::DesignSystem::button().shadowMargins().bottom()));
    for (auto label : QVector<Widget*>{ d->language,
                                        d->theme,
                                        d->scaleFactorTitle, d->scaleFactorSmallInfo, d->scaleFactorBigInfo }) {
        label->setBackgroundColor(DesignSystem::color().background());
        label->setTextColor(DesignSystem::color().onBackground());
        label->setContentsMargins(labelMargins);
    }

    for (auto checkBox : {
         d->useTypewriterSound,
         d->useSpellChecker,
         //
         d->autoSave,
         d->saveBackups,
         //
         d->screenplayEditorShowSceneNumber,
         d->screenplayEditorShowSceneNumberAtLeft,
         d->screenplayEditorShowSceneNumberAtRight,
         d->screenplayEditorShowDialogueNumber,
         d->screenplayEditorHighlightCurrentLine }) {
        checkBox->setBackgroundColor(DesignSystem::color().background());
        checkBox->setTextColor(DesignSystem::color().onBackground());
    }

    for (auto textField : QVector<TextField*>{ d->spellCheckerLanguage,
                                               d->backupsFolderPath }) {
        textField->setBackgroundColor(DesignSystem::color().background());
        textField->setTextColor(DesignSystem::color().onBackground());
    }

    for (auto button : { d->changeLanuage,
                         d->changeTheme }) {
        button->setBackgroundColor(DesignSystem::color().secondary());
        button->setTextColor(DesignSystem::color().secondary());
    }

    d->scaleFactor->setBackgroundColor(DesignSystem::color().background());
    d->scaleFactor->setContentsMargins({static_cast<int>(Ui::DesignSystem::layout().px24()), 0,
                                           static_cast<int>(Ui::DesignSystem::layout().px24()), 0});

    d->applicationCardLayout->setRowMinimumHeight(d->applicationCardBottomSpacerIndex,
                                                  static_cast<int>(Ui::DesignSystem::layout().px24()));
    d->screenplayCardLayout->setRowMinimumHeight(d->screenplayCardBottomSpacerIndex,
                                                  static_cast<int>(Ui::DesignSystem::layout().px24()));
    d->shortcutsCardLayout->setRowMinimumHeight(d->shortcutsCardBottomSpacerIndex,
                                                  static_cast<int>(Ui::DesignSystem::layout().px24()));
}

} // namespace Ui
