#include "database.h"

#include <QApplication>
#include <QDateTime>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>

namespace DatabaseLayer
{

namespace {
    /**
     * @brief Получить ключ хранения номера версии приложения
     */
    static QString applicationVersionKey() {
        return
#ifdef MOBILE_OS
                "application-version-mobile";
#else
                "application-version";
#endif
    }
    /**
     * @brief Инвертированный ключ хранения номера версии, для проверок
     */
    static QString invertedApplicationVersionKey() {
        return
#ifdef MOBILE_OS
                "application-version";
#else
                "application-version-mobile";
#endif
    }
}


bool Database::canOpenFile(const QString& _databaseFileName)
{
    bool canOpen = true;

    {
        QSqlDatabase database = QSqlDatabase::addDatabase(s_sqlDriver, "tmp_database");
        database.setDatabaseName(_databaseFileName);
        database.open();

        QSqlQuery q_checker(database);

        //
        // 1. Если файл был создан в более поздней версии приложения, его нельзя открывать
        //
        if (q_checker.exec("SELECT value FROM system_variables WHERE variable = 'application-version' ")
            && q_checker.next()
            && q_checker.value("value").toString().split(" ").first() > QApplication::applicationVersion()) {
            canOpen = false;
            s_openFileError =
                    QApplication::translate("DatabaseLayer::Database",
                        "Project was modified in higher version. You need update application to latest version for open it.");
        }
    }

    QSqlDatabase::removeDatabase("tmp_database");

    return canOpen;
}

QString Database::openFileError()
{
    return s_openFileError;
}

bool Database::hasError()
{
    return !s_lastError.isEmpty();
}

QString Database::lastError()
{
    return s_lastError;
}

void Database::setLastError(const QString& _error)
{
    if (s_lastError != _error)   {
        s_lastError = _error;
    }
}

void Database::setCurrentFile(const QString& _databaseFileName)
{
    //
    // Если использовалась база данных, то удалим старое соединение
    //
    closeCurrentFile();

    //
    // Установим текущее имя базы данных
    //
    if (s_databaseName != _databaseFileName) {
        s_databaseName = _databaseFileName;
        s_connectionName = "local_database [" + s_databaseName + "]";
    }

    //
    // Откроем базу данных, или создадим новую
    //
    instanse();
}

void Database::closeCurrentFile()
{
    if (QSqlDatabase::contains(s_connectionName)) {
        QSqlDatabase::removeDatabase(s_connectionName);
    }
}

QString Database::currentFile()
{
    return instanse().databaseName();
}

QSqlQuery Database::query()
{
    return QSqlQuery(instanse());
}

void Database::transaction()
{
    //
    // Для первого запроса открываем транзакцию
    //
    if (s_openedTransactions == 0) {
        instanse().transaction();
    }

    //
    // Увеличиваем счётчик открытых транзакций
    //
    ++s_openedTransactions;
}

void Database::commit()
{
    //
    // Уменьшаем счётчик транзакций
    //
    --s_openedTransactions;

    //
    // При закрытии корневой транзакции фиксируем изменения в базе данных
    //
    if (s_openedTransactions == 0) {
        instanse().commit();
    }
}


// ****


QString Database::s_connectionName = "local_database";
QString Database::s_sqlDriver = "QSQLITE";
QString Database::s_databaseName = ":memory:";

QString Database::s_openFileError = QString();
QString Database::s_lastError = QString();
int Database::s_openedTransactions = 0;

QSqlDatabase Database::instanse()
{
    QSqlDatabase database;

    if (!QSqlDatabase::contains(s_connectionName)) {
        open(database, s_connectionName, s_databaseName);
    } else {
        database = QSqlDatabase::database(s_connectionName);
    }

    return database;
}

void Database::open(QSqlDatabase& _database, const QString& _connectionName, const QString& _databaseName)
{
    s_lastError.clear();

    _database = QSqlDatabase::addDatabase(s_sqlDriver, _connectionName);
    _database.setDatabaseName(_databaseName);
    _database.open();

    Database::States states = checkState(_database);

    if (!states.testFlag(SchemeFlag))
        createTables(_database);
    if (!states.testFlag(IndexesFlag))
        createIndexes(_database);
    if (!states.testFlag(EnumsFlag))
        createEnums(_database);
    if (states.testFlag(OldVersionFlag))
        updateDatabase(_database);
}

// Проверка состояния базы данных
// например:
// - БД отсутствует
// - БД пуста
// - БД имеет старую версию
// - БД имеет последнюю версию
// - и т.д.
Database::States Database::checkState(QSqlDatabase& _database)
{
    QSqlQuery q_checker(_database);
    Database::States states = Database::EmptyFlag;

    //
    // Созданы ли таблицы
    //
    if (q_checker.exec("SELECT COUNT(*) as size FROM sqlite_master WHERE type = 'table' ") &&
        q_checker.next() &&
        q_checker.record().value("size").toInt()) {
        //
        // Все остальные проверки имеют смысл, только если проходит данная проверка
        //
        states = states | Database::SchemeFlag;

        //
        // Созданы ли индексы
        //
        if (q_checker.exec("SELECT COUNT(*) as size FROM sqlite_master WHERE type = 'index' ") &&
            q_checker.next() &&
            q_checker.record().value("size").toInt()) {
            states = states | Database::IndexesFlag;
        }

        //
        // Проверка версии
        //
        if (q_checker.exec(
                QString("SELECT value as version FROM system_variables WHERE variable = '%1' ")
                .arg(applicationVersionKey()))) {
            //
            // Если версия не задана, или она не соответствует текущей, то надо обновить файл
            //
            if (!q_checker.next()
                || q_checker.record().value("version").toString() != QApplication::applicationVersion()) {
                states = states | Database::OldVersionFlag;
            }
        }
    }

    return states;
}

void Database::createTables(QSqlDatabase& _database)
{
    QSqlQuery query(_database);
    _database.transaction();

    //
    // Таблица системных переменных
    //
    query.exec("CREATE TABLE system_variables "
               "( "
               "variable TEXT PRIMARY KEY ON CONFLICT REPLACE, "
               "value TEXT NOT NULL "
               "); "
               );

    //
    // Таблица с документами
    //
    query.exec("CREATE TABLE items "
               "("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "parent_id INTEGER DEFAULT(NULL), "
               "type INTEGER NOT NULL DEFAULT(0), "
               "name TEXT DEFAULT(NULL), "
               "content TEXT DEFAULT(NULL), "
               "image BLOB DEFAULT(NULL), "
               "color TEXT DEFAULT(NULL), "
               "sort_order INTEGER NOT NULL DEFAULT(0), "
               "is_deleted INTEGER NOT NULL DEFAULT(0) "
               ")"
               );

    //
    // Таблица с изменениями документов
    //
    query.exec("CREATE TABLE items_changes "
               "("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "fk_item_id INTEGER NOT NULL, "
               "uuid TEXT NOT NULL, "
               "undo_patch BLOB NOT NULL, " // отмена изменения
               "redo_patch BLOB NOT NULL, " // повтор изменения
               "date_time TEXT NOT NULL, " // yyyy.mm.dd.hh.mm.ss.zzz
               "user_email TEXT DEFAULT(NULL), "
               "user_name TEXT NOT NULL "
               ")"
               );

    //
    // Таблица с версиями документов
    //
    query.exec("CREATE TABLE items_versions "
               "( "
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "fk_item_id INTEGER NOT NULL, "
               "color TEXT DEFAULT(NULL), "
               "name TEXT UNIQUE NOT NULL, "
               "description TEXT DEFAULT(NULL), "
               "item_content TEXT NOT NULL"
               "date_time TEXT NOT NULL, " // yyyy.mm.dd.hh.mm.ss.zzz
               "user_email TEXT DEFAULT(NULL), "
               "user_name TEXT NOT NULL "
               "); "
               );

    _database.commit();
}

void Database::createIndexes(QSqlDatabase& _database)
{
    Q_UNUSED(_database);
}

void Database::createEnums(QSqlDatabase& _database)
{
    Q_UNUSED(_database);
}

void Database::updateDatabase(QSqlDatabase& _database)
{
    QSqlQuery query(_database);

    //
    // Определим версию базы данных
    //
    query.prepare("SELECT value as version FROM system_variables WHERE variable = ? ");
    query.addBindValue(applicationVersionKey());
    query.exec();
    query.next();
    QString databaseVersion = query.record().value("version").toString();
    //
    // ... если версии нет (файл пришёл из другой версии мобильная <-> десктоп), то создадим её
    //
    if (databaseVersion.isEmpty()) {
        query.addBindValue(invertedApplicationVersionKey());
        query.exec();
        query.next();
        databaseVersion = query.record().value("version").toString();
    }

    //
    // Некоторые версии выходили с ошибками, их заменяем на предыдущие
    //
    {
//        if (databaseVersion.startsWith("X.X.X beta")) {
//            databaseVersion = "Y.Y.Y";
//        }
    }
    const QStringList& versionParts = databaseVersion.split(".");
    const int versionMajor = versionParts.value(0, "0").toInt();
    const int versionMinor = versionParts.value(1, "0").toInt();
    const int versionBuild = versionParts.value(2, "1").split(" ").value(0, "1").toInt();

    //
    // Вызываются необходимые процедуры обновления БД в зависимости от её версии
    //
    // 0.X.X
    //
    if (versionMajor <= 0) {
        //
        // 0.0.X
        //
        if (versionMinor <= 0) {
            if (versionMinor < 0
                || versionBuild <= 1) {
//                updateDatabaseTo_0_0_2(_database);
            }
            if (versionMinor < 0
                || versionBuild <= 4) {
//                updateDatabaseTo_0_0_5(_database);
            }
        }
        //
        // 0.1.X
        //
        if (versionMinor <= 1) {
            if (versionMinor < 1
                || versionBuild <= 0) {
//                updateDatabaseTo_0_1_0(_database);
            }
        }
    }

    //
    // Сохраняем информацию об обновлении версии
    //
    query.exec(
        QString("INSERT INTO system_variables VALUES ('application-updated-to-version-%1', '%2')")
               .arg(QApplication::applicationVersion())
               .arg(QDateTime::currentDateTime().toString(Qt::ISODate))
        );

    //
    // Обновляется версия программы
    //
    query.exec(
        QString("INSERT INTO system_variables VALUES ('%1', '%2')")
                .arg(applicationVersionKey())
                .arg(QApplication::applicationVersion())
        );
}

} // namespace DatabaseLayer
