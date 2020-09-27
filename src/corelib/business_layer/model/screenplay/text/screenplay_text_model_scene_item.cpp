#include "screenplay_text_model_scene_item.h"

#include "screenplay_text_model_splitter_item.h"
#include "screenplay_text_model_text_item.h"

#include <business_layer/templates/screenplay_template.h>

#include <utils/helpers/text_helper.h>

#include <QDomElement>
#include <QUuid>
#include <QVariant>

#include <optional>


namespace BusinessLayer
{

namespace {
    const QString kSceneTag = QLatin1String("scene");
    const QString kUuidAttribute = QLatin1String("uuid");
    const QString kPlotsAttribute = QLatin1String("plots");
    const QString kOmitedAttribute = QLatin1String("omited");
    const QString kNumberTag = QLatin1String("number");
    const QString kNumberValueAttribute = QLatin1String("value");
    const QString kStampTag = QLatin1String("stamp");
    const QString kPlannedDurationTag = QLatin1String("planned_duration");
    const QString kContentTag = QLatin1String("content");
    const QString kSplitterTag = QLatin1String("splitter");
}

class ScreenplayTextModelSceneItem::Implementation
{
public:
    Implementation();


    /**
     * @brief Идентификатор сцены
     */
    QUuid uuid;

    /**
     * @brief Пропущена ли сцена
     */
    bool isOmited = false;

    /**
     * @brief Номер сцены
     */
    std::optional<Number> number;

    /**
     * @brief Штамп на сцене
     */
    QString stamp;

    /**
     * @brief Запланированная длительность сцены
     */
    std::optional<int> plannedDuration;

    //
    // Ридонли свойства, которые формируются по ходу работы со сценарием
    //

    /**
     * @brief Заголовок сцены
     */
    QString heading;

    /**
     * @brief Текст сцены
     */
    QString text;

    /**
     * @brief Количество заметок по тексту
     */
    int inlineNotesSize = 0;

    /**
     * @brief Количество редакторских заметок
     */
    int reviewMarksSize = 0;

