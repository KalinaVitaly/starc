#pragma once

#include <ui/widgets/widget/widget.h>

namespace Ui
{

/**
 * @brief Виджет добавления комментария
 */
class ScreenplayTextAddCommentView : public Widget
{
    Q_OBJECT

public:
    explicit ScreenplayTextAddCommentView(QWidget* _parent = nullptr);
    ~ScreenplayTextAddCommentView() override;

    /**
     * @brief Текст комментария
     */
    QString comment() const;
    void setComment(const QString& _comment);

signals:
    /**
     * @brief Пользователь нажал кнопку сохранить
     */
    void savePressed();

    /**
     * @brief Пользователь нажал кнопку отмена
     */
    void cancelPressed();

protected:
    void updateTranslations() override;
    void designSystemChangeEvent(DesignSystemChangeEvent* _event) override;

private:
    class Implementation;
    QScopedPointer<Implementation> d;
};

} // namespace Ui
