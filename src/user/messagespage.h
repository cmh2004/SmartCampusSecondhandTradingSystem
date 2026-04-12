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
    void addMessage(const QString &sender, const QString &message, bool isSelf = false);
    void openOrCreateChat(int goodsId, const QString &sellerName);

private slots:
    void onSendMessage();
    void onChatItemClicked(QListWidgetItem *item);
    void onNewMessage(const QJsonObject &message);

private:
    void setupUI();
    void createNewChat(int goodsId, const QString &sellerName);
    int findChatByGoodsId(int goodsId);
    void loadChatMessages(const QString &sessionId, int page=1, int pageSize=10);
    void updateChatListLastMessage(const QString &sessionId, const QString &lastMessage);

    QListWidget *chatList;
    QTextEdit *chatArea;
    QLineEdit *messageEdit;
    QLabel *currentChatLabel;

    QList<QMap<QString, QVariant>> chatData;
};

#endif // MESSAGESPAGE_H
