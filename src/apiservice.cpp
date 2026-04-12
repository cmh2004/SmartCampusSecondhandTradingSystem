#include "apiservice.h"
#include <QCryptographicHash>
#include <QDateTime>
#include <QUuid>
#include <QFileInfo>

ApiService* ApiService::m_instance = nullptr;

ApiService::ApiService(QObject* parent)
    : QObject(parent)
{
    // 连接HTTP客户端信号
    HttpClient* httpClient = HttpClient::instance();
    connect(httpClient, &HttpClient::unauthorized, this, &ApiService::unauthorized);

    // 连接WebSocket客户端信号
    WebSocketClient* wsClient = WebSocketClient::instance();
    connect(wsClient, &WebSocketClient::chatMessageReceived, this, [this](
                                                                       const QString& senderId, const QString& content, const QString& timestamp) {
        QJsonObject message{
            {"sender_id", senderId},
            {"content", content},
            {"timestamp", timestamp}
        };
        emit newMessageReceived(message);
    });

    connect(wsClient, &WebSocketClient::orderStatusChanged, this,
            &ApiService::orderStatusUpdated);
}

ApiService* ApiService::instance()
{
    if (!m_instance) {
        m_instance = new ApiService();
    }
    return m_instance;
}

bool ApiService::initialize(const QString& apiBaseUrl, const QString& wsUrl)
{
    HttpClient* httpClient = HttpClient::instance();
    httpClient->initialize(apiBaseUrl);

    if (!m_authToken.isEmpty()) {
        httpClient->setAuthToken(m_authToken);
    }

    if (!wsUrl.isEmpty()) {
        WebSocketClient* wsClient = WebSocketClient::instance();
        wsClient->connectToServer(wsUrl);
    }

    return true;
}

QJsonObject ApiService::login(const QString& username, const QString& password,
                              const QString& role)
{
    QJsonObject data{
        {"username", username},
        {"password", password},
        {"role", role},
        {"device", "desktop"},
        {"timestamp", QDateTime::currentSecsSinceEpoch()}
    };

    QJsonObject response = HttpClient::instance()->syncRequest("/api/auth/login", data);

    if (response.value("success").toBool()) {
        QJsonObject data = response.value("data").toObject();
        setAuthToken(data.value("token").toString());
        setCurrentUserId(data.value("user_id").toInt());

        // 连接WebSocket（如果有）
        QString wsUrl = response.value("data").toObject().value("ws_url").toString();
        if (!wsUrl.isEmpty()) {
            WebSocketClient::instance()->connectToServer(wsUrl);
        }
    }

    return response;
}

QJsonObject ApiService::registerUser(const QString& username, const QString& password,
                                     const QString& email, const QString& phone,
                                     const QString& nickname)
{
    QJsonObject data{
        {"username", username},
        {"password", password},
        {"email", email},
        {"phone", phone},
        {"nickname", nickname},
        {"register_time", QDateTime::currentSecsSinceEpoch()}
    };

    return HttpClient::instance()->syncRequest("/api/auth/register", data);
}

QJsonObject ApiService::logout()
{
    QJsonObject response = HttpClient::instance()->syncRequest("/api/auth/logout", {}, "POST");

    // 清除本地token
    clearAuthToken();

    // 断开WebSocket
    WebSocketClient::instance()->disconnectFromServer();

    return response;
}

QJsonObject ApiService::getUserProfile(const QString& userId)
{
    QJsonObject params;
    if (!userId.isEmpty()) {
        params["user_id"] = userId;
    }

    return HttpClient::instance()->syncRequest("/api/user/profile", params, "GET");
}

QJsonObject ApiService::publishGoods(const QJsonObject& goodsData, const QStringList& imagePaths)
{
    QJsonArray imageUrls;
    for (const QString &path : imagePaths) {
        QJsonObject uploadResult = uploadImage(path);
        if (uploadResult.value("success").toBool()) {
            QString url = uploadResult.value("data").toObject().value("file_url").toString();
            imageUrls.append(url);
        } else {
            qWarning() << "图片上传失败:" << uploadResult.value("error").toString();
        }
    }

    QJsonObject finalData = goodsData;
    finalData["images"] = imageUrls;
    finalData["publish_time"] = QDateTime::currentSecsSinceEpoch();
    return HttpClient::instance()->syncRequest("/api/goods/publish", finalData);
}

