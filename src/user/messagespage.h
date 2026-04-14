#ifndef MESSAGESPAGE_H
#define MESSAGESPAGE_H

#include <QWidget>
#include <QListWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QLabel>

class MessagesPage : public QWidget {
    Q_OBJECT

public:
    explicit MessagesPage(QWidget *parent = nullptr);
    void loadChatHistory();
    void addMessage(const QString &sender, const QString &message, bool isSelf = false, const QString &timestamp = "");
    void openOrCreateChat(int goodsId, const QString &sellerName, int sellerId);

private slots:
    void onSendMessage();
    void onChatItemClicked(QListWidgetItem *item);
    void onNewMessage(const QJsonObject &message);
    void onReportClicked();

private:
    void setupUI();
    void createNewChat(int goodsId, const QString &sellerName, int otherId);
    int findChatByGoodsId(int goodsId);
    void loadChatMessages(const QString &sessionId, int page=1, int pageSize=10);
    void updateChatListLastMessage(const QString &sessionId, const QString &lastMessage);
    QWidget* createMessageWidget(const QString &senderName, const QString &message, bool isSelf, const QString &timestamp);
    QString formatMessageTime(const QString &timestamp);

    QListWidget *chatList;
    QListWidget *messageListWidget;
    QLineEdit *messageEdit;
    QLabel *currentChatLabel;

    int m_currentChatOtherId = -1;

    QList<QMap<QString, QVariant>> chatData;
};

#endif // MESSAGESPAGE_H
