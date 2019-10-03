#include "projects_cards.h"

#include <domain/project.h>

#include <ui/design_system/design_system.h>

#include <utils/helpers/color_helper.h>
#include <utils/helpers/image_helper.h>
#include <utils/helpers/text_helper.h>

#include <QAbstractItemModel>
#include <QApplication>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsRectItem>
#include <QResizeEvent>
#include <QVariantAnimation>


namespace Ui
{

namespace {

/**
 * @brief Карточка проекта
 */
class ProjectCard : public QGraphicsRectItem
{
public:
    explicit ProjectCard(QGraphicsItem* _parent = nullptr);

    /**
     * @brief Задать проект для отображения на карточке
     */
    void setProject(const Domain::Project& _project);

    /**
     * @brief Переопределяем метод, чтобы работал qgraphicsitem_cast
     */
    int type() const override;

    /**
     * @brief Отрисовка карточки
     */
    void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget) override;

    /**
     * @brief Анимируем hover
     */
    void hoverEnterEvent(QGraphicsSceneHoverEvent* _event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* _event) override;

    /**
     * @brief Реализуем клик на объекте
     */
    void mousePressEvent(QGraphicsSceneMouseEvent* _event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) override;

private:
    /**
     * @brief Проект для отображения на карточке
     */
    Domain::Project m_project;

    /**
     * @brief  Декорации тени при наведении
     */
    QVariantAnimation m_shadowHeightAnimation;

