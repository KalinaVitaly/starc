#include "screenplay_text_view.h"

#include "screenplay_text_edit.h"
#include "screenplay_text_edit_toolbar.h"

#include <ui/design_system/design_system.h>
#include <ui/widgets/floating_tool_bar/floating_tool_bar.h>
#include <ui/widgets/scroll_bar/scroll_bar.h>
#include <ui/widgets/text_edit/completer/completer.h>
#include <ui/widgets/text_edit/page/page_metrics.h>
#include <ui/widgets/text_edit/spell_check/spell_checker.h>
#include <ui/widgets/text_edit/scalable_wrapper/scalable_wrapper.h>

#include <QAction>
#include <QScrollArea>
#include <QStringListModel>
#include <QVBoxLayout>


namespace Ui
{

class ScreenplayTextView::Implementation
{
public:
    explicit Implementation(QWidget* _parent);

    /**
     * @brief Обновить настройки UI панели инструментов
     */
    void updateToolBarsUi();


    ScreenplayTextEditToolBar* toolBar = nullptr;

    ScreenplayTextEdit* screenplayText = nullptr;
    ScalableWrapper* scalableWrapper = nullptr;
};

ScreenplayTextView::Implementation::Implementation(QWidget* _parent)
    : toolBar(new ScreenplayTextEditToolBar(_parent)),
      screenplayText(new ScreenplayTextEdit(_parent)),
      scalableWrapper(new ScalableWrapper(screenplayText, _parent))
{
    screenplayText->setVerticalScrollBar(new ScrollBar);
    screenplayText->setHorizontalScrollBar(new ScrollBar);
    scalableWrapper->setVerticalScrollBar(new ScrollBar);
    scalableWrapper->setHorizontalScrollBar(new ScrollBar);
    scalableWrapper->initScrollBarsSyncing();

    screenplayText->setFrameShape(QFrame::NoFrame);
    screenplayText->setUsePageMode(true);
    screenplayText->setPageFormat(QPageSize::A4);
    screenplayText->setPageMargins({38, 20, 17, 20});
    QFont font("Courier Prime");
    auto size = PageMetrics::ptToPx(12);
    font.setPixelSize(size);
    screenplayText->document()->setDefaultFont(font);
    screenplayText->setPageNumbersAlignment(Qt::AlignBottom | Qt::AlignRight);
    screenplayText->setHeader("Header text");
    screenplayText->setFooter("Footer text");
    screenplayText->setCursorWidth(4);
    screenplayText->setUseSpellChecker(true);
    screenplayText->setSpellCheckLanguage(SpellCheckerLanguage::EnglishUS);

    connect(screenplayText, &CompleterTextEdit::textChanged, [this] {
        if (screenplayText->toPlainText().isEmpty()) {
            screenplayText->closeCompleter();
            return;
        }

        static QStringListModel* model = new QStringListModel({"АНТОН", "АДМИРАЛ", "АДМИНИСТРАТОР", "АПОСТОЛ", "АДЛЕН", "АМИР"}, screenplayText);
        screenplayText->complete(model, screenplayText->toPlainText());
    });
}

void ScreenplayTextView::Implementation::updateToolBarsUi()
{
    toolBar->resize(toolBar->sizeHint());
    toolBar->move(QPointF(Ui::DesignSystem::layout().px24(),
                          Ui::DesignSystem::layout().px24()).toPoint());
    toolBar->setBackgroundColor(Ui::DesignSystem::color().primary());
    toolBar->setTextColor(Ui::DesignSystem::color().onPrimary());
    toolBar->raise();
}


// ****


ScreenplayTextView::ScreenplayTextView(QWidget* _parent)
    : Widget(_parent),
      d(new Implementation(this))
{


    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins({});
    layout->setSpacing(0);
    layout->addWidget(d->scalableWrapper);
    setLayout(layout);

    connect(d->screenplayText, &PageTextEdit::textChanged, this, [this] {
        emit textChanged(d->screenplayText->toHtml());
    });

    updateTranslations();
    designSystemChangeEvent(nullptr);
}

void ScreenplayTextView::setText(const QString& _text)
{

}

void ScreenplayTextView::resizeEvent(QResizeEvent* _event)
{
    Widget::resizeEvent(_event);

    d->toolBar->move(QPointF(Ui::DesignSystem::layout().px24(),
                             Ui::DesignSystem::layout().px24()).toPoint());
}

ScreenplayTextView::~ScreenplayTextView() = default;

void ScreenplayTextView::updateTranslations()
{

}

void ScreenplayTextView::designSystemChangeEvent(DesignSystemChangeEvent* _event)
{
    Widget::designSystemChangeEvent(_event);

    setBackgroundColor(Ui::DesignSystem::color().surface());

    d->updateToolBarsUi();

    d->screenplayText->setPageSpacing(Ui::DesignSystem::layout().px24());
    QPalette palette;
    palette.setColor(QPalette::Base, Ui::DesignSystem::color().background());
    palette.setColor(QPalette::Window, Ui::DesignSystem::color().surface());
    palette.setColor(QPalette::Text, Ui::DesignSystem::color().onBackground());
    palette.setColor(QPalette::Highlight, Ui::DesignSystem::color().secondary());
    palette.setColor(QPalette::HighlightedText, Ui::DesignSystem::color().onSecondary());
    d->scalableWrapper->setPalette(palette);
    d->screenplayText->setPalette(palette);
    d->screenplayText->completer()->setTextColor(Ui::DesignSystem::color().onBackground());
    d->screenplayText->completer()->setBackgroundColor(Ui::DesignSystem::color().background());
}

} // namespace Ui
