#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#include <QObject>
#include <QWebSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTimer>
#include <QAbstractSocket>
#include <QMap>

// WebSocket消息类型
enum class WsMessageType {
    CHAT_MESSAGE = 1,
    NOTIFICATION = 2,
    ORDER_UPDATE = 3,
    SYSTEM_ALERT = 4,
    HEARTBEAT = 99
};

// WebSocket客户端
class WebSocketClient : public QObject
{
    Q_OBJECT

public:
    static WebSocketClient* instance();

    // 连接管理
    bool connectToServer(const QString& url);
    void disconnectFromServer();
    bool isConnected() const;

    // 消息发送
    void sendChatMessage(const QString& receiverId, const QString& content);
    void sendHeartbeat();

    // 订阅管理
    void subscribe(const QString& channel);
    void unsubscribe(const QString& channel);

signals:
    void connected();
    void disconnected();
    void connectionError(const QString& error);

    // 消息接收
    void chatMessageReceived(const QString& senderId, const QString& content,
                             const QString& timestamp);
    void notificationReceived(const QString& title, const QString& content,
                              const QString& type);
    void orderStatusChanged(int orderId, const QString& status);
    void systemAlertReceived(const QString& message);

private slots:
    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(const QString& message);
    void onError(QAbstractSocket::SocketError error);
    void reconnect();
    void checkConnection();

private:
    explicit WebSocketClient(QObject* parent = nullptr);
    ~WebSocketClient();

    void sendMessage(WsMessageType type, const QJsonObject& data);
    void processMessage(const QJsonObject& message);

    QWebSocket* m_webSocket;
    QTimer* m_reconnectTimer;
    QTimer* m_heartbeatTimer;
    QString m_serverUrl;
    bool m_autoReconnect;

    static WebSocketClient* m_instance;
};

#endif // WEBSOCKETCLIENT_H
