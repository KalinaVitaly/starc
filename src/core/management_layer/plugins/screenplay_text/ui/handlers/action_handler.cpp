#include "action_handler.h"

#include <business_layer/model/screenplay/screenplay_dictionaries_model.h>
#include <business_layer/templates/screenplay_template.h>

#include <ui/screenplay_text_edit.h>

#include <utils/helpers/text_helper.h>

#include <QKeyEvent>
#include <QTextBlock>

using BusinessLayer::ScreenplayParagraphType;
using Ui::ScreenplayTextEdit;

namespace KeyProcessingLayer
{

ActionHandler::ActionHandler(ScreenplayTextEdit* _editor)
    : StandardKeyHandler(_editor)
{
}

void ActionHandler::handleEnter(QKeyEvent*)
{
	//
	// Получим необходимые значения
	//
	// ... курсор в текущем положении
	QTextCursor cursor = editor()->textCursor();
	// ... блок текста в котором находится курсор
	QTextBlock currentBlock = cursor.block();
	// ... текст до курсора
	QString cursorBackwardText = currentBlock.text().left(cursor.positionInBlock());
	// ... текст после курсора
	QString cursorForwardText = currentBlock.text().mid(cursor.positionInBlock());


	//
	// Обработка
	//
	if (editor()->isCompleterVisible()) {
		//! Если открыт подстановщик

		//
		// Ни чего не делаем
		//
	} else {
		//! Подстановщик закрыт

        if (cursor.hasSelection()) {
            //! Есть выделение

            //
            // Удаляем всё, но оставляем стилем блока текущий
            //
            editor()->addScenarioBlock(ScreenplayParagraphType::Action);
        } else {
            //! Нет выделения

            if (cursorBackwardText.isEmpty()
                && cursorForwardText.isEmpty()) {
                //! Текст пуст

                //
                // Меняем стиль на место и время
                //
                editor()->changeScenarioBlockType(changeForEnter(ScreenplayParagraphType::Action));
            } else {
                //! Текст не пуст

                //
                // Вставляем блок и применяем ему стиль описания действия
                //
                editor()->addScenarioBlock(jumpForEnter(ScreenplayParagraphType::Action));
            }
        }
	}
}

void ActionHandler::handleTab(QKeyEvent*)
{
	//
	// Получим необходимые значения
	//
	// ... курсор в текущем положении
	QTextCursor cursor = editor()->textCursor();
	// ... блок текста в котором находится курсор
	QTextBlock currentBlock = cursor.block();
	// ... текст до курсора
	QString cursorBackwardText = currentBlock.text().left(cursor.positionInBlock());
	// ... текст после курсора
	QString cursorForwardText = currentBlock.text().mid(cursor.positionInBlock());


	//
	// Обработка
	//
	if (editor()->isCompleterVisible()) {
		//! Если открыт подстановщик

		//
		// Ни чего не делаем
		//
	} else {
		//! Подстановщик закрыт

		if (cursor.hasSelection()) {
			//! Есть выделение

			//
			// Ни чего не делаем
			//
		} else {
			//! Нет выделения

			if (cursorBackwardText.isEmpty()
				&& cursorForwardText.isEmpty()) {
				//! Текст пуст

				//
				// Если строка пуста, то сменить стиль на имя героя
				//
                editor()->changeScenarioBlockType(changeForTab(ScreenplayParagraphType::Action));
			} else {
				//! Текст не пуст

				if (cursorBackwardText.isEmpty()) {
					//! В начале блока

					//
					// Меняем на блок персонажа
					//
                    editor()->changeScenarioBlockType(ScreenplayParagraphType::Character);
				} else if (cursorForwardText.isEmpty()) {
					//! В конце блока

					//
					// Вставляем блок персонажа
					//
                    editor()->addScenarioBlock(jumpForTab(ScreenplayParagraphType::Action));
				} else {
					//! Внутри блока

					//
					// Ни чего не делаем
					//
				}
			}
		}
	}
}

void ActionHandler::handleOther(QKeyEvent* _event)
{
	//
	// Получим необходимые значения
	//
	// ... курсор в текущем положении
	QTextCursor cursor = editor()->textCursor();
	// ... блок текста в котором находится курсор
	QTextBlock currentBlock = cursor.block();
	// ... текст до курсора
	QString cursorBackwardText = currentBlock.text().left(cursor.positionInBlock());


	//
	// Обработка
	//
	if (cursorBackwardText.endsWith(".")
		&& _event != 0
		&& _event->text() == ".") {
		//! Если нажата точка

		//
		// Если было введено какое-либо значение из списка мест (ИНТ./НАТ. и т.п.)
		// то необходимо преобразовать блок во время и место
		//
        const QString maybeSceneIntro = TextHelper::smartToUpper(cursorBackwardText);
        if (editor()->dictionaries()->sceneIntros().contains(maybeSceneIntro)) {
            editor()->changeScenarioBlockType(ScreenplayParagraphType::SceneHeading);
		}
	} else {
		//! В противном случае, обрабатываем в базовом классе

		StandardKeyHandler::handleOther(_event);
	}
}

} // namespace KeyProcessingLayer
