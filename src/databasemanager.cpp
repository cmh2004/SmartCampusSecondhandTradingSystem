#include "databasemanager.h"
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QCryptographicHash>
#include <QDateTime>

DatabaseManager* DatabaseManager::m_instance = nullptr;

DatabaseManager::DatabaseManager(QObject* parent)
    : QObject(parent)
{
}

DatabaseManager::~DatabaseManager()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
}

DatabaseManager* DatabaseManager::instance()
{
    if (!m_instance) {
        m_instance = new DatabaseManager();
    }
    return m_instance;
}

bool DatabaseManager::initialize(const QString& databasePath)
{
    QMutexLocker locker(&m_mutex);

    QString path = databasePath;
    if (path.isEmpty()) {
        QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir dir(dataDir);
        if (!dir.exists()) {
            dir.mkpath(".");
        }
        path = dataDir + "/local_cache.db";
    }

    m_databasePath = path;

    m_database = QSqlDatabase::addDatabase("QSQLITE", "local_cache_connection");
    m_database.setDatabaseName(path);

    if (!m_database.open()) {
        qWarning() << "Failed to open database:" << m_database.lastError().text();
        return false;
    }

    return createTables();
}

bool DatabaseManager::createTables()
{
    QSqlQuery query(m_database);

    // 用户信息表
    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS users (
            user_id TEXT PRIMARY KEY,
            username TEXT NOT NULL,
            nickname TEXT,
            email TEXT,
            phone TEXT,
            avatar_url TEXT,
            credit_score INTEGER DEFAULT 100,
            role TEXT DEFAULT 'user',
            created_at INTEGER,
            updated_at INTEGER
        )
    )")) {
        qWarning() << "Failed to create users table:" << query.lastError().text();
        return false;
    }

    // 登录信息表
    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS login_info (
            username TEXT PRIMARY KEY,
            password_hash TEXT,
            remember_me INTEGER DEFAULT 0,
            last_login INTEGER
        )
    )")) {
        qWarning() << "Failed to create login_info table:" << query.lastError().text();
        return false;
    }

    // 商品缓存表
    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS goods_cache (
            goods_id INTEGER PRIMARY KEY,
            title TEXT NOT NULL,
            price REAL NOT NULL,
            category TEXT,
            description TEXT,
            seller_id TEXT,
            seller_name TEXT,
            images TEXT,
            status TEXT DEFAULT 'onsale',
            condition TEXT,
            location TEXT,
            publish_time INTEGER,
            view_count INTEGER DEFAULT 0,
            cache_time INTEGER
        )
    )")) {
        qWarning() << "Failed to create goods_cache table:" << query.lastError().text();
        return false;
    }

    // 订单缓存表
    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS orders_cache (
            order_id INTEGER PRIMARY KEY,
            user_id TEXT NOT NULL,
            goods_id INTEGER,
            goods_title TEXT,
            seller_id TEXT,
            seller_name TEXT,
            buyer_id TEXT,
            buyer_name TEXT,
            amount REAL,
            status TEXT,
            payment_method TEXT,
            payment_time INTEGER,
            delivery_info TEXT,
            created_at INTEGER,
            updated_at INTEGER,
            cache_time INTEGER
        )
    )")) {
        qWarning() << "Failed to create orders_cache table:" << query.lastError().text();
        return false;
    }

    // 消息缓存表
    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS messages_cache (
            message_id TEXT PRIMARY KEY,
            chat_id TEXT NOT NULL,
            sender_id TEXT NOT NULL,
            sender_name TEXT,
            receiver_id TEXT NOT NULL,
            content TEXT,
            message_type TEXT DEFAULT 'text',
            attachments TEXT,
            timestamp INTEGER,
            read_status INTEGER DEFAULT 0
        )
    )")) {
        qWarning() << "Failed to create messages_cache table:" << query.lastError().text();
        return false;
    }

    // 收藏表
    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS favorites (
            user_id TEXT NOT NULL,
            goods_id INTEGER NOT NULL,
            added_time INTEGER,
            PRIMARY KEY (user_id, goods_id)
        )
    )")) {
        qWarning() << "Failed to create favorites table:" << query.lastError().text();
        return false;
    }

    // 搜索历史表
    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS search_history (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id TEXT NOT NULL,
            keyword TEXT NOT NULL,
            search_time INTEGER,
            result_count INTEGER
        )
    )")) {
        qWarning() << "Failed to create search_history table:" << query.lastError().text();
        return false;
    }

    // 离线操作队列
    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS offline_operations (
            operation_id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id TEXT NOT NULL,
            operation_type TEXT NOT NULL,
            operation_data TEXT NOT NULL,
            created_time INTEGER,
            retry_count INTEGER DEFAULT 0,
            last_retry_time INTEGER
        )
    )")) {
        qWarning() << "Failed to create offline_operations table:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::saveUserInfo(const QJsonObject& userInfo)
{
    QMutexLocker locker(&m_mutex);

    QSqlQuery query(m_database);
    query.prepare(R"(
        INSERT OR REPLACE INTO users
        (user_id, username, nickname, email, phone, avatar_url, credit_score, role, created_at, updated_at)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    )");

    query.addBindValue(userInfo.value("user_id").toString());
    query.addBindValue(userInfo.value("username").toString());
    query.addBindValue(userInfo.value("nickname").toString());
    query.addBindValue(userInfo.value("email").toString());
    query.addBindValue(userInfo.value("phone").toString());
    query.addBindValue(userInfo.value("avatar_url").toString());
    query.addBindValue(userInfo.value("credit_score").toInt(100));
    query.addBindValue(userInfo.value("role").toString("user"));
    query.addBindValue(userInfo.value("created_at").toVariant().toLongLong());
    query.addBindValue(QDateTime::currentSecsSinceEpoch());

    return query.exec();
}

