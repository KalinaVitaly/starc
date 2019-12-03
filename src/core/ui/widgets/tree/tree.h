#pragma once

#include <ui/widgets/widget/widget.h>

class QAbstractItemModel;


/**
 * @brief Виджет дерева элементов
 */
class Tree : public Widget
{
    Q_OBJECT

public:
    explicit Tree(QWidget* _parent = nullptr);
    ~Tree() override;

    void setModel(QAbstractItemModel* _model);

protected:
    /**
     * @brief Корректируем внешний вид виджета дерева и его делегата
     */
    void designSystemChangeEvent(DesignSystemChangeEvent* _event) override;

private:
    class Implementation;
    QScopedPointer<Implementation> d;
};