QJsonArray ApiService::searchGoods(const QString& keyword, const QString& category,
                                   double minPrice, double maxPrice,
                                   const QString& sortBy, int page, int pageSize)
{
    QJsonObject params{
        {"keyword", keyword},
        {"category", category},
        {"min_price", minPrice},
        {"max_price", maxPrice},
        {"sort_by", sortBy},
        {"page", page},
        {"page_size", pageSize}
    };

    QJsonObject response = HttpClient::instance()->syncRequest("/api/goods/search", params, "POST");

    qDebug() << "[searchGoods] Full response:" << response;

    bool success = response.value("success").toBool();
    qDebug() << "[searchGoods] success value:" << success;

    if (success) {
        QJsonObject dataObj = response.value("data").toObject();
        QJsonArray goodsList = dataObj.value("goods_list").toArray();
        qDebug() << "[searchGoods] goods_list size:" << goodsList.size();
        return goodsList;
    } else {
        QString error = response.value("error").toString();
        qDebug() << "[searchGoods] Error:" << error;
    }

    return QJsonArray();
}

QJsonObject ApiService::createOrder(int goodsId, const QJsonObject& orderInfo)
{
    QJsonObject data = orderInfo;
    data["goods_id"] = goodsId;
    data["order_time"] = QDateTime::currentSecsSinceEpoch();

    return HttpClient::instance()->syncRequest("/api/order/create", data);
}

QJsonObject ApiService::sendMessage(const QString& receiverId, const QString& content,
                                    int goodsId, const QStringList& attachments)
{
    QJsonObject httpData{
        {"receiver_id", receiverId},
        {"content", content},
        {"goods_id", goodsId},
        {"attachments", QJsonArray::fromStringList(attachments)},
        {"timestamp", QDateTime::currentSecsSinceEpoch()}
    };

    // HTTP发送（持久化）
    QJsonObject httpResponse = HttpClient::instance()->syncRequest("/api/message/send", httpData);

    // WebSocket发送（实时）
    if (httpResponse.value("success").toBool()) {
        WebSocketClient::instance()->sendChatMessage(receiverId, content);
    }

    return httpResponse;
}

QJsonObject ApiService::estimatePrice(const QString& description, const QString& imagePath)
{
    QJsonObject data{{"description", description}};

    // 如果有图片，需要处理图片
    if (!imagePath.isEmpty() && QFileInfo::exists(imagePath)) {
        // 这里可以调用图片分析API
        data["has_image"] = true;
        // 实际项目中应该上传图片到AI服务
    }

    return HttpClient::instance()->syncRequest("/api/ai/estimate", data, "POST", 60000);
}

void ApiService::setAuthToken(const QString& token)
{
    m_authToken = token;
    HttpClient::instance()->setAuthToken(token);
}

QString ApiService::getAuthToken() const
{
    return m_authToken;
}

void ApiService::clearAuthToken()
{
    m_authToken.clear(); // 清空本地token
    HttpClient::instance()->setAuthToken(""); // 清空HTTP客户端的token
}

QJsonObject ApiService::buildResponse(bool success, const QString& message,
                                      const QJsonValue& data)
{
    QJsonObject response{
        {"success", success},
        {"message", message},
        {"timestamp", QDateTime::currentSecsSinceEpoch()}
    };

    if (!data.isUndefined()) {
        response["data"] = data;
    }

    return response;
}

QString ApiService::generateRequestId() const
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

QJsonObject ApiService::markMessageRead(const QString &sessionId) {
    QJsonObject data{{"session_id", sessionId}};
    return HttpClient::instance()->syncRequest("/api/message/mark_read", data, "POST");
}

QJsonObject ApiService::getDisputeByOrder(int orderId) {
    QJsonObject params{{"order_id", orderId}};
    return HttpClient::instance()->syncRequest("/api/dispute/by_order", params, "GET");
}

// ==================== 商品详情 ====================
QJsonObject ApiService::getGoodsDetail(int goodsId)
{
    QJsonObject params;
    params["goods_id"] = goodsId;
    return HttpClient::instance()->syncRequest("/api/goods/detail", params, "POST");
}

// ==================== 纠纷提交 ====================
QJsonObject ApiService::submitDispute(int orderId, const QString& disputeType,
                                      const QString& description, const QStringList& evidence)
{
    QJsonObject data;
    data["order_id"] = orderId;
    data["dispute_type"] = disputeType;
    data["description"] = description;
    QJsonArray evidenceArray = QJsonArray::fromStringList(evidence);
    data["evidence"] = evidenceArray;
    data["submit_time"] = QDateTime::currentSecsSinceEpoch();
    return HttpClient::instance()->syncRequest("/api/dispute/submit", data);
}

