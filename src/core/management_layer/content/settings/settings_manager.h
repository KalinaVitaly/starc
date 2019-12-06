#pragma once

#include <QLocale>
#include <QObject>

namespace Ui {
    enum class ApplicationTheme;
}


namespace ManagementLayer
{

class SettingsManager : public QObject
{
    Q_OBJECT

public:
    explicit SettingsManager(QObject* _parent, QWidget* _parentWidget);
    ~SettingsManager() override;

    QWidget* toolBar() const;
    QWidget* navigator() const;
    QWidget* view() const;

    /**
     * @brief Обновить используемый язык в представлении
     */
    void updateLanguage();

    /**
     * @brief Обновить используемую тему в представлении
     */
    void updateTheme();

    /**
     * @brief Обновить используемый коэффициент масштабирования в представлении
     */
    void updateScaleFactor();

signals:
    /**
     * @brief Пользователь хочет закрыть настройки
     */
    void closeSettingsRequested();

    /**
     * @brief Пользователь изменил язык
     */
    void languageChanged(QLocale::Language _language);

    /**
     * @brief Пользователь выбрал тему приложения
     */
    void themeChanged(Ui::ApplicationTheme _theme);

    /**
     * @brief Пользователь изменил масштаб интерфейса
     */
    void scaleFactorChanged(qreal _scaleFactor);

private:
    //
    // Сохранение параметров приложения
    //
    void setApplicationLanguage(int _language);
    void setApplicationUseSpellChecker(bool _use);
    void setApplicationSpellCheckerLanguage(const QString& _languageCode);
    void setApplicationTheme(int _theme);
    void setApplicationScaleFactor(qreal _scaleFactor);
    void setApplicationUseAutoSave(bool _use);
    void setApplicationSaveBackups(bool _save);
    void setApplicationBackupsFolder(const QString& _path);

private:
    class Implementation;
    QScopedPointer<Implementation> d;
};

} // namespace ManagementLayer
