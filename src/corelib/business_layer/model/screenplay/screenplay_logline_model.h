#pragma once

#include "../abstract_model.h"


namespace BusinessLayer
{

/**
 * @brief Модель корзины с удалёнными документами
 */
class ScreenplayLoglineModel : public AbstractModel
{
    Q_OBJECT

public:
    explicit ScreenplayLoglineModel(QObject* _parent = nullptr);

protected:
    /**
     * @brief Реализация модели для работы с документами
     */
    /** @{ */
    void initDocument() override;
    void clearDocument() override;
    QByteArray toXml() const override;
    /** @} */
};

} // namespace BusinessLayer