QJsonObject DatabaseManager::getUserInfo(const QString& userId)
{
    QMutexLocker locker(&m_mutex);

    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM users WHERE user_id = ?");
    query.addBindValue(userId);

    if (!query.exec() || !query.next()) {
        return QJsonObject();
    }

    QJsonObject userInfo;
    QSqlRecord record = query.record();
    for (int i = 0; i < record.count(); i++) {
        QString fieldName = record.fieldName(i);
        QVariant value = query.value(i);

        if (value.typeId() == QMetaType::QString) {
            userInfo[fieldName] = value.toString();
        } else if (value.typeId() == QMetaType::Int || value.typeId() == QMetaType::LongLong) {
            userInfo[fieldName] = value.toInt();
        } else if (value.typeId() == QMetaType::Double) {
            userInfo[fieldName] = value.toDouble();
        }
    }

    return userInfo;
}

bool DatabaseManager::updateUserInfo(const QString& userId, const QJsonObject& updates)
{
    QMutexLocker locker(&m_mutex);

    QStringList updateFields;
    QList<QVariant> values;

    for (auto it = updates.begin(); it != updates.end(); ++it) {
        updateFields.append(it.key() + " = ?");
        values.append(it.value().toVariant());
    }

    if (updateFields.isEmpty()) {
        return false;
    }

    updateFields.append("updated_at = ?");
    values.append(QDateTime::currentSecsSinceEpoch());

    QSqlQuery query(m_database);
    QString sql = QString("UPDATE users SET %1 WHERE user_id = ?").arg(updateFields.join(", "));
    query.prepare(sql);

    for (const QVariant& value : values) {
        query.addBindValue(value);
    }
    query.addBindValue(userId);

    return query.exec();
}

bool DatabaseManager::saveLoginInfo(const QString& username, const QString& passwordHash)
{
    QMutexLocker locker(&m_mutex);

    QSqlQuery query(m_database);
    query.prepare(R"(
        INSERT OR REPLACE INTO login_info
        (username, password_hash, last_login)
        VALUES (?, ?, ?)
    )");

    query.addBindValue(username);
    query.addBindValue(passwordHash);
    query.addBindValue(QDateTime::currentSecsSinceEpoch());

    return query.exec();
}

