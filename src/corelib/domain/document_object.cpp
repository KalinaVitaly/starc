#include "document_object.h"


namespace Domain
{

namespace {

const QHash<DocumentObjectType, QByteArray> kDocumentObjectTypeToMimeType
    = {{ DocumentObjectType::Undefined, "application/x-starc/document/undefined" },
       { DocumentObjectType::Structure, "application/x-starc/document/structure" },
       { DocumentObjectType::Project, "application/x-starc/document/project" },
       { DocumentObjectType::RecycleBin, "application/x-starc/document/recycle-bin" },
       { DocumentObjectType::Screenplay, "application/x-starc/document/screenplay" },
       { DocumentObjectType::ScreenplayTitlePage, "application/x-starc/document/screenplay/title-page" },
       { DocumentObjectType::ScreenplaySynopsis, "application/x-starc/document/screenplay/synopsis" },
       { DocumentObjectType::ScreenplayOutline, "application/x-starc/document/screenplay/outline" },
       { DocumentObjectType::ScreenplayText, "application/x-starc/document/screenplay/text" },
       { DocumentObjectType::ScreenplayDictionaries, "application/x-starc/document/screenplay/dictionaries" }};
const QHash<DocumentObjectType, QString> kDocumentObjectTypeToIcon
    = {{ DocumentObjectType::Undefined, "\uf78a" },
       { DocumentObjectType::Structure, "\uf78a" },
       { DocumentObjectType::Project, "\ufab6" },
       { DocumentObjectType::RecycleBin, "\uf1c0" },
       { DocumentObjectType::Screenplay, "\ufb9e" },
       { DocumentObjectType::ScreenplayTitlePage, "\uf0be" },
       { DocumentObjectType::ScreenplaySynopsis, "\uf21a" },
       { DocumentObjectType::ScreenplayOutline, "\uf21a" },
       { DocumentObjectType::ScreenplayText, "\uf21a" }};
}

QByteArray mimeTypeFor(DocumentObjectType _type)
{
    return kDocumentObjectTypeToMimeType.value(_type);
}

DocumentObjectType typeFor(const QByteArray& _mime)
{
    return static_cast<DocumentObjectType>(kDocumentObjectTypeToMimeType.key(_mime));
}

QString iconForType(DocumentObjectType _type)
{
    return kDocumentObjectTypeToIcon.value(_type);
}

const QUuid& DocumentObject::uuid() const
{
    return m_uuid;
}

void DocumentObject::setUuid(const QUuid& _uuid)
{
    if (m_uuid == _uuid) {
        return;
    }

    m_uuid = _uuid;
    markChangesNotStored();
}

DocumentObjectType DocumentObject::type() const
{
    return m_type;
}

void DocumentObject::setType(DocumentObjectType _type)
{
    if (m_type == _type) {
        return;
    }

    m_type = _type;
    markChangesNotStored();
}

const QByteArray& DocumentObject::content() const
{
    return m_content;
}

void DocumentObject::setContent(const QByteArray& _content)
{
    //
    // NOTE: Тут специально нет проверки, т.к. данные могут быть очень большими
    //

    m_content = _content;
    markChangesNotStored();
}

DocumentObject::DocumentObject(const Identifier& _id, const QUuid& _uuid, DocumentObjectType _type,
    const QByteArray& _content)
    : DomainObject(_id),
      m_uuid(_uuid),
      m_type(_type),
      m_content(_content)
{
}

} // namespace Domain
