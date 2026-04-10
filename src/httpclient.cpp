#include "httpclient.h"
#include <QUrlQuery>
#include <QEventLoop>
#include <QTimer>
#include <QStandardPaths>
#include <QHttpPart>
#include <QDir>
#include <QFile>

HttpClient* HttpClient::m_instance = nullptr;

HttpClient::HttpClient(QObject* parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_timeout(40000)
{
    // 创建缓存目录
    QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    QDir dir(cacheDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
}

HttpClient::~HttpClient()
{
}

HttpClient* HttpClient::instance()
{
    if (!m_instance) {
        m_instance = new HttpClient();
    }
    return m_instance;
}

void HttpClient::initialize(const QString& baseUrl)
{
    m_baseUrl = baseUrl;

    // 配置SSL（如果使用HTTPS）
    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone); // 开发环境可关闭验证
    QSslConfiguration::setDefaultConfiguration(sslConfig);

    qDebug() << "HTTP Client initialized with base URL:" << baseUrl;
}

QNetworkReply* HttpClient::get(const QString& endpoint, const QJsonObject& params)
{
    QString urlStr = m_baseUrl + endpoint;
    if (!params.isEmpty()) {
        urlStr += "?" + buildQueryString(params);
    }

    QNetworkRequest request = createRequest(urlStr);
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                         QNetworkRequest::RedirectPolicy::UserVerifiedRedirectPolicy);

    emit requestStarted(endpoint);
    return m_networkManager->get(request);
}

QNetworkReply* HttpClient::post(const QString& endpoint, const QJsonObject& data)
{
    QString urlStr = m_baseUrl + endpoint;
    QNetworkRequest request = createRequest(urlStr);

    QJsonDocument doc(data);
    QByteArray postData = doc.toJson();

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, postData.size());

    emit requestStarted(endpoint);
    return m_networkManager->post(request, postData);
}

QNetworkReply* HttpClient::put(const QString& endpoint, const QJsonObject& data)
{
    QString urlStr = m_baseUrl + endpoint;
    QNetworkRequest request = createRequest(urlStr);

    QJsonDocument doc(data);
    QByteArray putData = doc.toJson();

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, putData.size());

    emit requestStarted(endpoint);
    return m_networkManager->put(request, putData);
}

QNetworkReply* HttpClient::deleteResource(const QString& endpoint)
{
    QString urlStr = m_baseUrl + endpoint;
    QNetworkRequest request = createRequest(urlStr);

    emit requestStarted(endpoint);
    return m_networkManager->deleteResource(request);
}

QJsonObject HttpClient::syncRequest(const QString& endpoint, const QJsonObject& data,
                                    const QString& method, int timeout)
{
    qDebug() << "[HTTP] Request start:" << endpoint << "timeout:" << timeout;
    auto startTime = QDateTime::currentMSecsSinceEpoch();

    QNetworkReply* reply = nullptr;

    if (method.toUpper() == "GET") {
        reply = get(endpoint, data);
    } else if (method.toUpper() == "POST") {
        reply = post(endpoint, data);
    } else if (method.toUpper() == "PUT") {
        reply = put(endpoint, data);
    } else if (method.toUpper() == "DELETE") {
        reply = deleteResource(endpoint);
    } else {
        return QJsonObject{{"success", false}, {"error", "Unsupported method"}};
    }

    // 创建事件循环等待响应
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);

    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);

    timer.start(timeout);
    loop.exec();

    auto elapsed = QDateTime::currentMSecsSinceEpoch() - startTime;
    qDebug() << "[HTTP] Request finished, elapsed:" << elapsed << "ms";

    QJsonObject response;
    if (timer.isActive()) {
        // 没有超时
        timer.stop();
        response = parseReply(reply);
    } else {
        // 超时
        response = QJsonObject{{"success", false}, {"error", "Request timeout"}};
        reply->abort();
    }

    reply->deleteLater();
    return response;
}

