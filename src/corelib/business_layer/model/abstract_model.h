#pragma once

#include <QAbstractItemModel>

namespace Domain {
    class DocumentObject;
}


namespace BusinessLayer {

/**
 * @brief Абстрактная модель для работы над документами
 */
class AbstractModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit AbstractModel(const QVector<QString>& _tags, QObject* _parent = nullptr);
    ~AbstractModel() override;

    /**
     * @brief Получить документ модели
     */
    Domain::DocumentObject* document() const;

    /**
     * @brief Задать документ со структурой
     */
    void setDocument(Domain::DocumentObject* _document);

    /**
     * @brief Очистить все загруженные данные
     */
    void clear();

    /**
     * @brief Реализация базовых вещей для древовидной модели
     */
    /** @{ */
    QModelIndex index(int _row, int _column, const QModelIndex& _parent = {}) const override;
    QModelIndex parent(const QModelIndex& _child) const override;
    int columnCount( const QModelIndex& _parent = {}) const override;
    int rowCount(const QModelIndex &_parent = {}) const override;
    QVariant data(const QModelIndex &_index, int _role) const override;
    /** @} */

signals:
    /**
     * @brief Данные изменились
     */
    void contentsChanged(const QByteArray& _undo, const QByteArray& _redo);

protected:
    /**
     * @brief Настроить документ
     */
    virtual void initDocument() = 0;

    /**
     * @brief Очистить документ
     */
    virtual void clearDocument() = 0;

    /**
     * @brief Сформировать xml-содержимое документа
     */
    virtual QByteArray toXml() const = 0;

    /**
     * @brief Установить данные в документ
     */
    void updateDocumentContent();

private:
    class Implementation;
    QScopedPointer<Implementation> d;
};

} // namespace BusinessLayer
