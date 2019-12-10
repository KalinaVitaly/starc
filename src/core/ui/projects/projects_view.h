#pragma once

#include <ui/widgets/stack_widget/stack_widget.h>

namespace Domain {
    class Project;
    class ProjectsModel;
}


namespace Ui
{

/**
 * @brief Представление списка проектов
 */
class ProjectsView : public StackWidget
{
    Q_OBJECT

public:
    explicit ProjectsView(QWidget* _parent = nullptr);
    ~ProjectsView() override;

    /**
     * @brief Задать список проектов
     */
    void setProjects(Domain::ProjectsModel* _projects);

    /**
     * @brief Показать страницу без проектов
     */
    void showEmptyPage();

    /**
     * @brief Показать страницу со списком проектов
     */
    void showProjectsPage();

signals:
    /**
     * @brief Пользователь нажал кнопку создания истории
     */
    void createProjectPressed();

    /**
     * @brief Пользователь нажал кнопку открытия истории
     */
    void openProjectPressed();

    /**
     * @brief Пользователь хочет перенести проект в облако
     */
    void moveProjectToCloudRequested(const Domain::Project& _project);

    /**
     * @brief Пользователь хочет скрыть проект
     */
    void hideProjectRequested(const Domain::Project& _project);

    /**
     * @brief Пользователь хочет изменить название проекта
     */
    void changeProjectNameRequested(const Domain::Project& _project);

    /**
     * @brief Пользователь хочет удалить проект
     */
    void removeProjectRequested(const Domain::Project& _project);

protected:
    /**
     * @brief Переопределяем для корректировки положения тулбара действий над проектами
     */
    void resizeEvent(QResizeEvent* _event) override;

    /**
     * @brief Обновить переводы
     */
    void updateTranslations() override;

    /**
     * @brief Обновляем навигатор при изменении дизайн системы
     */
    void designSystemChangeEvent(DesignSystemChangeEvent* _event) override;

private:
    class Implementation;
    QScopedPointer<Implementation> d;
};

} // namespace Ui