QNetworkReply* HttpClient::uploadFile(const QString& endpoint, const QString& filePath,
                                      const QJsonObject& formData)
{
    QString urlStr = m_baseUrl + endpoint;
    QNetworkRequest request = createRequest(urlStr);

    // 创建multipart/form-data
    QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    // 添加表单字段
    for (auto it = formData.begin(); it != formData.end(); ++it) {
        QHttpPart textPart;
        textPart.setHeader(QNetworkRequest::ContentDispositionHeader,
                           QString("form-data; name=\"%1\"").arg(it.key()));
        textPart.setBody(it.value().toString().toUtf8());
        multiPart->append(textPart);
    }

    // 添加文件
    QFile* file = new QFile(filePath);
    if (!file->open(QIODevice::ReadOnly)) {
        delete multiPart;
        delete file;
        return nullptr;
    }

    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                       QString("form-data; name=\"file\"; filename=\"%1\"")
                           .arg(QFileInfo(filePath).fileName()));
    filePart.setBodyDevice(file);
    file->setParent(multiPart); // 删除multiPart时会自动删除file
    multiPart->append(filePart);

    emit requestStarted(endpoint);
    QNetworkReply* reply = m_networkManager->post(request, multiPart);
    multiPart->setParent(reply); // 删除reply时会自动删除multiPart

    return reply;
}

void HttpClient::setAuthToken(const QString& token)
{
    m_authToken = token;
}

void HttpClient::clearAuthToken()
{
    m_authToken.clear();
}

void HttpClient::setTimeout(int ms)
{
    m_timeout = ms;
}

QString HttpClient::serverBaseUrl() const
{
    return m_baseUrl;
}

void HttpClient::onRequestFinished(QNetworkReply* reply)
{
    QString endpoint = reply->url().toString().remove(m_baseUrl);
    QJsonObject response = parseReply(reply);

    if (reply->error() == QNetworkReply::NoError) {
        emit requestFinished(endpoint, response);

        // 检查是否需要重新认证
        if (response.value("code").toInt() == 401) {
            emit unauthorized();
        }
    } else {
        QString error = reply->errorString();
        emit requestFailed(endpoint, error);
    }

    reply->deleteLater();
}

void HttpClient::handleSslErrors(QNetworkReply* reply, const QList<QSslError>& errors)
{
    qWarning() << "SSL errors occurred:";
    for (const QSslError& error : errors) {
        qWarning() << " -" << error.errorString();
    }

    // 开发环境可以忽略SSL错误
    reply->ignoreSslErrors();
}

QNetworkRequest HttpClient::createRequest(const QString& urlStr) const
{
    QUrl url(urlStr);
    QNetworkRequest request(url);

    // 设置通用头
    request.setHeader(QNetworkRequest::UserAgentHeader, "CampusSecondhandClient/1.0");
    request.setRawHeader("Accept", "application/json");

    // 设置认证头
    if (!m_authToken.isEmpty()) {
        request.setRawHeader("Authorization", QString("Bearer %1").arg(m_authToken).toUtf8());
    }

    // 设置超时
    // request.setTransferTimeout(m_timeout);

    return request;
}

QJsonObject HttpClient::parseReply(QNetworkReply* reply)
{
    QJsonObject result;

    // 获取HTTP状态码
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    result["status_code"] = statusCode;

    // 读取响应数据
    QByteArray responseData = reply->readAll();

    if (reply->error() != QNetworkReply::NoError) {
        result["success"] = false;
        result["error"] = reply->errorString();
        return result;
    }

    // 解析JSON
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(responseData, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        result["success"] = false;
        result["error"] = "JSON parse error: " + parseError.errorString();
        result["raw_response"] = QString(responseData);
    } else if (doc.isObject()) {
        result = doc.object();
    } else if (doc.isArray()) {
        result["success"] = true;
        result["data"] = doc.array();
    } else {
        result["success"] = false;
        result["error"] = "Invalid response format";
    }

    return result;
}

QString HttpClient::buildQueryString(const QJsonObject& params) const
{
    QUrlQuery query;
    for (auto it = params.begin(); it != params.end(); ++it) {
        query.addQueryItem(it.key(), it.value().toString());
    }
    return query.toString();
}
