#include "screenplay_information_model.h"

#include <business_layer/model/abstract_image_wrapper.h>

#include <domain/document_object.h>

#include <utils/helpers/image_helper.h>
#include <utils/helpers/text_helper.h>

#include <QDomDocument>
#include <QPixmap>


namespace BusinessLayer
{

namespace {
    const QString kDocumentKey = "document";
    const QString kNameKey = "name";
    const QString kLoglineKey = "logline";
    const QString kTitlePageVisibleKey = "title_page_visible";
    const QString kSynopsisVisibleKey = "synopsis_visible";
    const QString kOutlineVisibleKey = "outline_visible";
    const QString kScreenplayTextVisibleKey = "screenplay_text_visible";
    const QString kScreenplayStatisticsVisibleKey = "screenplay_statistics_visible";
    const QString kHeaderKey = "header";
    const QString kPrintHeaderOnTitlePageKey = "print_header_on_title";
    const QString kFooterKey = "footer";
    const QString kPrintFooterOnTitlePageKey = "print_footer_on_title";
    const QString kScenesNumbersPrefixKey = "scenes_numbers_prefix";
    const QString kScenesNumberingStartAtKey = "scenes_numbering_start_at";
}

class ScreenplayInformationModel::Implementation
{
public:
    QString name;
    QString logline;
    bool titlePageVisible = true;
    bool synopsisVisible = true;
    bool outlineVisible = true;
    bool screenplayTextVisible = true;
    bool screenplayStatisticsVisible = true;
    QString header;
    bool printHeaderOnTitlePage = true;
    QString footer;
    bool printFooterOnTitlePage = true;
    QString scenesNumbersPrefix;
    int scenesNumberingStartAt = 1;
};


// ****


ScreenplayInformationModel::ScreenplayInformationModel(QObject* _parent)
    : AbstractModel({ kDocumentKey, kNameKey, kLoglineKey, kHeaderKey, kPrintHeaderOnTitlePageKey,
                      kFooterKey, kPrintFooterOnTitlePageKey, kScenesNumbersPrefixKey,
                      kScenesNumberingStartAtKey },
                    _parent),
      d(new Implementation)
{
    connect(this, &ScreenplayInformationModel::nameChanged, this, &ScreenplayInformationModel::updateDocumentContent);
    connect(this, &ScreenplayInformationModel::loglineChanged, this, &ScreenplayInformationModel::updateDocumentContent);
    connect(this, &ScreenplayInformationModel::titlePageVisibleChanged, this, &ScreenplayInformationModel::updateDocumentContent);
    connect(this, &ScreenplayInformationModel::synopsisVisibleChanged, this, &ScreenplayInformationModel::updateDocumentContent);
    connect(this, &ScreenplayInformationModel::outlineVisibleChanged, this, &ScreenplayInformationModel::updateDocumentContent);
    connect(this, &ScreenplayInformationModel::screenplayTextVisibleChanged, this, &ScreenplayInformationModel::updateDocumentContent);
    connect(this, &ScreenplayInformationModel::screenplayStatisticsVisibleChanged, this, &ScreenplayInformationModel::updateDocumentContent);
    connect(this, &ScreenplayInformationModel::headerChanged, this, &ScreenplayInformationModel::updateDocumentContent);
    connect(this, &ScreenplayInformationModel::printHeaderOnTitlePageChanged, this, &ScreenplayInformationModel::updateDocumentContent);
    connect(this, &ScreenplayInformationModel::footerChanged, this, &ScreenplayInformationModel::updateDocumentContent);
    connect(this, &ScreenplayInformationModel::printFooterOnTitlePageChanged, this, &ScreenplayInformationModel::updateDocumentContent);
    connect(this, &ScreenplayInformationModel::scenesNumbersPrefixChanged, this, &ScreenplayInformationModel::updateDocumentContent);
    connect(this, &ScreenplayInformationModel::scenesNumberingStartAtChanged, this, &ScreenplayInformationModel::updateDocumentContent);
}

ScreenplayInformationModel::~ScreenplayInformationModel() = default;

const QString& ScreenplayInformationModel::name() const
{
    return d->name;
}

void ScreenplayInformationModel::setName(const QString& _name)
{
    if (d->name == _name) {
        return;
    }

    d->name = _name;
    emit nameChanged(d->name);
    emit documentNameChanged(d->name);
}

void ScreenplayInformationModel::setDocumentName(const QString& _name)
{
    setName(_name);
}

const QString& ScreenplayInformationModel::logline() const
{
    return d->logline;
}

void ScreenplayInformationModel::setLogline(const QString& _logline)
{
    if (d->logline == _logline) {
        return;
    }

    d->logline = _logline;
    emit loglineChanged(d->logline);
}

bool ScreenplayInformationModel::titlePageVisible() const
{
    return d->titlePageVisible;
}

void ScreenplayInformationModel::setTitlePageVisible(bool _visible)
{
    if (d->titlePageVisible == _visible) {
        return;
    }

    d->titlePageVisible = _visible;
    emit titlePageVisibleChanged(d->titlePageVisible);
}

bool ScreenplayInformationModel::synopsisVisible() const
{
    return d->synopsisVisible;
}

void ScreenplayInformationModel::setSynopsisVisible(bool _visible)
{
    if (d->synopsisVisible == _visible) {
        return;
    }

    d->synopsisVisible = _visible;
    emit synopsisVisibleChanged(d->synopsisVisible);
}

bool ScreenplayInformationModel::outlineVisible() const
{
    return d->outlineVisible;
}

void ScreenplayInformationModel::setOutlineVisible(bool _visible)
{
    if (d->outlineVisible == _visible) {
        return;
    }

    d->outlineVisible = _visible;
    emit outlineVisibleChanged(d->outlineVisible);
}

bool ScreenplayInformationModel::screenplayTextVisible() const
{
    return d->screenplayTextVisible;
}

void ScreenplayInformationModel::setScreenplayTextVisible(bool _visible)
{
    if (d->screenplayTextVisible == _visible) {
        return;
    }

    d->screenplayTextVisible = _visible;
    emit screenplayTextVisibleChanged(d->screenplayTextVisible);
}

bool ScreenplayInformationModel::screenplayStatisticsVisible() const
{
    return d->screenplayStatisticsVisible;
}

void ScreenplayInformationModel::setScreenplayStatisticsVisible(bool _visible)
{
    if (d->screenplayStatisticsVisible == _visible) {
        return;
    }

    d->screenplayStatisticsVisible = _visible;
    emit screenplayStatisticsVisibleChanged(d->screenplayStatisticsVisible);
}

const QString& ScreenplayInformationModel::header() const
{
    return d->header;
}

void ScreenplayInformationModel::setHeader(const QString& _header)
{
    if (d->header == _header) {
        return;
    }

    d->header = _header;
    emit headerChanged(d->header);
}

bool ScreenplayInformationModel::printHeaderOnTitlePage() const
{
    return d->printHeaderOnTitlePage;
}

void ScreenplayInformationModel::setPrintHeaderOnTitlePage(bool _print)
{
    if (d->printHeaderOnTitlePage == _print) {
        return;
    }

    d->printHeaderOnTitlePage = _print;
    emit printHeaderOnTitlePageChanged(d->printHeaderOnTitlePage);
}

const QString& ScreenplayInformationModel::footer() const
{
    return d->footer;
}

void ScreenplayInformationModel::setFooter(const QString& _footer)
{
    if (d->footer == _footer) {
        return;
    }

    d->footer = _footer;
    emit footerChanged(d->footer);
}

bool ScreenplayInformationModel::printFooterOnTitlePage() const
{
    return d->printFooterOnTitlePage;
}

void ScreenplayInformationModel::setPrintFooterOnTitlePage(bool _print)
{
    if (d->printFooterOnTitlePage == _print) {
        return;
    }

    d->printFooterOnTitlePage = _print;
    emit printFooterOnTitlePageChanged(d->printFooterOnTitlePage);
}

const QString& ScreenplayInformationModel::scenesNumbersPrefix() const
{
    return d->scenesNumbersPrefix;
}

void ScreenplayInformationModel::setScenesNumbersPrefix(const QString& _prefix)
{
    if (d->scenesNumbersPrefix == _prefix) {
        return;
    }

    d->scenesNumbersPrefix = _prefix;
    emit scenesNumbersPrefixChanged(d->scenesNumbersPrefix);
}

int ScreenplayInformationModel::scenesNumberingStartAt() const
{
    return d->scenesNumberingStartAt;
}

void ScreenplayInformationModel::setScenesNumberingStartAt(int _startNumber)
{
    if (d->scenesNumberingStartAt == _startNumber) {
        return;
    }

    d->scenesNumberingStartAt = _startNumber;
    emit scenesNumberingStartAtChanged(d->scenesNumberingStartAt);
}

void ScreenplayInformationModel::initDocument()
{
    if (document() == nullptr) {
        return;
    }

    QDomDocument domDocument;
    const bool isContentValid = domDocument.setContent(document()->content());
    if (!isContentValid) {
        return;
    }

    const auto documentNode = domDocument.firstChildElement(kDocumentKey);
    d->name = documentNode.firstChildElement(kNameKey).text();
    d->logline = documentNode.firstChildElement(kLoglineKey).text();
    d->titlePageVisible = documentNode.firstChildElement(kTitlePageVisibleKey).text() == "true";
    d->synopsisVisible = documentNode.firstChildElement(kSynopsisVisibleKey).text() == "true";
    d->outlineVisible = documentNode.firstChildElement(kOutlineVisibleKey).text() == "true";
    d->screenplayTextVisible = documentNode.firstChildElement(kScreenplayTextVisibleKey).text() == "true";
    d->screenplayStatisticsVisible = documentNode.firstChildElement(kScreenplayStatisticsVisibleKey).text() == "true";
    d->header = documentNode.firstChildElement(kHeaderKey).text();
    d->printHeaderOnTitlePage = documentNode.firstChildElement(kPrintHeaderOnTitlePageKey).text() == "true";
    d->footer = documentNode.firstChildElement(kFooterKey).text();
    d->printFooterOnTitlePage = documentNode.firstChildElement(kPrintFooterOnTitlePageKey).text() == "true";
    d->scenesNumbersPrefix = documentNode.firstChildElement(kScenesNumbersPrefixKey).text();
    const auto scenesNumberingStartAtNode = documentNode.firstChildElement(kScenesNumberingStartAtKey);
    if (!scenesNumberingStartAtNode.isNull()) {
        d->scenesNumberingStartAt = scenesNumberingStartAtNode.text().toInt();
    }
}

void ScreenplayInformationModel::clearDocument()
{
    QSignalBlocker signalBlocker(this);

    setName({});
    setLogline({});
    setTitlePageVisible({});
    setSynopsisVisible({});
    setOutlineVisible({});
    setScreenplayTextVisible({});
    setScreenplayStatisticsVisible({});
    setHeader({});
    setPrintHeaderOnTitlePage({});
    setFooter({});
    setPrintFooterOnTitlePage({});
    setScenesNumbersPrefix({});
    setScenesNumberingStartAt({});
}

QByteArray ScreenplayInformationModel::toXml() const
{
    if (document() == nullptr) {
        return {};
    }

    QByteArray xml = "<?xml version=\"1.0\"?>\n";
    xml += QString("<%1 mime-type=\"%2\" version=\"1.0\">\n").arg(kDocumentKey, Domain::mimeTypeFor(document()->type()));
    xml += QString("<%1><![CDATA[%2]]></%1>\n").arg(kNameKey, d->name);
    xml += QString("<%1><![CDATA[%2]]></%1>\n").arg(kLoglineKey, d->logline);
    xml += QString("<%1><![CDATA[%2]]></%1>\n").arg(kTitlePageVisibleKey, d->titlePageVisible ? "true" : "false");
    xml += QString("<%1><![CDATA[%2]]></%1>\n").arg(kSynopsisVisibleKey, d->synopsisVisible ? "true" : "false");
    xml += QString("<%1><![CDATA[%2]]></%1>\n").arg(kOutlineVisibleKey, d->outlineVisible ? "true" : "false");
    xml += QString("<%1><![CDATA[%2]]></%1>\n").arg(kScreenplayTextVisibleKey, d->screenplayTextVisible ? "true" : "false");
    xml += QString("<%1><![CDATA[%2]]></%1>\n").arg(kScreenplayStatisticsVisibleKey, d->screenplayStatisticsVisible ? "true" : "false");
    xml += QString("<%1><![CDATA[%2]]></%1>\n").arg(kHeaderKey, d->header);
    xml += QString("<%1><![CDATA[%2]]></%1>\n").arg(kPrintHeaderOnTitlePageKey, d->printHeaderOnTitlePage ? "true" : "false");
    xml += QString("<%1><![CDATA[%2]]></%1>\n").arg(kFooterKey, d->footer);
    xml += QString("<%1><![CDATA[%2]]></%1>\n").arg(kPrintFooterOnTitlePageKey, d->printFooterOnTitlePage ? "true" : "false");
    xml += QString("<%1><![CDATA[%2]]></%1>\n").arg(kScenesNumbersPrefixKey, d->scenesNumbersPrefix);
    xml += QString("<%1><![CDATA[%2]]></%1>\n").arg(kScenesNumberingStartAtKey, QString::number(d->scenesNumberingStartAt));
    xml += QString("</%1>").arg(kDocumentKey);
    return xml;
}

} // namespace BusinessLayer
