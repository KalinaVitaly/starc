#pragma once

#include <ui/widgets/widget/widget.h>

class QAbstractItemModel;


namespace Ui
{

/**
 * @brief Виджет комментариев сценария
 */
class ScreenplayTextCommentsWidget : public Widget
{
    Q_OBJECT

public:
    explicit ScreenplayTextCommentsWidget(QWidget* _parent = nullptr);
    ~ScreenplayTextCommentsWidget() override;

private:
    class Implementation;
    QScopedPointer<Implementation> d;
};

} // namespace Ui