QJsonObject DatabaseManager::getLoginInfo()
{
    QMutexLocker locker(&m_mutex);

    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM login_info ORDER BY last_login DESC LIMIT 1");

    if (!query.exec() || !query.next()) {
        return QJsonObject();
    }

    QJsonObject loginInfo;
    loginInfo["username"] = query.value("username").toString();
    loginInfo["password_hash"] = query.value("password_hash").toString();

    return loginInfo;
}

bool DatabaseManager::cacheGoodsList(const QJsonArray& goodsList)
{
    QMutexLocker locker(&m_mutex);

    QSqlQuery query(m_database);

    // 开始事务
    if (!m_database.transaction()) {
        return false;
    }

    qint64 cacheTime = QDateTime::currentSecsSinceEpoch();

    for (const QJsonValue& goodsValue : goodsList) {
        QJsonObject goods = goodsValue.toObject();

        query.prepare(R"(
            INSERT OR REPLACE INTO goods_cache
            (goods_id, title, price, category, description, seller_id, seller_name,
             images, status, condition, location, publish_time, view_count, cache_time)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
        )");

        query.addBindValue(goods.value("goods_id").toInt());
        query.addBindValue(goods.value("title").toString());
        query.addBindValue(goods.value("price").toDouble());
        query.addBindValue(goods.value("category").toString());
        query.addBindValue(goods.value("description").toString());
        query.addBindValue(goods.value("seller_id").toString());
        query.addBindValue(goods.value("seller_name").toString());

        // 将图片数组转为JSON字符串
        QJsonDocument imagesDoc(goods.value("images").toArray());
        query.addBindValue(imagesDoc.toJson(QJsonDocument::Compact));

        query.addBindValue(goods.value("status").toString("onsale"));
        query.addBindValue(goods.value("condition").toString());
        query.addBindValue(goods.value("location").toString());
        query.addBindValue(goods.value("publish_time").toVariant().toLongLong());
        query.addBindValue(goods.value("view_count").toInt(0));
        query.addBindValue(cacheTime);

        if (!query.exec()) {
            m_database.rollback();
            return false;
        }
    }

    return m_database.commit();
}

QJsonArray DatabaseManager::getCachedGoodsList(int page, int pageSize)
{
    QMutexLocker locker(&m_mutex);

    int offset = (page - 1) * pageSize;

    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM goods_cache WHERE status = 'onsale' "
                  "ORDER BY cache_time DESC LIMIT ? OFFSET ?");
    query.addBindValue(pageSize);
    query.addBindValue(offset);

    QJsonArray goodsList;

    if (!query.exec()) {
        return goodsList;
    }

    while (query.next()) {
        QJsonObject goods;
        QSqlRecord record = query.record();

        for (int i = 0; i < record.count(); i++) {
            QString fieldName = record.fieldName(i);
            QVariant value = query.value(i);

            if (fieldName == "images") {
                // 解析图片JSON
                QJsonDocument doc = QJsonDocument::fromJson(value.toByteArray());
                goods[fieldName] = doc.array();
            } else if (value.typeId() == QMetaType::QString) {
                goods[fieldName] = value.toString();
            } else if (value.typeId() == QMetaType::Int || value.typeId() == QMetaType::LongLong) {
                goods[fieldName] = value.toInt();
            } else if (value.typeId() == QMetaType::Double) {
                goods[fieldName] = value.toDouble();
            }
        }

        goodsList.append(goods);
    }

    return goodsList;
}

QJsonObject DatabaseManager::getCachedGoodsDetail(int goodsId)
{
    QMutexLocker locker(&m_mutex);

    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM goods_cache WHERE goods_id = ?");
    query.addBindValue(goodsId);

    if (!query.exec() || !query.next()) {
        return QJsonObject();
    }

    QJsonObject goods;
    QSqlRecord record = query.record();

    for (int i = 0; i < record.count(); i++) {
        QString fieldName = record.fieldName(i);
        QVariant value = query.value(i);

        if (fieldName == "images") {
            QJsonDocument doc = QJsonDocument::fromJson(value.toByteArray());
            goods[fieldName] = doc.array();
        } else if (value.typeId() == QMetaType::QString) {
            goods[fieldName] = value.toString();
        } else if (value.typeId() == QMetaType::Int || value.typeId() == QMetaType::LongLong) {
            goods[fieldName] = value.toInt();
        } else if (value.typeId() == QMetaType::Double) {
            goods[fieldName] = value.toDouble();
        }
    }

    return goods;
}

