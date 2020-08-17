#pragma once

#include <corelib_global.h>

#include <business_layer/model/abstract_model_item.h>


namespace BusinessLayer
{


/**
 * @brief Перечисление типов элементов модели сценария
 */
enum class ScreenplayTextModelItemType {
    Folder,
    Scene,
    Text,
    Splitter
};


/**
 * @brief Базовый класс элемента модели сценария
 */
class CORE_LIBRARY_EXPORT ScreenplayTextModelItem : public AbstractModelItem
{
public:
    ScreenplayTextModelItem(ScreenplayTextModelItemType _type);
    ~ScreenplayTextModelItem() override;

    /**
     * @brief Получить тип элемента
     */
    ScreenplayTextModelItemType type() const;

    /**
     * @brief Переопределяем интерфейс для возврата элемента собственного класса
     */
    ScreenplayTextModelItem* parent() const override;
    ScreenplayTextModelItem* childAt(int _index) const override;

    /**
     * @brief Определяем интерфейс получения данных элемента
     */
    QVariant data(int _role) const override;

    /**
     * @brief Сформировать xml блока
     */
    virtual QString toXml() const = 0;

private:
    class Implementation;
    QScopedPointer<Implementation> d;
};

} // namespace BusinessLayer
