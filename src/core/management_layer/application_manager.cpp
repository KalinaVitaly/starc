#include "application_manager.h"

#include "content/account/account_manager.h"
#include "content/onboarding/onboarding_manager.h"
#include "content/projects/projects_manager.h"
#include "content/settings/settings_manager.h"

#ifdef CLOUD_SERVICE_MANAGER
#include <cloud/cloud_service_manager.h>
#endif

#include <include/custom_events.h>

#include <data_layer/storage/settings_storage.h>
#include <data_layer/storage/storage_facade.h>

#include <ui/application_style.h>
#include <ui/application_view.h>
#include <ui/menu_view.h>

#include <ui/design_system/design_system.h>
#include <ui/widgets/dialog/dialog.h>
#include <ui/widgets/dialog/standard_dialog.h>

#include <utils/3rd_party/WAF/Animation/Animation.h>

#include <QApplication>
#include <QDir>
#include <QFontDatabase>
#include <QLocale>
#include <QStyleFactory>
#include <QTimer>
#include <QTranslator>
#include <QVariant>
#include <QWidget>


namespace ManagementLayer {

class ApplicationManager::Implementation
{
public:
    explicit Implementation(ApplicationManager* _q);
    ~Implementation();

    /**
     * @brief Получить значение параметра настроек
     */
    QVariant settingsValue(const QString& _key) const;
    QVariantMap settingsValues(const QString& _key) const;

    /**
     * @brief Настроить параметры автосохранения
     */
    void configureAutoSave();

    /**
     * @brief Показать контент приложения
     */
    void showContent();

    /**
     * @brief Показать меню приложения
     */
    void showMenu();

    /**
     * @brief Показать личный кабинет
     */
    void showAccount();

    /**
     * @brief Показать страницу проектов
     */
    void showProjects();

    /**
     * @brief Показать страницу настроек
     */
    void showSettings();

    /**
     * @brief Показать предыдущий контент
     */
    void showLastContent();

    /**
     * @brief Установить перевод
     */
    void setTranslation(QLocale::Language _language);

    /**
     * @brief Установить тему
     */
    void setTheme(Ui::ApplicationTheme _theme);

    /**
     * @brief Установить цвета кастомной темы
     */
    void setCustomThemeColors(const Ui::DesignSystem::Color& _color);

    /**
     * @brief Установить коэффициент масштабирования
     */
    void setScaleFactor(qreal _scaleFactor);

    //
    // Работа с проектом
    //

    /**
     * @brief Настроить состояние приложения сохранены ли все изменения или нет
     */
    void markChangesSaved(bool _saved);

    /**
     * @brief Сохранить изменения проекта
     */
    void saveChanges();

    /**
     * @brief Если проект был изменён, но не сохранён предложить пользователю сохранить его
     * @param _callback - метод, который будет вызван, если пользователь хочет (Да),
     *        или не хочет (Нет) сохранять и не будет вызван, если пользователь передумал (Отмена)
     */
    void saveIfNeeded(std::function<void()> _callback);

    /**
     * @brief Создать проект
     */
    void createProject();
    void createLocalProject(const QString& _projectPath, const QString& _importFilePath);

    /**
     * @brief Закрыть текущий проект
     */
    void closeCurrentProject();

    //

    /**
     * @brief Выйти из приложения
     */
    void exit();


    //
    // Данные
    //

    ApplicationManager* q = nullptr;

    Ui::ApplicationView* applicationView = nullptr;
    Ui::MenuView* menuView = nullptr;
    struct LastContent {
        QWidget* toolBar = nullptr;
        QWidget* navigator = nullptr;
        QWidget* view = nullptr;
    } lastContent;

    QScopedPointer<AccountManager> accountManager;
    QScopedPointer<OnboardingManager> onboardingManager;
    QScopedPointer<ProjectsManager> projectsManager;
    QScopedPointer<SettingsManager> settingsManager;
#ifdef CLOUD_SERVICE_MANAGER
    QScopedPointer<CloudServiceManager> cloudServiceManager;
#endif