bool DatabaseManager::cacheOrders(const QString& userId, const QJsonArray& orders)
{
    QMutexLocker locker(&m_mutex);

    QSqlQuery query(m_database);

    if (!m_database.transaction()) {
        return false;
    }

    qint64 cacheTime = QDateTime::currentSecsSinceEpoch();

    for (const QJsonValue& orderValue : orders) {
        QJsonObject order = orderValue.toObject();

        query.prepare(R"(
            INSERT OR REPLACE INTO orders_cache
            (order_id, user_id, goods_id, goods_title, seller_id, seller_name,
             buyer_id, buyer_name, amount, status, payment_method, payment_time,
             delivery_info, created_at, updated_at, cache_time)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
        )");

        query.addBindValue(order.value("order_id").toInt());
        query.addBindValue(userId);
        query.addBindValue(order.value("goods_id").toInt());
        query.addBindValue(order.value("goods_title").toString());
        query.addBindValue(order.value("seller_id").toString());
        query.addBindValue(order.value("seller_name").toString());
        query.addBindValue(order.value("buyer_id").toString());
        query.addBindValue(order.value("buyer_name").toString());
        query.addBindValue(order.value("amount").toDouble());
        query.addBindValue(order.value("status").toString());
        query.addBindValue(order.value("payment_method").toString());
        query.addBindValue(order.value("payment_time").toVariant().toLongLong());

        QJsonDocument deliveryDoc(order.value("delivery_info").toObject());
        query.addBindValue(deliveryDoc.toJson(QJsonDocument::Compact));

        query.addBindValue(order.value("created_at").toVariant().toLongLong());
        query.addBindValue(order.value("updated_at").toVariant().toLongLong());
        query.addBindValue(cacheTime);

        if (!query.exec()) {
            m_database.rollback();
            return false;
        }
    }

    return m_database.commit();
}

QJsonArray DatabaseManager::getCachedOrders(const QString& userId)
{
    QMutexLocker locker(&m_mutex);

    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM orders_cache WHERE user_id = ? ORDER BY created_at DESC");
    query.addBindValue(userId);

    QJsonArray orders;

    if (!query.exec()) {
        return orders;
    }

    while (query.next()) {
        QJsonObject order;
        QSqlRecord record = query.record();

        for (int i = 0; i < record.count(); i++) {
            QString fieldName = record.fieldName(i);
            QVariant value = query.value(i);

            if (fieldName == "delivery_info") {
                QJsonDocument doc = QJsonDocument::fromJson(value.toByteArray());
                order[fieldName] = doc.object();
            } else if (value.typeId() == QMetaType::QString) {
                order[fieldName] = value.toString();
            } else if (value.typeId() == QMetaType::Int || value.typeId() == QMetaType::LongLong) {
                order[fieldName] = value.toInt();
            } else if (value.typeId() == QMetaType::Double) {
                order[fieldName] = value.toDouble();
            }
        }

        orders.append(order);
    }

    return orders;
}

bool DatabaseManager::cacheMessage(const QJsonObject& message)
{
    QMutexLocker locker(&m_mutex);

    QSqlQuery query(m_database);
    query.prepare(R"(
        INSERT INTO messages_cache
        (message_id, chat_id, sender_id, sender_name, receiver_id, content,
         message_type, attachments, timestamp, read_status)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    )");

    query.addBindValue(message.value("message_id").toString());
    query.addBindValue(message.value("chat_id").toString());
    query.addBindValue(message.value("sender_id").toString());
    query.addBindValue(message.value("sender_name").toString());
    query.addBindValue(message.value("receiver_id").toString());
    query.addBindValue(message.value("content").toString());
    query.addBindValue(message.value("message_type").toString("text"));

    QJsonDocument attachmentsDoc(message.value("attachments").toArray());
    query.addBindValue(attachmentsDoc.toJson(QJsonDocument::Compact));

    query.addBindValue(message.value("timestamp").toVariant().toLongLong());
    query.addBindValue(message.value("read_status").toInt(0));

    return query.exec();
}