    /**
     * @brief Декорации при клике
     */
    QPointF m_decorationCenterPosition;
    QVariantAnimation m_decorationRadiusAnimation;
    QVariantAnimation m_decorationOpacityAnimation;
};

ProjectCard::ProjectCard(QGraphicsItem* _parent)
    : QGraphicsRectItem(_parent)
{
    setRect(0, 0, 410, 230);
    setAcceptHoverEvents(true);

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    m_shadowHeightAnimation.setStartValue(Ui::DesignSystem::card().minimumShadowBlurRadius());
    m_shadowHeightAnimation.setEndValue(Ui::DesignSystem::card().maximumShadowBlurRadius());
    m_shadowHeightAnimation.setEasingCurve(QEasingCurve::OutQuad);
    m_shadowHeightAnimation.setDuration(160);
    QObject::connect(&m_shadowHeightAnimation, &QVariantAnimation::valueChanged, [this] { update(); });

    m_decorationRadiusAnimation.setEasingCurve(QEasingCurve::InQuad);
    m_decorationRadiusAnimation.setStartValue(1.0);
    m_decorationRadiusAnimation.setDuration(240);
    QObject::connect(&m_decorationRadiusAnimation, &QVariantAnimation::valueChanged, [this] { update(); });

    m_decorationOpacityAnimation.setEasingCurve(QEasingCurve::InQuad);
    m_decorationOpacityAnimation.setDuration(240);
    QObject::connect(&m_decorationOpacityAnimation, &QVariantAnimation::valueChanged, [this] { update(); });
}

void ProjectCard::setProject(const Domain::Project& _project)
{
    if (m_project == _project) {
        return;
    }

    m_project = _project;
    update();
}

int ProjectCard::type() const
{
    return QGraphicsItem::UserType;
}

void ProjectCard::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget)
{
    Q_UNUSED(_option);
    Q_UNUSED(_widget);

    const QRectF backgroundRect = rect().marginsRemoved(Ui::DesignSystem::card().shadowMargins());
    if (!backgroundRect.isValid()) {
        return;
    }

    //
    // Заливаем фон
    //
    QPixmap backgroundImage(backgroundRect.size().toSize());
    backgroundImage.fill(Qt::transparent);
    QPainter backgroundImagePainter(&backgroundImage);
    backgroundImagePainter.setPen(Qt::NoPen);
    backgroundImagePainter.setBrush(Ui::DesignSystem::color().background());
    const qreal borderRadius = Ui::DesignSystem::card().borderRadius();
    backgroundImagePainter.drawRoundedRect(QRect({0,0}, backgroundImage.size()), borderRadius, borderRadius);
    //
    // ... рисуем тень
    //
    const qreal shadowHeight = std::max(Ui::DesignSystem::floatingToolBar().minimumShadowBlurRadius(),
                                        m_shadowHeightAnimation.currentValue().toReal());
    const QPixmap shadow
            = ImageHelper::dropShadow(backgroundImage,
                                      Ui::DesignSystem::floatingToolBar().shadowMargins(),
                                      shadowHeight,
                                      Ui::DesignSystem::color().shadow());
    _painter->drawPixmap(0, 0, shadow);
    //
    // ... рисуем сам фон
    //
    _painter->setPen(Qt::NoPen);
    _painter->setBrush(Ui::DesignSystem::color().background());
    _painter->drawRoundedRect(backgroundRect, borderRadius, borderRadius);

    //
    // TODO: Постер
    //
    const QPixmap& poster = m_project.poster();
    const QRectF posterRect(backgroundRect.topLeft(),
                            poster.size().scaled(backgroundRect.size().toSize(), Qt::KeepAspectRatio));
    _painter->drawPixmap(posterRect, poster, poster.rect());

    //
    // Заголовок
    //
    _painter->setPen(Ui::DesignSystem::color().onBackground());
    _painter->setFont(Ui::DesignSystem::font().h6());
    const QFontMetricsF textFontMetrics(Ui::DesignSystem::font().h6());
    const QRectF textRect(posterRect.right() + Ui::DesignSystem::layout().px16(),
                          backgroundRect.top() + Ui::DesignSystem::layout().px8(),
                          backgroundRect.width() - posterRect.width() - Ui::DesignSystem::layout().px12() * 2,
                          textFontMetrics.lineSpacing());
    _painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, textFontMetrics.elidedText(m_project.name(), Qt::ElideRight, textRect.width()));

    //
    // Путь
    //
    const QColor betweenBackgroundColor = ColorHelper::colorBetween(Ui::DesignSystem::color().onBackground(),
                                                                    Ui::DesignSystem::color().background());
    _painter->setPen(betweenBackgroundColor);
    _painter->setFont(Ui::DesignSystem::font().body2());
    const QFontMetricsF fontMetrics(_painter->font());
    const QRectF pathRect(textRect.left(), textRect.bottom() + Ui::DesignSystem::layout().px4(),
                          textRect.width(), fontMetrics.lineSpacing());
    _painter->drawText(pathRect, Qt::AlignLeft | Qt::AlignVCenter, fontMetrics.elidedText(m_project.path(), Qt::ElideLeft, pathRect.width()));

    //
    // Логлайн
    //
    _painter->setPen(Ui::DesignSystem::color().onBackground());
    const QRectF loglineRect(pathRect.left(), pathRect.bottom() + Ui::DesignSystem::layout().px4(),
                             pathRect.width(), fontMetrics.lineSpacing() * 5);
    _painter->drawText(loglineRect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, m_project.logline());

    //
    // Дата последнего изменения
    //
    _painter->setPen(betweenBackgroundColor);
    const qreal lastDateHeight = fontMetrics.lineSpacing() + Ui::DesignSystem::layout().px8() * 2;
    const QRectF lastDateRect(loglineRect.left(), backgroundRect.bottom() - lastDateHeight,
                             pathRect.width(), lastDateHeight);
    _painter->drawText(lastDateRect, Qt::AlignLeft | Qt::AlignTop, m_project.displayLastEditDate());

    //
    // Иконки действий
    //
    // TODO: иконки в зависимости от того, наведена ли мышь, локальный или облачный
    //
    _painter->setPen(Ui::DesignSystem::color().onBackground());
    _painter->setFont(Ui::DesignSystem::font().iconsMid());
    const QRectF iconRect(backgroundRect.right() - Ui::DesignSystem::layout().px24() * 2,
                          backgroundRect.bottom() - Ui::DesignSystem::layout().px24() * 2,
                          Ui::DesignSystem::layout().px24() * 2,
                          Ui::DesignSystem::layout().px24() * 2);
    _painter->drawText(iconRect, Qt::AlignCenter, "\uf379");

    //
    // Декорация
    //
    if (m_decorationRadiusAnimation.state() == QVariantAnimation::Running
        || m_decorationOpacityAnimation.state() == QVariantAnimation::Running
        || QApplication::mouseButtons().testFlag(Qt::LeftButton)) {
        _painter->setClipRect(backgroundRect);
        _painter->setPen(Qt::NoPen);
        _painter->setBrush(Ui::DesignSystem::color().secondary());
        _painter->setOpacity(m_decorationOpacityAnimation.currentValue().toReal());
        _painter->drawEllipse(m_decorationCenterPosition, m_decorationRadiusAnimation.currentValue().toReal(),
                            m_decorationRadiusAnimation.currentValue().toReal());
    }
}

