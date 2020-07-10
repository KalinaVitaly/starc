#pragma once

#include <ui/widgets/floating_tool_bar/floating_tool_bar.h>


namespace Ui
{

/**
 * @brief Панель инструментов рецензирования
 */
class ScreenplayTextCommentsToolbar : public FloatingToolBar
{
    Q_OBJECT

public:
    explicit ScreenplayTextCommentsToolbar(QWidget* _parent = nullptr);
    ~ScreenplayTextCommentsToolbar() override;

    /**
     * @brief Отобразить тулбар
     */
    void showToolbar();

    /**
     * @brief Скрыть тулбар
     */
    void hideToolbar();

    /**
     * @brief Сместить тулбар в заданную точку
     */
    void moveToolbar(const QPoint& _position);

signals:

protected:
    void paintEvent(QPaintEvent* _event) override;

    /**
     * @brief Обновить переводы
     */
    void updateTranslations() override;

    /**
     * @brief Обновляем виджет при изменении дизайн системы
     */
    void designSystemChangeEvent(DesignSystemChangeEvent* _event) override;

private:
    class Implementation;
    QScopedPointer<Implementation> d;
};

} // namespace Ui