// ==================== 评价提交 ====================
QJsonObject ApiService::submitReview(int orderId, int rating, const QString& comment,
                                     const QStringList& images)
{
    QJsonObject data;
    data["order_id"] = orderId;
    data["score"] = rating;
    data["comment"] = comment;
    data["images"] = QJsonArray::fromStringList(images);
    data["review_time"] = QDateTime::currentSecsSinceEpoch();
    return HttpClient::instance()->syncRequest("/api/review/submit", data);
}

// ==================== 更新个人资料 ====================
QJsonObject ApiService::updateUserProfile(const QJsonObject& userData)
{
    return HttpClient::instance()->syncRequest("/api/user/profile", userData, "POST");
}

// ==================== 信用分获取 ====================
QJsonObject ApiService::getCreditScore(const QString& userId)
{
    QJsonObject params;
    if (!userId.isEmpty()) {
        params["user_id"] = userId;
    }
    return HttpClient::instance()->syncRequest("/api/credit/score", params, "GET");
}

// ==================== 信用历史 ====================
QJsonArray ApiService::getCreditHistory(const QString& userId, int page, int pageSize)
{
    QJsonObject params;
    if (!userId.isEmpty()) {
        params["user_id"] = userId;
    }
    params["page"] = page;
    params["page_size"] = pageSize;
    QJsonObject response = HttpClient::instance()->syncRequest("/api/credit/history", params, "GET");
    if (response.value("success").toBool()) {
        return response.value("data").toObject().value("history").toArray();
    }
    return QJsonArray();
}

// ==================== 举报提交 ====================
QJsonObject ApiService::submitReport(int targetId, const QString& targetType,
                                     int reason, const QString& description,
                                     const QStringList& evidence)
{
    QJsonObject data;
    data["target_id"] = targetId;
    // 将 targetType 转换为服务端所需的 target_type (int)
    int targetTypeInt = 0;
    if (targetType == "goods") targetTypeInt = 1;
    else if (targetType == "user") targetTypeInt = 2;
    else if (targetType == "order") targetTypeInt = 3; // 根据实际定义
    data["target_type"] = targetTypeInt;
    data["reason_type"] = reason;   // 服务端字段名为 reason_type
    data["description"] = description;
    data["evidence_urls"] = QJsonArray::fromStringList(evidence); // 服务端期望字符串
    data["report_time"] = QDateTime::currentSecsSinceEpoch();
    return HttpClient::instance()->syncRequest("/api/report/submit", data);
}

// ==================== 获取收藏列表 ====================
QJsonArray ApiService::getFavorites(int page, int pageSize)
{
    QJsonObject params;
    params["page"] = page;
    params["page_size"] = pageSize;
    QJsonObject response = HttpClient::instance()->syncRequest("/api/favorite/list", params, "POST");
    if (response.value("success").toBool()) {
        QJsonArray list = response.value("data").toObject().value("list").toArray();
        return list;
    }
    return QJsonArray();
}

// ==================== 获取聊天列表 ====================
QJsonArray ApiService::getChatList()
{
    QJsonObject response = HttpClient::instance()->syncRequest("/api/message/chatlist", {}, "GET");
    if (response.value("success").toBool()) {
        return response.value("data").toObject().value("chats").toArray();
    }
    return QJsonArray();
}

// ==================== 获取聊天消息历史 ====================
QJsonArray ApiService::getMessageHistory(const QString& chatId, int page, int pageSize)
{
    QJsonObject params;
    params["chat_id"] = chatId;
    params["page"] = page;
    params["page_size"] = pageSize;
    QJsonObject response = HttpClient::instance()->syncRequest("/api/message/history", params, "GET");
    if (response.value("success").toBool()) {
        return response.value("data").toObject().value("messages").toArray();
    }
    return QJsonArray();
}

// ==================== 获取订单列表 ====================
QJsonArray ApiService::getOrderList(const QString& status, int page, int pageSize, const QString& keyword)
{
    QJsonObject params;
    if (!status.isEmpty()) {
        params["status"] = status;
    }
    if (!keyword.isEmpty()) {
        params["keyword"] = keyword;
    }
    params["page"] = page;
    params["page_size"] = pageSize;
    QJsonObject response = HttpClient::instance()->syncRequest("/api/order/list", params, "POST");
    if (response.value("success").toBool()) {
        return response.value("data").toObject().value("orders").toArray();
    }
    return QJsonArray();
}

// ==================== 订单支付 ====================
QJsonObject ApiService::payOrder(int orderId, const QString& paymentMethod, double amount)
{
    QJsonObject data;
    data["order_id"] = orderId;
    data["payment_method"] = paymentMethod;
    data["amount"] = amount;
    data["pay_time"] = QDateTime::currentSecsSinceEpoch();
    return HttpClient::instance()->syncRequest("/api/order/pay", data);
}

