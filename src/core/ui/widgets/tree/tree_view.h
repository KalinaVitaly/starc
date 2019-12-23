#pragma once

#include <QTreeView>


/**
 * @brief Докрученный виджет дерева с эффектом клика на элементе
 */
class TreeView : public QTreeView
{
    Q_OBJECT

public:
    explicit TreeView(QWidget* _parent = nullptr);
    ~TreeView() override;

    /**
     * @brief Загрузить состояние дерева
     */
    void restoreState(const QVariant& _state);

    /**
     * @brief Сохранить состояние дерева
     */
    QVariant saveState() const;

protected:
    /**
     * @brief Фильтруем события клика вьюпорта, чтобы анимировать их
     */
    bool eventFilter(QObject* _watched, QEvent* _event) override;

    /**
     * @brief Декорируем вьюпорт после нажатия клавиши
     */
    void paintEvent(QPaintEvent* _event) override;

    /**
     * @brief Переопределяем, чтобы раскрывать элемент в который кидаются данные, если элемент свёрнут
     */
    void dropEvent(QDropEvent* _event) override;

private:
    class Implementation;
    QScopedPointer<Implementation> d;
};