    /**
     * @brief Длительность сцены
     */
    std::chrono::milliseconds duration = std::chrono::milliseconds{0};
};

ScreenplayTextModelSceneItem::Implementation::Implementation()
    : uuid(QUuid::createUuid())
{
}


// ****


ScreenplayTextModelSceneItem::ScreenplayTextModelSceneItem()
    : ScreenplayTextModelItem(ScreenplayTextModelItemType::Scene),
      d(new Implementation)
{
}

ScreenplayTextModelSceneItem::ScreenplayTextModelSceneItem(const QDomElement& _node)
    : ScreenplayTextModelItem(ScreenplayTextModelItemType::Scene),
      d(new Implementation)
{
    Q_ASSERT(_node.tagName() == kSceneTag);

    d->uuid = _node.attribute(kUuidAttribute);
    //
    // TODO: plots
    //
    d->isOmited = _node.hasAttribute(kOmitedAttribute);
    const auto numberNode = _node.firstChildElement(kNumberTag);
    if (!numberNode.isNull()) {
        d->number = { numberNode.attribute(kNumberValueAttribute) };
    }
    const auto stampNode = _node.firstChildElement(kStampTag);
    if (!stampNode.isNull()) {
        d->stamp = TextHelper::fromHtmlEscaped(_node.text());
    }
    const auto plannedDurationNode = _node.firstChildElement(kPlannedDurationTag);
    if (!plannedDurationNode.isNull()) {
        d->plannedDuration = plannedDurationNode.text().toInt();
    }
    auto childNode = _node.firstChildElement(kContentTag).firstChildElement();
    while (!childNode.isNull()) {
        if (childNode.tagName() == kSceneTag) {
            appendItem(new ScreenplayTextModelSceneItem(childNode));
        } else if (childNode.tagName() == kSplitterTag) {
            appendItem(new ScreenplayTextModelSplitterItem(childNode));
        } else {
            appendItem(new ScreenplayTextModelTextItem(childNode));
        }
        childNode = childNode.nextSiblingElement();
    }

    //
    // Соберём заголовок, текст сцены и прочие параметры
    //
    handleChange();
}

ScreenplayTextModelSceneItem::~ScreenplayTextModelSceneItem() = default;

ScreenplayTextModelSceneItem::Number ScreenplayTextModelSceneItem::number() const
{
    if (!d->number.has_value()) {
        return {};
    }

    return d->number.value();
}

void ScreenplayTextModelSceneItem::setNumber(int _number)
{
    const auto newNumber = QString("%1.").arg(_number);
    if (d->number.has_value()
        && d->number->value == newNumber) {
        return;
    }

    d->number = { newNumber };
    //
    // Т.к. пока мы не сохраняем номера, в указании, что произошли изменения нет смысла
    //
//    setChanged(true);
}

std::chrono::milliseconds ScreenplayTextModelSceneItem::duration() const
{
    return d->duration;
}

QVariant ScreenplayTextModelSceneItem::data(int _role) const
{
    switch (_role) {
        case Qt::DecorationRole: {
            return u8"\U000f021a";
        }

        case SceneNumberRole: {
            if (d->number.has_value()) {
                return d->number->value;
            }
            return {};
        }

        case SceneHeadingRole: {
            return d->heading;
        }

        case SceneTextRole: {
            return d->text;
        }

        case SceneInlineNotesSizeRole: {
            return d->inlineNotesSize;
        }

        case SceneReviewMarksSizeRole: {
            return d->reviewMarksSize;
        }

        case SceneDurationRole: {
            const int duration = std::chrono::duration_cast<std::chrono::seconds>(d->duration).count();
            return duration;
        }

        default: {
            return ScreenplayTextModelItem::data(_role);
        }
    }
}

QString ScreenplayTextModelSceneItem::toXml() const
{
    return toXml(nullptr, 0, nullptr, 0);
}

QString ScreenplayTextModelSceneItem::toXml(ScreenplayTextModelItem* _from, int _fromPosition,
    ScreenplayTextModelItem* _to, int _toPosition) const
{
    QByteArray xml;
    //
    // TODO: plots
    //
    xml += QString("<%1 %2=\"%3\" %4=\"%5\" %6>\n")
           .arg(kSceneTag,
                kUuidAttribute, d->uuid.toString(),
                kPlotsAttribute, {},
                (d->isOmited ? QString("%1=\"true\"").arg(kOmitedAttribute) : ""));
    if (d->number.has_value()) {
        xml += QString("<%1 %2=\"%3\"/>\n")
               .arg(kNumberTag, kNumberValueAttribute, d->number->value);
    }
    if (!d->stamp.isEmpty()) {
        xml += QString("<%1><![CDATA[%2]]></%1>\n").arg(kStampTag, TextHelper::toHtmlEscaped(d->stamp));
    }
    if (d->plannedDuration.has_value()) {
        xml += QString("<%1>%2</%1>\n").arg(kPlannedDurationTag, QString::number(*d->plannedDuration));
    }
    xml.append(QString("<%1>\n").arg(kContentTag));
    for (int childIndex = 0; childIndex < childCount(); ++childIndex) {
        auto child = childAt(childIndex);

        //
        // Нетекстовые блоки, просто добавляем к общему xml
        //
        if (child->type() != ScreenplayTextModelItemType::Text) {
            xml += child->toXml();
            continue;
        }

        //
        // Текстовые блоки, в зависимости от необходимости вставить блок целиком, или его часть
        //
        auto textItem = static_cast<ScreenplayTextModelTextItem*>(child);
        if (textItem == _to) {
            if (textItem == _from) {
                xml += textItem->toXml(_fromPosition, _toPosition - _fromPosition);
            } else {
                xml += textItem->toXml(0, _toPosition);
            }
            break;
        }
        //
        else if (textItem == _from) {
            xml += textItem->toXml(_fromPosition, textItem->text().length() - _fromPosition);
        } else {
            xml += textItem->toXml();
        }
    }
    xml.append(QString("</%1>\n").arg(kContentTag));
    xml.append(QString("</%1>\n").arg(kSceneTag));

    return xml;
}

void ScreenplayTextModelSceneItem::handleChange()
{
    d->heading.clear();
    d->text.clear();
    d->inlineNotesSize = 0;
    d->reviewMarksSize = 0;
    d->duration = std::chrono::seconds{0};

    for (int childIndex = 0; childIndex < childCount(); ++childIndex) {
        auto child = childAt(childIndex);
        if (child->type() != ScreenplayTextModelItemType::Text) {
            continue;
        }

        auto childTextItem = static_cast<ScreenplayTextModelTextItem*>(child);

        //
        // Собираем текст
        //
        switch (childTextItem->paragraphType()) {
            case ScreenplayParagraphType::SceneHeading: {
                d->heading = TextHelper::smartToUpper(childTextItem->text());
                break;
            }

            case ScreenplayParagraphType::InlineNote: {
                ++d->inlineNotesSize;
                break;
            }

            default: {
                d->text.append(childTextItem->text() + " ");
                d->reviewMarksSize += std::count_if(childTextItem->reviewMarks().begin(),
                                                    childTextItem->reviewMarks().end(),
                                                    [] (const ScreenplayTextModelTextItem::ReviewMark& _reviewMark) {
                                                        return !_reviewMark.isDone;
                                                    });
                break;
            }
        }

        //
        // Собираем хронометраж
        //
        d->duration += childTextItem->duration();
    }
}

} // namespace BusinessLayer
