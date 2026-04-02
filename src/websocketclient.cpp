#include "websocketclient.h"
#include "apiservice.h"
#include <QUrl>
#include <QUrlQuery>

WebSocketClient* WebSocketClient::m_instance = nullptr;

WebSocketClient::WebSocketClient(QObject* parent)
    : QObject(parent)
    , m_webSocket(new QWebSocket)
    , m_reconnectTimer(new QTimer(this))
    , m_heartbeatTimer(new QTimer(this))
    , m_autoReconnect(true)
{
    // 连接信号
    connect(m_webSocket, &QWebSocket::connected, this, &WebSocketClient::onConnected);
    connect(m_webSocket, &QWebSocket::disconnected, this, &WebSocketClient::onDisconnected);
    connect(m_webSocket, &QWebSocket::textMessageReceived,
            this, &WebSocketClient::onTextMessageReceived);
    connect(m_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::errorOccurred),
            this, &WebSocketClient::onError);

    // 配置重连定时器
    m_reconnectTimer->setInterval(5000); // 5秒重连
    connect(m_reconnectTimer, &QTimer::timeout, this, &WebSocketClient::reconnect);

    // 配置心跳定时器
    m_heartbeatTimer->setInterval(30000); // 30秒心跳
    connect(m_heartbeatTimer, &QTimer::timeout, this, &WebSocketClient::sendHeartbeat);
}

WebSocketClient::~WebSocketClient()
{
    disconnectFromServer();
    m_webSocket->deleteLater();
}

WebSocketClient* WebSocketClient::instance()
{
    if (!m_instance) {
        m_instance = new WebSocketClient();
    }
    return m_instance;
}

bool WebSocketClient::connectToServer(const QString& url)
{
    if (m_webSocket->state() == QAbstractSocket::ConnectedState) {
        return true;
    }

    m_serverUrl = url;

    // 添加认证token到URL（如果有）
    QUrl qUrl(url);
    // 如果需要，可以在这里添加查询参数

    m_webSocket->open(qUrl);
    return true;
}

void WebSocketClient::disconnectFromServer()
{
    m_autoReconnect = false;
    m_reconnectTimer->stop();
    m_heartbeatTimer->stop();

    if (m_webSocket->state() == QAbstractSocket::ConnectedState) {
        m_webSocket->close();
    }
}

bool WebSocketClient::isConnected() const
{
    return m_webSocket->state() == QAbstractSocket::ConnectedState;
}

void WebSocketClient::sendChatMessage(const QString& receiverId, const QString& content)
{
    QJsonObject data{
        {"receiver_id", receiverId},
        {"content", content},
        {"timestamp", QDateTime::currentSecsSinceEpoch()}
    };

    sendMessage(WsMessageType::CHAT_MESSAGE, data);
}

void WebSocketClient::sendHeartbeat()
{
    QJsonObject data{{"timestamp", QDateTime::currentSecsSinceEpoch()}};
    sendMessage(WsMessageType::HEARTBEAT, data);
}

void WebSocketClient::subscribe(const QString& channel)
{
    QJsonObject data{{"channel", channel}};
    sendMessage(WsMessageType::SYSTEM_ALERT, data); // 使用系统消息类型
}

void WebSocketClient::unsubscribe(const QString& channel)
{
    QJsonObject data{{"channel", channel}, {"action", "unsubscribe"}};
    sendMessage(WsMessageType::SYSTEM_ALERT, data);
}

void WebSocketClient::onConnected()
{
    qDebug() << "WebSocket connected";
    m_reconnectTimer->stop();
    m_heartbeatTimer->start();

    // 发送认证消息
    if (ApiService::instance()->getCurrentUserId() != -1) {
        QJsonObject authMsg;
        authMsg["type"] = 0;  // 约定类型 0 为认证
        authMsg["userId"] = ApiService::instance()->getCurrentUserId();
        authMsg["token"] = ApiService::instance()->getAuthToken();
        sendMessage(WsMessageType::SYSTEM_ALERT, authMsg);
    }

    // 发送连接成功的消息
    QJsonObject data{
        {"client_type", "desktop"},
        {"version", "1.0.0"},
        {"timestamp", QDateTime::currentSecsSinceEpoch()}
    };
    sendMessage(WsMessageType::SYSTEM_ALERT, data);

    emit connected();
}

void WebSocketClient::onDisconnected()
{
    qDebug() << "WebSocket disconnected";
    m_heartbeatTimer->stop();

    emit disconnected();

    // 自动重连
    if (m_autoReconnect) {
        m_reconnectTimer->start();
    }
}

void WebSocketClient::onTextMessageReceived(const QString& message)
{
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "Failed to parse WebSocket message:" << parseError.errorString();
        return;
    }

    if (!doc.isObject()) {
        qWarning() << "WebSocket message is not a JSON object";
        return;
    }

    processMessage(doc.object());
}

void WebSocketClient::onError(QAbstractSocket::SocketError error)
{
    QString errorStr = m_webSocket->errorString();
    qWarning() << "WebSocket error:" << error << errorStr;
    emit connectionError(errorStr);
}

void WebSocketClient::reconnect()
{
    if (m_webSocket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "Attempting to reconnect WebSocket...";
        connectToServer(m_serverUrl);
    } else {
        m_reconnectTimer->stop();
    }
}

void WebSocketClient::checkConnection()
{
    if (m_webSocket->state() != QAbstractSocket::ConnectedState) {
        m_heartbeatTimer->stop();
        if (m_autoReconnect) {
            m_reconnectTimer->start();
        }
    }
}

void WebSocketClient::sendMessage(WsMessageType type, const QJsonObject& data)
{
    if (!isConnected()) {
        qWarning() << "WebSocket not connected, cannot send message";
        return;
    }

    QJsonObject message{
        {"type", static_cast<int>(type)},
        {"data", data},
        {"timestamp", QDateTime::currentSecsSinceEpoch()}
    };

    QJsonDocument doc(message);
    m_webSocket->sendTextMessage(doc.toJson(QJsonDocument::Compact));
}

void WebSocketClient::processMessage(const QJsonObject& message)
{
    int type = message.value("type").toInt();
    QJsonObject data = message.value("data").toObject();

    switch (static_cast<WsMessageType>(type)) {
    case WsMessageType::CHAT_MESSAGE:
        emit chatMessageReceived(
            data.value("sender_id").toString(),
            data.value("content").toString(),
            data.value("timestamp").toString()
            );
        break;

    case WsMessageType::NOTIFICATION:
        emit notificationReceived(
            data.value("title").toString(),
            data.value("content").toString(),
            data.value("type").toString()
            );
        break;

    case WsMessageType::ORDER_UPDATE:
        emit orderStatusChanged(
            data.value("order_id").toInt(),
            data.value("status").toString()
            );
        break;

    case WsMessageType::SYSTEM_ALERT:
        emit systemAlertReceived(data.value("message").toString());
        break;

    case WsMessageType::HEARTBEAT:
        // 心跳响应，更新连接状态
        break;

    default:
        qWarning() << "Unknown WebSocket message type:" << type;
        break;
    }
}
