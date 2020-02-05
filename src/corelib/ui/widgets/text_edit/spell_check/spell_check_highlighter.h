#pragma once

#include "syntax_highlighter.h"

class SpellChecker;


/**
 * @brief Класс подсвечивающий слова не прошедшие проверку правописания
 */
class SpellCheckHighlighter : public SyntaxHighlighter
{
    Q_OBJECT

public:
    explicit SpellCheckHighlighter(QTextDocument* _parent, const SpellChecker& _checker);
    ~SpellCheckHighlighter() override;

    /**
     * @brief Включить/выключить проверку орфографии
     */
    void setUseSpellChecker(bool _use);

    /**
     * @brief Включена ли проверка орфографии
     */
    bool useSpellChecker() const;

    /**
     * @brief Задать позицию курсора
     */
    void setCursorPosition(int position);

    /**
     * @brief Подсветить текст не прошедший проверку орфографии
     */
    void highlightBlock(const QString& _text);

private:
    class Implementation;
    QScopedPointer<Implementation> d;
};