    QTimer autosaveTimer;

private:
    template<typename Manager>
    void saveLastContent(Manager* _manager);
};

ApplicationManager::Implementation::Implementation(ApplicationManager* _q)
    : q(_q),
      applicationView(new Ui::ApplicationView),
      menuView(new Ui::MenuView(applicationView)),
      accountManager(new AccountManager(nullptr, applicationView)),
      onboardingManager(new OnboardingManager(nullptr, applicationView)),
      projectsManager(new ProjectsManager(nullptr, applicationView)),
      settingsManager(new SettingsManager(nullptr, applicationView))
#ifdef CLOUD_SERVICE_MANAGER
    , cloudServiceManager(new CloudServiceManager)
#endif
{
    applicationView->setAccountBar(accountManager->accountBar());
}

ApplicationManager::Implementation::~Implementation()
{
    applicationView->disconnect();
    menuView->disconnect();
    accountManager->disconnect();
    onboardingManager->disconnect();
    projectsManager->disconnect();
#ifdef CLOUD_SERVICE_MANAGER
    cloudServiceManager->disconnect();
#endif
}

QVariant ApplicationManager::Implementation::settingsValue(const QString& _key) const
{
    return DataStorageLayer::StorageFacade::settingsStorage()->value(
                _key, DataStorageLayer::SettingsStorage::SettingsPlace::Application);
}

QVariantMap ApplicationManager::Implementation::settingsValues(const QString& _key) const
{
    return DataStorageLayer::StorageFacade::settingsStorage()->values(
                _key, DataStorageLayer::SettingsStorage::SettingsPlace::Application);
}

void ApplicationManager::Implementation::configureAutoSave()
{
    autosaveTimer.stop();
    autosaveTimer.disconnect();

    if (settingsValue(DataStorageLayer::kApplicationUseAutoSaveKey).toBool()) {
        QObject::connect(&autosaveTimer, &QTimer::timeout, [this] { saveChanges(); });
        autosaveTimer.start(std::chrono::minutes{3});
    }
}

void ApplicationManager::Implementation::showContent()
{
    //
    // Если это первый запуск приложения, то покажем онбординг
    //
    if (settingsValue(DataStorageLayer::kApplicationConfiguredKey).toBool() == false) {
        applicationView->showContent(onboardingManager->toolBar(),
                                     onboardingManager->navigator(),
                                     onboardingManager->view());
    }
    //
    // В противном случае показываем недавние проекты
    //
    else {
        //
        // Сперва проекты нужно загрузить
        //
        projectsManager->loadProjects();
        //
        // ... а затем уже отобразить
        //
        showProjects();

        //
        // Сохраняем последнее отображаемое представление, чтобы можно было вернуться к нему
        //
        saveLastContent(projectsManager.data());

#ifdef CLOUD_SERVICE_MANAGER
        //
        // Если менеджер облака доступен, отобржаем панель для работы с личным кабинетом
        //
        accountManager->accountBar()->show();
#endif
    }
}

void ApplicationManager::Implementation::showMenu()
{
    menuView->setFixedWidth(std::max(applicationView->navigationPanelWidth(),
                                     static_cast<int>(Ui::DesignSystem::drawer().width())));
    WAF::Animation::sideSlideIn(menuView);
}

void ApplicationManager::Implementation::showAccount()
{
    applicationView->showContent(accountManager->toolBar(),
                                 accountManager->navigator(),
                                 accountManager->view());
    accountManager->accountBar()->hide();
}

void ApplicationManager::Implementation::showProjects()
{
    applicationView->showContent(projectsManager->toolBar(),
                                 projectsManager->navigator(),
                                 projectsManager->view());
}

void ApplicationManager::Implementation::showSettings()
{
    applicationView->showContent(settingsManager->toolBar(),
                                 settingsManager->navigator(),
                                 settingsManager->view());
}

void ApplicationManager::Implementation::showLastContent()
{
    if (lastContent.toolBar == nullptr
        || lastContent.navigator == nullptr
        || lastContent.view == nullptr) {
        return;
    }

    applicationView->showContent(lastContent.toolBar, lastContent.navigator, lastContent.view);
}

void ApplicationManager::Implementation::setTranslation(QLocale::Language _language)
{
    //
    // Определим файл перевода
    //
    const QLocale::Language currentLanguage = _language != QLocale::AnyLanguage
                                        ? _language
                                        : QLocale::system().language();
    QString translation;
    switch (currentLanguage) {
        default:
        case QLocale::English: {
            translation = "en_EN";
            break;
        }

        case QLocale::Russian: {
            translation = "ru_RU";
            break;
        }
    }

    QLocale::setDefault(QLocale(currentLanguage));

    //
    // Подключим файл переводов программы
    //
    static QTranslator* appTranslator = [] {
        //
        // ... небольшой workaround для того, чтобы при запуске приложения кинуть событие о смене языка
        //
        QTranslator* translator = new QTranslator;
        QApplication::installTranslator(translator);
        return translator;
    } ();
    //
    QApplication::removeTranslator(appTranslator);
    appTranslator->load(":/translations/" + translation + ".qm");
    QApplication::installTranslator(appTranslator);

    //
    // Для языков, которые пишутся справа-налево настроим соответствующее выравнивание интерфейса
    //
    if (currentLanguage == QLocale::Persian
        || currentLanguage == QLocale::Hebrew) {
        QApplication::setLayoutDirection(Qt::RightToLeft);
    } else {
        QApplication::setLayoutDirection(Qt::LeftToRight);
    }
}

void ApplicationManager::Implementation::setTheme(Ui::ApplicationTheme _theme)
{
    Ui::DesignSystem::setTheme(_theme);
    QApplication::postEvent(q, new DesignSystemChangeEvent);
}

void ApplicationManager::Implementation::setCustomThemeColors(const Ui::DesignSystem::Color& _color)
{
    if (Ui::DesignSystem::theme() != Ui::ApplicationTheme::Custom) {
        return;
    }

    Ui::DesignSystem::setColor(_color);
    QApplication::postEvent(q, new DesignSystemChangeEvent);
}

void ApplicationManager::Implementation::setScaleFactor(qreal _scaleFactor)
{
    Ui::DesignSystem::setScaleFactor(_scaleFactor);
    QApplication::postEvent(q, new DesignSystemChangeEvent);
}

void ApplicationManager::Implementation::markChangesSaved(bool _saved)
{
    const QString suffix = QApplication::translate("ManagementLayer::ApplicationManager", " - changed");
    if (_saved
        && applicationView->windowTitle().endsWith(suffix)) {
        applicationView->setWindowTitle(applicationView->windowTitle().remove(suffix));
    } else if (!_saved
               && !applicationView->windowTitle().endsWith(suffix)) {
        applicationView->setWindowTitle(applicationView->windowTitle() + suffix);
    }

    applicationView->setWindowModified(!_saved);
}

void ApplicationManager::Implementation::saveChanges()
{
    //
    // TODO: Сохранение, все дела
    //

    markChangesSaved(true);
}

void ApplicationManager::Implementation::saveIfNeeded(std::function<void()> _callback)
{
    if (!applicationView->isWindowModified()) {
        _callback();
        return;
    }

    const int kCancelButtonId = 0;
    const int kNoButtonId = 1;
    const int kYesButtonId = 2;
    auto dialog = new Dialog(applicationView);
    dialog->showDialog({},
                       tr("Project was modified. Save changes?"),
                       {{ kCancelButtonId, tr("Cancel") },
                        { kNoButtonId, tr("Don't save") },
                        { kYesButtonId, tr("Save") }});
    QObject::connect(dialog, &Dialog::finished,
                     [this, dialog, _callback] (const Dialog::ButtonInfo& _buttonInfo)
    {
        dialog->hideDialog();

        //
        // Пользователь передумал сохранять
        //
        if (_buttonInfo.id == kCancelButtonId) {
            return;
        }

        //
        // Пользователь не хочет сохранять изменения
        //
        if (_buttonInfo.id == kNoButtonId) {
            markChangesSaved(true);
        }
        //
        // ... пользователь хочет сохранить изменения перед следующим действием
        //
        else {
            saveChanges();
        }

        _callback();
    });
    QObject::connect(dialog, &Dialog::disappeared, dialog, &Dialog::deleteLater);
}

void ApplicationManager::Implementation::createProject()
{
    saveIfNeeded(std::bind(&ProjectsManager::createProject, projectsManager.data()));
}

void ApplicationManager::Implementation::createLocalProject(const QString& _projectPath, const QString& _importFilePath)
{
    if (_projectPath.isEmpty()) {
        return;
    }

    //
    // Закроем текущий проект
    //
    closeCurrentProject();

    //
    // Папки, в которую пользователь хочет писать может и не быть,
    // создадим на всякий случай, чтобы его не мучать
    //
    QDir::root().mkpath(QFileInfo(_projectPath).absolutePath());

    //
    // Проверяем, можем ли мы писать в выбранный файл
    //
    QFile file(_projectPath);
    const bool canWrite = file.open(QIODevice::WriteOnly);
    file.close();

    //
    // Если невозможно записать в файл, предупреждаем пользователя и отваливаемся
    //
    if (!canWrite) {
        const QFileInfo fileInfo(_projectPath);
        QString errorMessage;
        if (!fileInfo.dir().exists()) {
            errorMessage =
                tr("You try to create a project in nonexistent folder %1. "
                   "Please, choose another location for the new project.")
                .arg(fileInfo.dir().absolutePath());
        } else if (fileInfo.exists()) {
            errorMessage =
                tr("The file can't be written. Looks like it is opened by another application. "
                   "Please close it and retry to create a new project.");
        } else {
            errorMessage =
                tr("The file can't be written. Please, check and give permissions to the app "
                   "to write into the selected folder, or choose another folder for saving a new project.");
        }
        StandardDialog::information(applicationView, tr("Create project error"), errorMessage);
        return;
    }

    //
    // Если возможна запись в файл
    //
    // ... создаём новую базу данных в файле и делаем её текущим проектом
    //
    projectsManager->setCurrentProject(_projectPath);
    //
    // ... перейдём к редактированию
    //
//    goToEditCurrentProject(_importFilePath);
}

void ApplicationManager::Implementation::closeCurrentProject()
{

}

void ApplicationManager::Implementation::exit()
{
    //
    // Сохраняем состояние приложения
    //
    DataStorageLayer::StorageFacade::settingsStorage()->setValues(
                DataStorageLayer::kApplicationViewStateKey,
                applicationView->saveState(),
                DataStorageLayer::SettingsStorage::SettingsPlace::Application);

    //
    // Выходим
    //
    QApplication::processEvents();
    QApplication::quit();
}

template<typename Manager>
void ApplicationManager::Implementation::saveLastContent(Manager* _manager)
{
    lastContent.toolBar = _manager->toolBar();
    lastContent.navigator = _manager->navigator();
    lastContent.view = _manager->view();
}


// ****


ApplicationManager::ApplicationManager(QObject* _parent)
    : QObject(_parent),
      IApplicationManager(),
      d(new Implementation(this))
{
    QApplication::setStyle(new ApplicationStyle(QStyleFactory::create("Fusion")));

    //
    // Загрузим шрифты в базу шрифтов программы, если их там ещё нет
    //
    QFontDatabase fontDatabase;
    fontDatabase.addApplicationFont(":/fonts/materialdesignicons");
    fontDatabase.addApplicationFont(":/fonts/roboto-black");
    fontDatabase.addApplicationFont(":/fonts/roboto-bold");
    fontDatabase.addApplicationFont(":/fonts/roboto-medium");
    fontDatabase.addApplicationFont(":/fonts/roboto-regular");
    fontDatabase.addApplicationFont(":/fonts/roboto-thin");

    initConnections();
}

ApplicationManager::~ApplicationManager() = default;

void ApplicationManager::exec(const QString& _fileToOpenPath)
{
    //
    // Установим размер экрана по-умолчанию, на случай, если это первый запуск
    //
    d->applicationView->resize(1024, 640);
    //
    // ... затем пробуем загрузить геометрию и состояние приложения
    //
    d->setTranslation(d->settingsValue(DataStorageLayer::kApplicationLanguagedKey).value<QLocale::Language>());
    d->setTheme(static_cast<Ui::ApplicationTheme>(d->settingsValue(DataStorageLayer::kApplicationThemeKey).toInt()));
    d->setCustomThemeColors(Ui::DesignSystem::Color(d->settingsValue(DataStorageLayer::kApplicationCustomThemeColorsKey).toString()));
    d->setScaleFactor(d->settingsValue(DataStorageLayer::kApplicationScaleFactorKey).toReal());
    d->applicationView->restoreState(d->settingsValues(DataStorageLayer::kApplicationViewStateKey));

    //
    // Покажем интерфейс
    //
    d->applicationView->show();

    //
    // Осуществляем остальную настройку и показываем содержимое, после того, как на экране
    // отобразится приложение, чтобы у пользователя возник эффект моментального запуска
    //
    QTimer::singleShot(0, this, [this, _fileToOpenPath] {
        //
        // Настройка
        //
        d->configureAutoSave();

        //
        // Отображение
        //
        d->showContent();

#ifdef CLOUD_SERVICE_MANAGER
        //
        // Запуск облачного сервиса
        //
        d->cloudServiceManager->start();
#endif

        //
        // Открыть заданный проект
        //
        openProject(_fileToOpenPath);
    });
}

void ApplicationManager::openProject(const QString& _path)
{
    if (_path.isEmpty()) {
        return;
    }

    if (!QFileInfo::exists(_path)) {
        d->projectsManager->hideProject(_path);
        return;
    }
}

bool ApplicationManager::event(QEvent* _event)
{
    switch (static_cast<int>(_event->type())) {
        case static_cast<QEvent::Type>(EventType::IdleEvent): {
            //
            // Сохраняем только если пользователь желает делать это автоматически
            //
            if (d->autosaveTimer.isActive()) {
                d->saveChanges();
            }

            _event->accept();
            return true;
        }

        case static_cast<QEvent::Type>(EventType::DesignSystemChangeEvent): {
            //
            // Уведомляем все виджеты о том, что сменилась дизайн система
            //
            for (auto widget : d->applicationView->findChildren<QWidget*>()) {
                QApplication::sendEvent(widget, _event);
            }
            QApplication::sendEvent(d->applicationView, _event);

            _event->accept();
            return true;
        }

        default: {
            return QObject::event(_event);
        }
    }
}

void ApplicationManager::initConnections()
{
    //
    // Представление приложения
    //
    connect(d->applicationView, &Ui::ApplicationView::closeRequested, this, [this]
    {
        d->saveIfNeeded(std::bind(&ApplicationManager::Implementation::exit, d.get()));
    });
    connect(d->menuView, &Ui::MenuView::projectsPressed, this, [this] { d->showProjects(); });
    connect(d->menuView, &Ui::MenuView::createProjectPressed, this, [this] { d->createProject(); });
    connect(d->menuView, &Ui::MenuView::settingsPressed, this, [this] { d->showSettings(); });

    //
    // Менеджер посадки
    //
    connect(d->onboardingManager.data(), &OnboardingManager::languageChanged, this,
            [this] (QLocale::Language _language) { d->setTranslation(_language); });
    connect(d->onboardingManager.data(), &OnboardingManager::themeChanged, this,
            [this] (Ui::ApplicationTheme _theme) { d->setTheme(_theme); });
    connect(d->onboardingManager.data(), &OnboardingManager::scaleFactorChanged, this,
            [this] (qreal _scaleFactor)
    {
        d->setScaleFactor(_scaleFactor);
        d->settingsManager->updateScaleFactor();
    });
    connect(d->onboardingManager.data(), &OnboardingManager::finished, this,
            [this]
    {
        auto setSettingsValue = [] (const QString& _key, const QVariant& _value) {
            DataStorageLayer::StorageFacade::settingsStorage()->setValue(
                        _key,
                        _value,
                        DataStorageLayer::SettingsStorage::SettingsPlace::Application);
        };
        setSettingsValue(DataStorageLayer::kApplicationConfiguredKey, true);
        setSettingsValue(DataStorageLayer::kApplicationLanguagedKey, QLocale::system().language());
        setSettingsValue(DataStorageLayer::kApplicationThemeKey, static_cast<int>(Ui::DesignSystem::theme()));
        setSettingsValue(DataStorageLayer::kApplicationScaleFactorKey, Ui::DesignSystem::scaleFactor());
        d->showContent();
    });

    //
    // Менеджер аккаунта
    //
    connect(d->accountManager.data(), &AccountManager::showAccountRequested, this, [this] { d->showAccount(); });
    connect(d->accountManager.data(), &AccountManager::closeAccountRequested, this, [this] {
        d->accountManager->accountBar()->show();
        d->showLastContent();
    });
    connect(d->accountManager.data(), &AccountManager::cloudProjectsCreationAvailabilityChanged,
            d->projectsManager.data(), &ProjectsManager::setProjectsInCloudCanBeCreated);

    //
    // Менеджер проектов
    //
    connect(d->projectsManager.data(), &ProjectsManager::menuRequested, this,
            [this] { d->showMenu(); });
    connect(d->projectsManager.data(), &ProjectsManager::createProjectRequested, this,
            [this] { d->createProject(); });
    connect(d->projectsManager.data(), &ProjectsManager::createLocalProjectRequested, this,
            [this] (const QString& _projectPath, const QString& _importFilePath)
    {
        d->createLocalProject(_projectPath, _importFilePath);
    });

    //
    // Менеджер настроек
    //
    connect(d->settingsManager.data(), &SettingsManager::closeSettingsRequested, this, [this] {
        d->showLastContent();
    });
    connect(d->settingsManager.data(), &SettingsManager::applicationLanguageChanged, this,
            [this] (QLocale::Language _language) { d->setTranslation(_language); });
    connect(d->settingsManager.data(), &SettingsManager::applicationThemeChanged, this,
            [this] (Ui::ApplicationTheme _theme)
    {
        d->setTheme(_theme);
        //
        // ... если применяется кастомная тема, то нужно загрузить её цвета
        //
        if (_theme == Ui::ApplicationTheme::Custom) {
            d->setCustomThemeColors(Ui::DesignSystem::Color(d->settingsValue(DataStorageLayer::kApplicationCustomThemeColorsKey).toString()));
        }
    });
    connect(d->settingsManager.data(), &SettingsManager::applicationCustomThemeColorsChanged, this,
            [this] (const Ui::DesignSystem::Color& _color) { d->setCustomThemeColors(_color); });
    connect(d->settingsManager.data(), &SettingsManager::applicationScaleFactorChanged, this,
            [this] (qreal _scaleFactor) { d->setScaleFactor(_scaleFactor); });
    connect(d->settingsManager.data(), &SettingsManager::applicationUseAutoSaveChanged, this,
            [this] { d->configureAutoSave(); });

#ifdef CLOUD_SERVICE_MANAGER
    //
    // Менеджер облака
    //
    // ... поймали/потеряли связь
    //
    connect(d->cloudServiceManager.data(), &CloudServiceManager::connected,
            d->accountManager.data(), &AccountManager::notifyConnected);
    connect(d->cloudServiceManager.data(), &CloudServiceManager::disconnected,
            d->accountManager.data(), &AccountManager::notifyDisconnected);
    //
    // ... авторизация/регистрация
    //
    {
        //
        // проверка регистрация или вход
        //
        connect(d->accountManager.data(), &AccountManager::emailEntered,
                d->cloudServiceManager.data(), &CloudServiceManager::canLogin);
        connect(d->cloudServiceManager.data(), &CloudServiceManager::registrationAllowed,
                d->accountManager.data(), &AccountManager::allowRegistration);
        connect(d->cloudServiceManager.data(), &CloudServiceManager::loginAllowed,
                d->accountManager.data(), &AccountManager::allowLogin);

        //
        // регистрация
        //
        connect(d->accountManager.data(), &AccountManager::registrationRequested,
                d->cloudServiceManager.data(), &CloudServiceManager::registerAccount);
        connect(d->accountManager.data(), &AccountManager::registrationConfirmationCodeEntered,
                d->cloudServiceManager.data(), &CloudServiceManager::confirmRegistration);
        connect(d->cloudServiceManager.data(), &CloudServiceManager::registrationConfiramtionCodeSended,
                d->accountManager.data(), &AccountManager::prepareToEnterRegistrationConfirmationCode);
        connect(d->cloudServiceManager.data(), &CloudServiceManager::registrationConfirmationError,
                d->accountManager.data(), &AccountManager::setRegistrationConfirmationError);
        connect(d->cloudServiceManager.data(), &CloudServiceManager::registrationCompleted,
                d->accountManager.data(), &AccountManager::login);

        //
        // восстановление пароля
        //
        connect(d->accountManager.data(), &AccountManager::restorePasswordRequested,
                d->cloudServiceManager.data(), &CloudServiceManager::restorePassword);
        connect(d->accountManager.data(), &AccountManager::passwordRestoringConfirmationCodeEntered,
                d->cloudServiceManager.data(), &CloudServiceManager::confirmPasswordRestoring);
        connect(d->accountManager.data(), &AccountManager::changePasswordRequested,
                d->cloudServiceManager.data(), &CloudServiceManager::changePassword);
        connect(d->cloudServiceManager.data(), &CloudServiceManager::passwordRestoringConfirmationCodeSended,
                d->accountManager.data(), &AccountManager::prepareToEnterRestorePasswordConfirmationCode);
        connect(d->cloudServiceManager.data(), &CloudServiceManager::passwordRestoringConfirmationSucceed,
                d->accountManager.data(), &AccountManager::allowChangePassword);
        connect(d->cloudServiceManager.data(), &CloudServiceManager::registrationConfirmationError,
                d->accountManager.data(), &AccountManager::setRestorePasswordConfirmationError);
        connect(d->cloudServiceManager.data(), &CloudServiceManager::passwordChanged,
                d->accountManager.data(), &AccountManager::login);

        //
        // авторизация
        //
        connect(d->accountManager.data(), &AccountManager::loginRequested,
                d->cloudServiceManager.data(), &CloudServiceManager::login);
        connect(d->cloudServiceManager.data(), &CloudServiceManager::loginPasswordError,
                d->accountManager.data(), &AccountManager::setLoginPasswordError);
        connect(d->cloudServiceManager.data(), &CloudServiceManager::loginCompleted,
                d->accountManager.data(), &AccountManager::completeLogin);

        //
        // Выход из аккаунта
        //
        connect(d->accountManager.data(), &AccountManager::logoutRequested,
                d->cloudServiceManager.data(), &CloudServiceManager::logout);
        connect(d->cloudServiceManager.data(), &CloudServiceManager::logoutCompleted,
                d->accountManager.data(), &AccountManager::completeLogout);
    }

    //
    // Получены параметры об аккаунте
    //
    connect(d->cloudServiceManager.data(), &CloudServiceManager::accountParametersLoaded,
            d->accountManager.data(), &AccountManager::setAccountParameters);
    connect(d->cloudServiceManager.data(), &CloudServiceManager::paymentInfoLoaded,
            d->accountManager.data(), &AccountManager::setPaymentInfo);

    //
    // Оплата услуг
    //
    connect(d->accountManager.data(), &AccountManager::renewSubscriptionRequested,
            d->cloudServiceManager.data(), &CloudServiceManager::renewSubscription);
    connect(d->cloudServiceManager.data(), &CloudServiceManager::subscriptionRenewed,
            d->accountManager.data(), &AccountManager::setSubscriptionEnd);

    //
    // Изменение параметров аккаунта
    //
    connect(d->accountManager.data(), &AccountManager::changeUserNameRequested,
            d->cloudServiceManager.data(), &CloudServiceManager::setUserName);
    connect(d->accountManager.data(), &AccountManager::changeReceiveEmailNotificationsRequested,
            d->cloudServiceManager.data(), &CloudServiceManager::setNeedNotify);
    connect(d->accountManager.data(), &AccountManager::changeAvatarRequested,
            d->cloudServiceManager.data(), &CloudServiceManager::setAvatar);
    connect(d->cloudServiceManager.data(), &CloudServiceManager::userNameChanged,
            d->accountManager.data(), &AccountManager::setUserName);
    connect(d->cloudServiceManager.data(), &CloudServiceManager::receiveEmailNotificationsChanged,
            d->accountManager.data(), &AccountManager::setReceiveEmailNotifications);
    connect(d->cloudServiceManager.data(), &CloudServiceManager::avatarChanged,
            d->accountManager.data(), qOverload<const QByteArray&>(&AccountManager::setAvatar));
#endif
}

} // namespace ManagementLayer
