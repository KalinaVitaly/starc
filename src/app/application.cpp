#include "application.h"

#include <include/custom_events.h>

#include <interfaces/management_layer/i_application_manager.h>

#include <QDebug>
#include <QFileOpenEvent>
#include <QTimer>


namespace {
/**
 * @brief Подготовить путь к файлу для сохранения
 */
static QString preparePath(const QString& _path) {
    QString newPath = _path;
#ifdef Q_OS_MAC
    //
    // Это две разные буквы!
    // Первую даёт нам мак, когда открываешь файл через двойной щелчок на нём
    //
    newPath.replace("й", "й");
#endif
    return newPath;
}
}

class Application::Implementation
{
public:
    QObject* applicationManager = nullptr;

    QString fileToOpen;
    QTimer idleTimer;
};


// **


Application::Application(int& _argc, char** _argv)
    : QApplication(_argc, _argv),
      d(new Implementation)
{
    setApplicationName("Starc");
    setOrganizationName("Story Architect");
    setOrganizationDomain("starc.app");

    //
    // Настроим таймер определения простоя приложения
    //
    d->idleTimer.setInterval(std::chrono::seconds{3});
    connect(&d->idleTimer, &QTimer::timeout, [=] {
        if (d->applicationManager != nullptr) {
            postEvent(d->applicationManager, new IdleEvent);
        }
    });
    d->idleTimer.start();
}

void Application::setApplicationManager(QObject* _manager)
{
    d->applicationManager = _manager;
}

void Application::startUp()
{
    auto manager = qobject_cast<ManagementLayer::IApplicationManager*>(d->applicationManager);
    if (manager == nullptr) {
        qCritical() << "Can start application without application manager";
        exit(1);
    }

    //
    // Получим имя файла, который пользователь возможно хочет открыть
    //
    if (d->fileToOpen.isEmpty()
        && arguments().size() > 1) {
        d->fileToOpen = arguments().first();
    }

    manager->exec(d->fileToOpen);
}

Application::~Application() = default;

bool Application::notify(QObject* _object, QEvent* _event)
{
    //
    // Работа с таймером определяющим простой приложения
    //
    if (_event != nullptr
        && (_event->type() == QEvent::MouseMove
            || _event->type() == QEvent::MouseButtonPress
            || _event->type() == QEvent::MouseButtonDblClick
            || _event->type() == QEvent::KeyPress
            || _event->type() == QEvent::InputMethod
            || _event->type() == QEvent::Wheel
            || _event->type() == QEvent::Gesture)) {
        d->idleTimer.start();
    }

    return QApplication::notify(_object, _event);
}

bool Application::event(QEvent* _event)
{
    if (_event->type() == QEvent::FileOpen) {
        QFileOpenEvent* fileOpenEvent = static_cast<QFileOpenEvent*>(_event);
        if (auto manager = qobject_cast<ManagementLayer::IApplicationManager*>(d->applicationManager)) {
            manager->openProject(preparePath(fileOpenEvent->file()));
        } else {
            d->fileToOpen = preparePath(fileOpenEvent->file());
        }

        return true;
    }

    return QApplication::event(_event);
}
