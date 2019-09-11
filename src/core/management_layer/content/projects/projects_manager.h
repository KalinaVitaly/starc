#pragma once

#include <QObject>


namespace ManagementLayer
{

/**
 * @brief Менеджер экрана со списком проектов
 */
class ProjectsManager : public QObject
{
    Q_OBJECT

public:
    explicit ProjectsManager(QObject* _parent, QWidget* _parentWidget);
    ~ProjectsManager() override;

    QWidget* toolBar() const;
    QWidget* navigator() const;
    QWidget* view() const;

private:
    class Implementation;
    QScopedPointer<Implementation> d;
};

} // namespace ManagementLayer