// ==================== 取消订单 ====================
QJsonObject ApiService::cancelOrder(int orderId, const QString& reason)
{
    QJsonObject data;
    data["order_id"] = orderId;
    data["reason"] = reason;
    data["cancel_time"] = QDateTime::currentSecsSinceEpoch();
    return HttpClient::instance()->syncRequest("/api/order/cancel", data);
}

// ==================== 确认收货 ====================
QJsonObject ApiService::confirmOrder(int orderId)
{
    QJsonObject data;
    data["order_id"] = orderId;
    data["confirm_time"] = QDateTime::currentSecsSinceEpoch();
    return HttpClient::instance()->syncRequest("/api/order/confirm", data);
}

// 添加收藏
QJsonObject ApiService::addFavorite(int goodsId)
{
    QJsonObject data;
    data["goods_id"] = goodsId;
    return HttpClient::instance()->syncRequest("/api/favorite/add", data, "POST");
}

// 上传头像
QJsonObject ApiService::uploadAvatar(const QString& filePath)
{
    // 方法一：使用文件上传（multipart/form-data）
    // 如果服务端支持文件上传，可以使用 uploadFile
    // 但需确保 HttpClient 有 uploadFile 方法且服务端路由正确
    // 这里使用 base64 方式，因为服务端 handleUploadAvatar 期望 base64 字符串

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QJsonObject error;
        error["success"] = false;
        error["error"] = "无法打开文件";
        return error;
    }

    QByteArray imageData = file.readAll();
    QString base64 = imageData.toBase64();

    QJsonObject data;
    data["avatar_base64"] = base64;

    return HttpClient::instance()->syncRequest("/api/user/avatar", data, "POST");
}

// 刷新 token
QJsonObject ApiService::refreshToken()
{
    return HttpClient::instance()->syncRequest("/api/auth/refresh", {}, "POST");
}

// 更新商品信息
QJsonObject ApiService::updateGoods(int goodsId, const QJsonObject& goodsData)
{
    QJsonObject data = goodsData;
    data["goods_id"] = goodsId;
    return HttpClient::instance()->syncRequest("/api/goods/update", data);
}

// 删除商品
QJsonObject ApiService::deleteGoods(int goodsId)
{
    QJsonObject data{{"goods_id", goodsId}};
    return HttpClient::instance()->syncRequest("/api/goods/delete", data);
}

// 获取推荐商品列表
QJsonArray ApiService::getRecommendedGoods(int limit)
{
    QJsonObject params{{"limit", limit}};
    QJsonObject response = HttpClient::instance()->syncRequest("/api/goods/recommended", params, "GET");
    if (response.value("success").toBool()) {
        return response.value("data").toArray();
    }
    return QJsonArray();
}

// 获取订单详情
QJsonObject ApiService::getOrderDetail(int orderId)
{
    QJsonObject params{{"order_id", orderId}};
    return HttpClient::instance()->syncRequest("/api/order/detail", params, "GET");
}

QJsonObject ApiService::getPaymentMethods()
{
    // 假设后端提供支付方式列表，若无则返回模拟数据
    return HttpClient::instance()->syncRequest("/api/payment/methods", {}, "GET");
}

QJsonObject ApiService::initPayment(int orderId, const QString& method)
{
    QJsonObject data{{"order_id", orderId}, {"method", method}};
    return HttpClient::instance()->syncRequest("/api/payment/init", data);
}

// 获取未读消息总数（若后端提供接口）
int ApiService::getUnreadMessageCount()
{
    QJsonObject response = HttpClient::instance()->syncRequest("/api/message/unread_count", {}, "GET");
    if (response.value("success").toBool()) {
        return response.value("data").toObject().value("count").toInt();
    }
    return 0;
}

// 获取商品的评价列表
QJsonArray ApiService::getGoodsReviews(int goodsId, int page, int pageSize)
{
    QJsonObject params{{"goods_id", goodsId}, {"page", page}, {"page_size", pageSize}};
    QJsonObject response = HttpClient::instance()->syncRequest("/api/review/goods", params, "GET");
    if (response.value("success").toBool()) {
        return response.value("data").toArray();
    }
    return QJsonArray();
}

// 获取卖家的评价列表
QJsonObject ApiService::getSellerReviews(const QString& sellerId, int page, int pageSize)
{
    QJsonObject params{{"seller_id", sellerId}, {"page", page}, {"page_size", pageSize}};
    return HttpClient::instance()->syncRequest("/api/review/seller", params, "GET");
}