QJsonArray DatabaseManager::getCachedMessages(const QString& chatId, int limit)
{
    QMutexLocker locker(&m_mutex);

    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM messages_cache WHERE chat_id = ? "
                  "ORDER BY timestamp DESC LIMIT ?");
    query.addBindValue(chatId);
    query.addBindValue(limit);

    QJsonArray messages;

    if (!query.exec()) {
        return messages;
    }

    while (query.next()) {
        QJsonObject message;
        QSqlRecord record = query.record();

        for (int i = 0; i < record.count(); i++) {
            QString fieldName = record.fieldName(i);
            QVariant value = query.value(i);

            if (fieldName == "attachments") {
                QJsonDocument doc = QJsonDocument::fromJson(value.toByteArray());
                message[fieldName] = doc.array();
            } else if (value.typeId() == QMetaType::QString) {
                message[fieldName] = value.toString();
            } else if (value.typeId() == QMetaType::Int || value.typeId() == QMetaType::LongLong) {
                message[fieldName] = value.toInt();
            }
        }

        messages.append(message);
    }

    return messages;
}

bool DatabaseManager::addToFavorites(const QString& userId, int goodsId)
{
    QMutexLocker locker(&m_mutex);

    QSqlQuery query(m_database);
    query.prepare(R"(
        INSERT OR REPLACE INTO favorites (user_id, goods_id, added_time)
        VALUES (?, ?, ?)
    )");

    query.addBindValue(userId);
    query.addBindValue(goodsId);
    query.addBindValue(QDateTime::currentSecsSinceEpoch());

    return query.exec();
}

QJsonArray DatabaseManager::getFavorites(const QString& userId)
{
    QMutexLocker locker(&m_mutex);

    QSqlQuery query(m_database);
    query.prepare(R"(
        SELECT g.* FROM goods_cache g
        JOIN favorites f ON g.goods_id = f.goods_id
        WHERE f.user_id = ? AND g.status = 'onsale'
        ORDER BY f.added_time DESC
    )");
    query.addBindValue(userId);

    QJsonArray favorites;

    if (!query.exec()) {
        return favorites;
    }

    while (query.next()) {
        QJsonObject goods;
        QSqlRecord record = query.record();

        for (int i = 0; i < record.count(); i++) {
            QString fieldName = record.fieldName(i);
            QVariant value = query.value(i);

            if (fieldName == "images") {
                QJsonDocument doc = QJsonDocument::fromJson(value.toByteArray());
                goods[fieldName] = doc.array();
            } else if (value.typeId() == QMetaType::QString) {
                goods[fieldName] = value.toString();
            } else if (value.typeId() == QMetaType::Int || value.typeId() == QMetaType::LongLong) {
                goods[fieldName] = value.toInt();
            } else if (value.typeId() == QMetaType::Double) {
                goods[fieldName] = value.toDouble();
            }
        }

        favorites.append(goods);
    }

    return favorites;
}

bool DatabaseManager::clearAllCache()
{
    QMutexLocker locker(&m_mutex);

    QSqlQuery query(m_database);

    QStringList tables = {
        "users", "login_info", "goods_cache", "orders_cache",
        "messages_cache", "favorites", "search_history", "offline_operations"
    };

    bool success = true;
    for (const QString& table : tables) {
        if (!query.exec(QString("DELETE FROM %1").arg(table))) {
            success = false;
            qWarning() << "Failed to clear table" << table << ":" << query.lastError().text();
        }
    }

    return success;
}

bool DatabaseManager::optimizeDatabase()
{
    QMutexLocker locker(&m_mutex);

    QSqlQuery query(m_database);

    // 清理过期缓存（7天以上）
    qint64 cutoffTime = QDateTime::currentSecsSinceEpoch() - 7 * 24 * 3600;

    if (!query.exec(QString("DELETE FROM goods_cache WHERE cache_time < %1").arg(cutoffTime))) {
        return false;
    }

    if (!query.exec("VACUUM")) {
        return false;
    }

    return true;
}
