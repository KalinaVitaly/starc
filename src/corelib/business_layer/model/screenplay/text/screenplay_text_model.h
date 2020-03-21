#pragma once

#include <business_layer/model/abstract_model.h>

/*

  наложение патчей
  формирование дочерних моделей
    для навигатора
    для редактора текста
        для аутлайна
        для сценария
  обновление данных из дочерних моделей

 */


namespace BusinessLayer
{

class CharactersModel;
class LocationsModel;
class ScreenplayDictionariesModel;
class ScreenplayTextModelItem;

/**
 * @brief Модель текста сценария
 */
class ScreenplayTextModel : public AbstractModel
{
    Q_OBJECT

public:
    explicit ScreenplayTextModel(QObject* _parent = nullptr);
    ~ScreenplayTextModel() override;

    /**
     * @brief Добавить элемент в конец
     */
    void appendItem(ScreenplayTextModelItem* _item, ScreenplayTextModelItem* _parentItem = nullptr);

    /**
     * @brief Добавить элемент в начало
     */
    void prependItem(ScreenplayTextModelItem* _item, ScreenplayTextModelItem* _parentItem = nullptr);

    /**
     * @brief Вставить элемент после заданного
     */
    void insertItem(ScreenplayTextModelItem* _item, ScreenplayTextModelItem* _afterSiblingItem);

    /**
     * @brief Удалить заданный элемент
     */
    void removeItem(ScreenplayTextModelItem* _item);

    /**
     * @brief Удалить элемент, не удаляя его дочерние элементы
     * @note Дочерние элементы размещаются в родителе на месте удаляемого элементв
     */
    void removeItemWithoutChilds(ScreenplayTextModelItem* _item);

    /**
     * @brief Обновить заданный элемент
     */
    void updateItem(ScreenplayTextModelItem* _item);

    /**
     * @brief Реализация древовидной модели
     */
    /** @{ */
    QModelIndex index(int _row, int _column, const QModelIndex& _parent = {}) const override;
    QModelIndex parent(const QModelIndex& _child) const override;
    int columnCount( const QModelIndex& _parent = {}) const override;
    int rowCount(const QModelIndex &_parent = {}) const override;
    Qt::ItemFlags flags(const QModelIndex &_index) const override;
    QVariant data(const QModelIndex &_index, int _role) const override;
    //! Реализация перетаскивания элементов
    bool canDropMimeData(const QMimeData* _data, Qt::DropAction _action, int _row, int _column, const QModelIndex& _parent = {}) const override;
    bool dropMimeData(const QMimeData* _data, Qt::DropAction _action, int _row, int _column, const QModelIndex& _parent = {}) override;
    QMimeData* mimeData(const QModelIndexList& _indexes) const override;
    QStringList mimeTypes() const override;
    Qt::DropActions supportedDragActions() const override;
    Qt::DropActions supportedDropActions() const override;
    /** @} */

    /**
     * @brief Получить элемент находящийся в заданном индексе
     */
    ScreenplayTextModelItem* itemForIndex(const QModelIndex& _index) const;

    /**
     * @brief Получить индекс заданного элемента
     */
    QModelIndex indexForItem(ScreenplayTextModelItem* _item) const;

    /**
     * @brief Задать модель справочников сценария
     */
    void setDictionariesModel(ScreenplayDictionariesModel* _model);
    ScreenplayDictionariesModel* dictionariesModel() const;

    /**
     * @brief Задать модель персонажей проекта
     */
    void setCharactersModel(CharactersModel* _model);
    CharactersModel* charactersModel() const;

    /**
     * @brief Задать модель локаций проекта
     */
    void setLocationsModel(LocationsModel* _model);
    LocationsModel* locationsModel() const;

protected:
    /**
     * @brief Реализация модели для работы с документами
     */
    /** @{ */
    void initDocument() override;
    void clearDocument() override;
    QByteArray toXml() const override;
    /** @} */

private:
    class Implementation;
    QScopedPointer<Implementation> d;
};

} // namespace BusinessLayer
