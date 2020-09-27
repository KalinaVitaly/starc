#pragma once

#include "screenplay_text_model_item.h"

#include <QString>

#include <chrono>

class QDomElement;


namespace BusinessLayer
{

/**
 * @brief Класс элементов сцен модели сценария
 */
class CORE_LIBRARY_EXPORT ScreenplayTextModelSceneItem : public ScreenplayTextModelItem
{
public:
    /**
     * @brief Номер сцены
     */
    struct Number {
        QString value;
    };

    /**
     * @brief Роли данных из модели
     */
    enum DataRole {
        SceneNumberRole = Qt::UserRole + 1,
        SceneHeadingRole,
        SceneTextRole,
        SceneInlineNotesSizeRole,
        SceneReviewMarksSizeRole,
        SceneDurationRole
    };

public:
    ScreenplayTextModelSceneItem();
    explicit ScreenplayTextModelSceneItem(const QDomElement& _node);
    ~ScreenplayTextModelSceneItem() override;

    /**
     * @brief Номер сцены
     */
    Number number() const;
    void setNumber(int _number);

    /**
     * @brief Длительность сцены
     */
    std::chrono::milliseconds duration() const;

    /**
     * @brief Определяем интерфейс получения данных сцены
     */
    QVariant data(int _role) const override;

    /**
     * @brief Определяем интерфейс для получения XML блока
     */
    QString toXml() const override;
    QString toXml(ScreenplayTextModelItem* _from, int _fromPosition, ScreenplayTextModelItem* _to, int _toPosition) const;

protected:
    /**
     * @brief Обновляем текст сцены при изменении кого-то из детей
     */
    void handleChange() override;

private:
    class Implementation;
    QScopedPointer<Implementation> d;
};

} // namespace BusinessLayer
