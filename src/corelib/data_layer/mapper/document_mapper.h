#pragma once

#include "abstract_mapper.h"

namespace Domain {
    class DocumentObject;
}


namespace DataMappingLayer
{

/**
 * @brief Отображатель данных документов из БД в объекты
 */
class DocumentMapper : public AbstractMapper
{
public:
    Domain::DocumentObject* find(const Domain::Identifier& _id);
    Domain::DocumentObject* find(const QUuid& _uuid);
    Domain::DocumentObject* findStructure();

    void insert(Domain::DocumentObject* _object);
    bool update(Domain::DocumentObject* _object);
    void remove(Domain::DocumentObject* _object);

protected:
    QString findStatement(const Domain::Identifier& _id) const override;
    QString findAllStatement() const override;
    QString findLastOneStatement() const override;
    QString insertStatement(Domain::DomainObject* _object, QVariantList& _insertValues) const override;
    QString updateStatement(Domain::DomainObject* _object, QVariantList& _updateValues) const override;
    QString deleteStatement(Domain::DomainObject* _object, QVariantList& _deleteValues) const override;

protected:
    Domain::DomainObject* doLoad(const Domain::Identifier& _id, const QSqlRecord& _record) override;
    void doLoad(Domain::DomainObject* _object, const QSqlRecord& _record) override;

private:
    DocumentMapper() = default;
    friend class MapperFacade;
};

} // namespace DataMappingLayer
