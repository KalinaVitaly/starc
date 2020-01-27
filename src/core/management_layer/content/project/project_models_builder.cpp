#include "project_models_builder.h"

#include <business_layer/model/project/project_information_model.h>
#include <business_layer/model/screenplay/screenplay_information_model.h>
#include <business_layer/model/text/text_model.h>

#include <domain/document_object.h>


namespace ManagementLayer
{

class ProjectModelsBuilder::Implementation
{
public:
    explicit Implementation(BusinessLayer::AbstractImageWrapper* _imageWrapper);

    QHash<Domain::DocumentObject*, BusinessLayer::AbstractModel*> documentsToModels;
    BusinessLayer::AbstractImageWrapper* imageWrapper = nullptr;
};

ProjectModelsBuilder::Implementation::Implementation(BusinessLayer::AbstractImageWrapper* _imageWrapper)
    : imageWrapper(_imageWrapper)
{
}


// ****


ProjectModelsBuilder::ProjectModelsBuilder(BusinessLayer::AbstractImageWrapper* _imageWrapper)
    : d(new Implementation(_imageWrapper))
{
}

ProjectModelsBuilder::~ProjectModelsBuilder()
{
    clear();
}

void ProjectModelsBuilder::clear()
{
    for (auto model : d->documentsToModels.values()) {
        model->disconnect();
        model->clear();
    }

    qDeleteAll(d->documentsToModels.values());
    d->documentsToModels.clear();
}

BusinessLayer::AbstractModel* ProjectModelsBuilder::modelFor(Domain::DocumentObject* _document)
{
    if (_document == nullptr) {
        return nullptr;
    }

    if (!d->documentsToModels.contains(_document)) {
        BusinessLayer::AbstractModel* model = nullptr;
        switch (_document->type()) {
            case Domain::DocumentObjectType::Project: {
                model = new BusinessLayer::ProjectInformationModel;
                break;
            }

            case Domain::DocumentObjectType::Screenplay: {
                model = new BusinessLayer::ScreenplayInformationModel;
                break;
            }

            case Domain::DocumentObjectType::ScreenplayTitlePage: {
                model = new BusinessLayer::TextModel;
                break;
            }

            default: {
                return nullptr;
            }
        }
        model->setImageWrapper(d->imageWrapper);

        model->setDocument(_document);

        d->documentsToModels.insert(_document, model);
    }

    return d->documentsToModels.value(_document);
}

QVector<BusinessLayer::AbstractModel*> ProjectModelsBuilder::models() const
{
    return d->documentsToModels.values().toVector();
}

} // namespace ManagementLayer