// 取消收藏
QJsonObject ApiService::removeFavorite(int goodsId)
{
    QJsonObject data{{"goods_id", goodsId}};
    return HttpClient::instance()->syncRequest("/api/favorite/remove", data, "POST");
}

// 获取我的举报列表
QJsonArray ApiService::getMyReports(int page, int pageSize)
{
    QJsonObject params{{"page", page}, {"page_size", pageSize}};
    QJsonObject response = HttpClient::instance()->syncRequest("/api/report/mylist", params, "GET");
    if (response.value("success").toBool()) {
        return response.value("data").toArray();
    }
    return QJsonArray();
}

// 获取纠纷详情
QJsonObject ApiService::getDisputeDetail(int disputeId)
{
    QJsonObject params{{"dispute_id", disputeId}};
    return HttpClient::instance()->syncRequest("/api/dispute/detail", params, "GET");
}

// 获取我的纠纷列表
QJsonArray ApiService::getMyDisputes(const QString& status, int page, int pageSize)
{
    QJsonObject params;
    if (!status.isEmpty()) params["status"] = status;
    params["page"] = page;
    params["page_size"] = pageSize;
    QJsonObject response = HttpClient::instance()->syncRequest("/api/dispute/mylist", params, "GET");
    if (response.value("success").toBool()) {
        return response.value("data").toArray();
    }
    return QJsonArray();
}

// 获取待审核商品列表
QJsonArray ApiService::getPendingGoods(int page, int pageSize)
{
    QJsonObject params{{"page", page}, {"page_size", pageSize}};
    QJsonObject response = HttpClient::instance()->syncRequest("/api/admin/pending_goods", params, "POST");
    if (response.value("success").toBool()) {
        return response.value("data").toArray();
    }
    return QJsonArray();
}

// 审核商品
QJsonObject ApiService::reviewGoods(int goodsId, bool approved, const QString& comment)
{
    QJsonObject data{{"goods_id", goodsId}, {"approved", approved}, {"comment", comment}};
    return HttpClient::instance()->syncRequest("/api/admin/review_goods", data);
}

// 获取用户列表（管理员）
QJsonArray ApiService::getUserList(const QString& role, int page, int pageSize)
{
    QJsonObject params;
    if (!role.isEmpty()) params["role"] = role;
    params["page"] = page;
    params["page_size"] = pageSize;
    QJsonObject response = HttpClient::instance()->syncRequest("/api/admin/user_list", params, "GET");
    if (response.value("success").toBool()) {
        return response.value("data").toArray();
    }
    return QJsonArray();
}

// 更新用户状态（管理员）
QJsonObject ApiService::updateUserStatus(const QString& userId, const QString& status, const QString& reason)
{
    QJsonObject data{{"user_id", userId}, {"status", status}, {"reason", reason}};
    return HttpClient::instance()->syncRequest("/api/admin/update_user_status", data);
}

// 获取纠纷列表（管理员）
QJsonArray ApiService::getDisputeList(const QString& status, int page, int pageSize)
{
    QJsonObject params;
    if (!status.isEmpty()) params["status"] = status;
    params["page"] = page;
    params["page_size"] = pageSize;
    QJsonObject response = HttpClient::instance()->syncRequest("/api/admin/dispute_list", params, "GET");
    if (response.value("success").toBool()) {
        return response.value("data").toArray();
    }
    return QJsonArray();
}

// 处理纠纷（管理员）
QJsonObject ApiService::processDispute(int disputeId, const QString& result, const QString& comment)
{
    QJsonObject data{{"dispute_id", disputeId}, {"result", result}, {"comment", comment}};
    return HttpClient::instance()->syncRequest("/api/admin/process_dispute", data);
}

// 获取统计信息（管理员）
QJsonObject ApiService::getStatistics(const QString& period)
{
    QJsonObject params{{"period", period}};
    return HttpClient::instance()->syncRequest("/api/admin/statistics", params, "GET");
}

QJsonObject ApiService::uploadImage(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return {{"success", false}, {"error", "无法打开文件"}};
    }
    QByteArray imageData = file.readAll();
    QString base64 = imageData.toBase64();

    // 推断图片类型（可选）
    QString imageType;
    if (filePath.endsWith(".png", Qt::CaseInsensitive)) imageType = "image/png";
    else if (filePath.endsWith(".jpg", Qt::CaseInsensitive) || filePath.endsWith(".jpeg", Qt::CaseInsensitive))
        imageType = "image/jpeg";
    else imageType = "image/jpeg"; // 默认

    QJsonObject data;
    data["image_base64"] = base64;
    data["image_type"] = imageType;

    return HttpClient::instance()->syncRequest("/api/upload/image", data);
}
