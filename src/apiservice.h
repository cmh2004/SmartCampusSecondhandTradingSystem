#ifndef APISERVICE_H
#define APISERVICE_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include "httpclient.h"
#include "websocketclient.h"

// API服务统一管理类
class ApiService : public QObject
{
    Q_OBJECT

public:
    static ApiService* instance();

    // 初始化
    bool initialize(const QString& apiBaseUrl, const QString& wsUrl = "");
    QJsonObject uploadImage(const QString &filePath);

    // 用户认证
    QJsonObject login(const QString& username, const QString& password, const QString& role);
    QJsonObject registerUser(const QString& username, const QString& password,
                             const QString& email, const QString& phone, const QString& nickname);
    QJsonObject logout();
    QJsonObject refreshToken();

    // 用户信息
    QJsonObject getUserProfile(const QString& userId = "");
    QJsonObject updateUserProfile(const QJsonObject& userData);
    QJsonObject uploadAvatar(const QString& filePath);
    QJsonObject changePassword(const QString &oldPassword, const QString &newPassword);

    // 商品管理
    QJsonObject publishGoods(const QJsonObject& goodsData, const QStringList& imagePaths);
    QJsonArray searchGoods(const QString& keyword = "",
                           const QString& category = "",
                           double minPrice = 0,
                           double maxPrice = 0,
                           const QString& sortBy = "newest",
                           int page = 1,
                           int pageSize = 20);
    QJsonObject getGoodsDetail(int goodsId);
    QJsonObject updateGoods(int goodsId, const QJsonObject& goodsData);
    QJsonObject deleteGoods(int goodsId);
    QJsonArray getRecommendedGoods(int limit = 10);

    // AI估价
    QJsonObject estimatePrice(const QString& description, const QString& imagePath = "");

    // 订单管理
    QJsonObject createOrder(int goodsId, const QJsonObject& orderInfo);
    QJsonObject payOrder(int orderId, const QString& paymentMethod, double amount);
    QJsonObject confirmOrder(int orderId);
    QJsonObject cancelOrder(int orderId, const QString& reason);
    QJsonArray getOrderList(const QString &status = "", int page = 1, int pageSize = 20, const QString &keyword = "");
    QJsonObject getOrderDetail(int orderId);

    // 支付相关
    QJsonObject getPaymentMethods();
    QJsonObject initPayment(int orderId, const QString& method);

    // 聊天消息
    QJsonObject sendMessage(const QString& receiverId, const QString& content,
                            int goodsId, const QStringList& attachments = QStringList());
    QJsonArray getMessageHistory(const QString& chatId, int page = 1, int pageSize = 50);
    QJsonArray getChatList();
    int getUnreadMessageCount();

    // 评价系统
    QJsonObject submitReview(int orderId, int rating, const QString& comment,
                             const QStringList& images = QStringList());
    QJsonArray getGoodsReviews(int goodsId, int page = 1, int pageSize = 10);
    QJsonObject getSellerReviews(const QString& sellerId, int page = 1, int pageSize = 10);

    // 收藏功能
    QJsonObject addFavorite(int goodsId);
    QJsonObject removeFavorite(int goodsId);
    QJsonArray getFavorites(int page = 1, int pageSize = 20);

    // 信用系统
    QJsonObject getCreditScore(int userId = -1);
    QJsonArray getCreditHistory(int userId = -1, int page = 1, int pageSize = 20);

    // 举报系统
    QJsonObject submitReport(int targetId, const QString& targetType,
                             int reason, const QString& description,
                             const QStringList& evidence = QStringList());
    QJsonArray getMyReports(int page = 1, int pageSize = 20);

    // 纠纷处理
    QJsonObject submitDispute(int orderId, const QString& disputeType,
                              const QString& description, const QStringList& evidence={});
    QJsonObject getDisputeDetail(int disputeId);
    QJsonArray getMyDisputes(const QString& status = "", int page = 1, int pageSize = 20);
    QJsonObject getDisputeByOrder(int orderId);

    // 管理员接口
    QJsonArray getPendingGoods(int page = 1, int pageSize = 20);
    QJsonObject reviewGoods(int goodsId, bool approved, const QString& comment = "");
    QJsonArray getUserList(const QString& role = "", int page = 1, int pageSize = 20);
    QJsonObject updateUserStatus(const QString& userId, const QString& status, const QString& reason = "");
    QJsonArray getDisputeList(const QString& status = "", int page = 1, int pageSize = 20);
    QJsonObject processDispute(int disputeId, const QString& result, const QString& comment);
    QJsonObject getStatistics(const QString& period = "daily");
    QJsonArray getAllReports(int page = 1, int pageSize = 20, const QString& status = "");
    bool processReport(int reportId, const QString& result);
    QJsonObject updateUserCreditScore(int userId, int newScore, const QString& reason = "");

    // 设置认证token
    void setAuthToken(const QString& token);
    QString getAuthToken() const;
    void clearAuthToken();

    void setCurrentUserId(int userId) { m_currentUserId = userId; }
    int getCurrentUserId() const { return m_currentUserId; }
    // 标记会话消息为已读
    QJsonObject markMessageRead(const QString &sessionId);
    // 获取我发布的商品
    QJsonArray getMyGoods(int page = 1, int pageSize = 20);
    // 获取我的评价记录（作为买家或卖家）
    QJsonArray getMyReviews(int page = 1, int pageSize = 20);
    // 获取浏览历史
    QJsonArray getBrowseHistory(int page = 1, int pageSize = 20);
    // 添加浏览记录（在商品详情页调用）
    QJsonObject addBrowseHistory(int goodsId);
    // 更新商品状态（下架/上架等）
    QJsonObject updateGoodsStatus(int goodsId, int status);

signals:
    void unauthorized();  // 认证失效
    void newMessageReceived(const QJsonObject& message);
    void orderStatusUpdated(int orderId, const QString& status);
    void newNotification(const QJsonObject& notification);
    void favoriteChanged();

private:
    explicit ApiService(QObject* parent = nullptr);

    // 辅助方法
    QJsonObject buildResponse(bool success, const QString& message = "",
                              const QJsonValue& data = QJsonValue());
    QString generateRequestId() const;

    QString m_authToken;
    int m_currentUserId = -1;

    static ApiService* m_instance;
};

#endif // APISERVICE_H
