#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMutex>

// 本地数据库管理类（用于缓存数据）
class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    static DatabaseManager* instance();

    // 初始化数据库
    bool initialize(const QString& databasePath = "local_cache.db");

    // 用户相关操作
    bool saveUserInfo(const QJsonObject& userInfo);
    QJsonObject getUserInfo(const QString& userId);
    bool updateUserInfo(const QString& userId, const QJsonObject& updates);
    bool saveLoginInfo(const QString& username, const QString& passwordHash);
    QJsonObject getLoginInfo();

    // 商品缓存操作
    bool cacheGoodsList(const QJsonArray& goodsList);
    bool cacheGoodsDetail(int goodsId, const QJsonObject& goodsInfo);
    QJsonArray getCachedGoodsList(int page = 1, int pageSize = 20);
    QJsonObject getCachedGoodsDetail(int goodsId);
    bool clearGoodsCache(int daysOld = 7);

    // 订单缓存操作
    bool cacheOrders(const QString& userId, const QJsonArray& orders);
    QJsonArray getCachedOrders(const QString& userId);
    bool updateOrderStatus(int orderId, const QString& status);

    // 消息缓存操作
    bool cacheMessage(const QJsonObject& message);
    QJsonArray getCachedMessages(const QString& chatId, int limit = 50);
    bool markMessageRead(const QString& messageId);
    int getUnreadCount(const QString& userId);

    // 收藏相关
    bool addToFavorites(const QString& userId, int goodsId);
    bool removeFromFavorites(const QString& userId, int goodsId);
    QJsonArray getFavorites(const QString& userId);
    bool isFavorited(const QString& userId, int goodsId);

    // 搜索历史
    bool addSearchHistory(const QString& userId, const QString& keyword);
    QJsonArray getSearchHistory(const QString& userId, int limit = 10);
    bool clearSearchHistory(const QString& userId);

    // 离线操作队列
    bool enqueueOfflineOperation(const QJsonObject& operation);
    QJsonArray getPendingOperations(const QString& userId);
    bool removeOperation(int operationId);

    // 清理操作
    bool clearAllCache();
    qint64 getCacheSize();
    bool optimizeDatabase();

private:
    explicit DatabaseManager(QObject* parent = nullptr);
    ~DatabaseManager();

    bool createTables();
    bool executeScript(const QString& scriptPath);

    QSqlDatabase m_database;
    QMutex m_mutex;
    QString m_databasePath;

    static DatabaseManager* m_instance;
};

#endif // DATABASEMANAGER_H