void ProjectCard::hoverEnterEvent(QGraphicsSceneHoverEvent* _event)
{
    QGraphicsRectItem::hoverEnterEvent(_event);
    m_shadowHeightAnimation.setDirection(QVariantAnimation::Forward);
    m_shadowHeightAnimation.start();
}

void ProjectCard::hoverLeaveEvent(QGraphicsSceneHoverEvent* _event)
{
    QGraphicsRectItem::hoverLeaveEvent(_event);
    m_shadowHeightAnimation.setDirection(QVariantAnimation::Backward);
    m_shadowHeightAnimation.start();
}

void ProjectCard::mousePressEvent(QGraphicsSceneMouseEvent* _event)
{
    m_decorationCenterPosition = _event->pos();
    m_decorationRadiusAnimation.setEndValue(rect().width());
    m_decorationOpacityAnimation.setCurrentTime(0);
    m_decorationRadiusAnimation.start();
    m_decorationOpacityAnimation.setStartValue(0.5);
    m_decorationOpacityAnimation.setEndValue(0.15);
    m_decorationOpacityAnimation.start();

    QGraphicsRectItem::mousePressEvent(_event);
}

void ProjectCard::mouseReleaseEvent(QGraphicsSceneMouseEvent* _event)
{
    m_decorationOpacityAnimation.setStartValue(0.15);
    m_decorationOpacityAnimation.setEndValue(0.0);
    m_decorationOpacityAnimation.start();

    QGraphicsRectItem::mouseReleaseEvent(_event);

    if (!rect().contains(_event->pos())) {
        return;
    }

    ProjectsScene* projectsScene = qobject_cast<ProjectsScene*>(scene());
    Q_ASSERT(projectsScene);
    emit projectsScene->projectPressed(m_project);
}

} // namespace


ProjectsScene::ProjectsScene(QObject* _parent)
    : QGraphicsScene(_parent)
{
}


// ****


class ProjectsCards::Implementation
{
public:
    explicit Implementation(QGraphicsView* _parent);

    /**
     * @brief Упорядочить карточки проектов
     */
    void reorderCards();

    /**
     * @brief Обновить область сцены, куда можно положить карточки
     */
    void updateSceneRect(const QRect& _rect);


    ProjectsScene* scene = nullptr;
    Domain::ProjectsModel* projects = nullptr;
};

ProjectsCards::Implementation::Implementation(QGraphicsView* _parent)
    : scene(new ProjectsScene(_parent))
{
}

void ProjectsCards::Implementation::reorderCards()
{

}

void ProjectsCards::Implementation::updateSceneRect(const QRect& _rect)
{
    scene->setSceneRect(_rect);
}


// ****


ProjectsCards::ProjectsCards(QWidget* _parent)
    : QGraphicsView(_parent),
      d(new Implementation(this))
{
    setFrameShape(QFrame::NoFrame);
    setScene(d->scene);
    d->scene->addItem(new ProjectCard);
}

void ProjectsCards::setBackgroundColor(const QColor& _color)
{
    scene()->setBackgroundBrush(_color);
}

void ProjectsCards::setProjects(Domain::ProjectsModel* _projects)
{
    if (d->projects == _projects) {
        return;
    }

    if (d->projects != nullptr) {
        d->projects->disconnect(this);
    }

    d->projects = _projects;

    if (d->projects == nullptr) {
        return;
    }

    connect(d->projects, &QAbstractListModel::rowsInserted, this,
            [this] (const QModelIndex& _parent, int _first, int _last)
    {
        Q_UNUSED(_parent);

        //
        // Ожидаем вставку только наверху
        //
        Q_ASSERT(_first == 0);

        //
        // Вставляем карточки
        //
        for (int row = _first; row <= _last; ++row) {
            //
            // Вставляется новая карточка слева вверху
            //
            auto projectCard = new ProjectCard;
            projectCard->setProject(d->projects->projectAt(_first));
            d->scene->addItem(projectCard);
            projectCard->setPos(-projectCard->rect().width(), -projectCard->rect().height());

            //
            // Корректируем расположение карточек в соответствии с новыми реалиями
            //
            d->reorderCards();

            //
            // Корректируем размер вьюпорта в зависимости от количества карточек
            //
            d->updateSceneRect(rect());
        }
    });
}

ProjectsCards::~ProjectsCards() = default;

void ProjectsCards::resizeEvent(QResizeEvent* _event)
{
    QGraphicsView::resizeEvent(_event);

    d->updateSceneRect(rect());
}

} // namespace Ui
