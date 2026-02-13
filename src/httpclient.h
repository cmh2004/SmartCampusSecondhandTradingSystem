#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTimer>
#include <QMap>
#include <QMutex>
#include <QWaitCondition>
#include <QSslConfiguration>

// HTTP客户端类
class HttpClient : public QObject
{
    Q_OBJECT

public:
    static HttpClient* instance();

    // 初始化配置
    void initialize(const QString& baseUrl);

    // 基础HTTP方法
    QNetworkReply* get(const QString& endpoint, const QJsonObject& params = QJsonObject());
    QNetworkReply* post(const QString& endpoint, const QJsonObject& data = QJsonObject());
    QNetworkReply* put(const QString& endpoint, const QJsonObject& data = QJsonObject());
    QNetworkReply* deleteResource(const QString& endpoint);

    // 同步请求（阻塞）
    QJsonObject syncRequest(const QString& endpoint,
                            const QJsonObject& data = QJsonObject(),
                            const QString& method = "POST",
                            int timeout = 10000);

    // 文件上传
    QNetworkReply* uploadFile(const QString& endpoint,
                              const QString& filePath,
                              const QJsonObject& formData = QJsonObject());

    // 设置认证令牌
    void setAuthToken(const QString& token);
    void clearAuthToken();

    // 设置超时
    void setTimeout(int ms);

    // 服务器状态
    QString serverBaseUrl() const;

signals:
    void requestStarted(const QString& endpoint);
    void requestFinished(const QString& endpoint, const QJsonObject& response);
    void requestFailed(const QString& endpoint, const QString& error);
    void unauthorized();  // 认证失效

private slots:
    void onRequestFinished(QNetworkReply* reply);
    void handleSslErrors(QNetworkReply* reply, const QList<QSslError>& errors);

private:
    explicit HttpClient(QObject* parent = nullptr);
    ~HttpClient();

    QNetworkRequest createRequest(const QString& endpoint) const;
    QJsonObject parseReply(QNetworkReply* reply);
    QString buildQueryString(const QJsonObject& params) const;

    QNetworkAccessManager* m_networkManager;
    QString m_baseUrl;
    QString m_authToken;
    int m_timeout;

    static HttpClient* m_instance;
};

#endif // HTTPCLIENT_H
