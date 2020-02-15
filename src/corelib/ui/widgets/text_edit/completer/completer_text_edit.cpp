#include "completer_text_edit.h"

#include "completer.h"

#include <QAbstractItemView>
#include <QScrollBar>


class CompleterTextEdit::Implementation
{
public:
    explicit Implementation(QWidget* _parent);

    /**
     * @brief Использовать ли подстановщик
     */
    bool useCompleter = true;

    /**
     * @brief Подстановщик для завершения текста
     */
    Completer* completer = nullptr;
};

CompleterTextEdit::Implementation::Implementation(QWidget* _parent)
    : completer(new Completer(_parent))
{
    completer->setWidget(_parent);
}


// ****


CompleterTextEdit::CompleterTextEdit(QWidget* _parent)
    : SpellCheckTextEdit(_parent),
      d(new Implementation(this))
{
    connect(d->completer, qOverload<const QString&>(&QCompleter::activated),
            this, qOverload<const QString&>(&CompleterTextEdit::applyCompletion));
}

CompleterTextEdit::~CompleterTextEdit() = default;

void CompleterTextEdit::setUseCompleter(bool _use)
{
    d->useCompleter = _use;
}

Completer* CompleterTextEdit::completer() const
{
    return d->completer;
}

bool CompleterTextEdit::isCompleterVisible() const
{
    return d->completer->popup()->isVisible();
}

bool CompleterTextEdit::complete(QAbstractItemModel* _model, const QString& _completionPrefix)
{
    if (!d->useCompleter) {
        return false;
    }

    if (_model == nullptr) {
        return false;
    }

    //
    // Настроим завершателя, если необходимо
    //
    const bool isNewModel = d->completer->model() != _model;
    const bool isOldModelChanged = !isNewModel
                                   && d->completer->model()->rowCount() != _model->rowCount();
    if (isNewModel || isOldModelChanged) {
        d->completer->setModel(_model);
        d->completer->setModelSorting(QCompleter::UnsortedModel);
        d->completer->setCaseSensitivity(Qt::CaseInsensitive);
    }
    d->completer->setCompletionPrefix(_completionPrefix);

    //
    // Если в модели для дополнения нет элементов, или она уже полностью дополнена
    //
    const bool hasCompletions = d->completer->completionModel()->rowCount() > 0;
    const bool alreadyComplete = _completionPrefix.toLower().endsWith(d->completer->currentCompletion().toLower());
    if (!hasCompletions || alreadyComplete) {
        //
        // ... скроем завершателя, если был показан
        //
        closeCompleter();
        return false;
    }

    //
    // Отобразим завершателя
    //
    d->completer->popup()->setCurrentIndex(d->completer->completionModel()->index(0, 0));
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor, _completionPrefix.length());
    QRect rect = cursorRect(cursor);
    rect.moveLeft(rect.left() + verticalScrollBar()->width() + viewportMargins().left());
    rect.moveTop(rect.top() + static_cast<int>(cursor.block().layout()->boundingRect().height()));
    rect.setWidth(d->completer->popup()->sizeHintForColumn(0)
                  + d->completer->popup()->verticalScrollBar()->sizeHint().width());
    d->completer->showCompleter(rect);
    emit popupShowed();
    return true;
}

void CompleterTextEdit::applyCompletion()
{
    if (!isCompleterVisible()) {
        return;
    }

    //
    // Получим выбранный из списка дополнений элемент
    //
    const QModelIndex currentIndex = d->completer->popup()->currentIndex();
    const QString completion = d->completer->popup()->model()->data(currentIndex).toString();
    applyCompletion(completion);

    closeCompleter();
}

void CompleterTextEdit::applyCompletion(const QString& _completion)
{
    //
    // Вставим дополнение в текст
    //
    const int completionStartFrom = d->completer->completionPrefix().length();
    const QString textToInsert = _completion.mid(completionStartFrom);

    //
    // Определяем позицию вставки текста дополнения и помещаем туда курсор редактора
    //
    while (!textCursor().atBlockEnd()) {
        QTextCursor cursor = textCursor();
        cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, completionStartFrom);
        if (cursor.selectedText().endsWith(d->completer->completionPrefix(), Qt::CaseInsensitive)) {
            break;
        }
        moveCursor(QTextCursor::NextCharacter);
    }

    //
    // Собственно дополняем текст
    //
    textCursor().insertText(textToInsert);
    emit completed();
}

void CompleterTextEdit::closeCompleter()
{
    d->completer->closeCompleter();
}
