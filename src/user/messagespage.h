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

signals:
    void sendMessage(const QString &receiver,const QString &message);
    void chatSelected(int chatId);

private slots:
    void onSendMessage();
    void onChatItemClicked(QListWidgetItem *item);

private:
    void setupUI();
    void createNewChat(int goodsId, const QString &sellerName);
    int findChatByGoodsId(int goodsId);
    void loadChatMessages(int chatIndex);

    QListWidget *chatList;
    QTextEdit *chatArea;
    QLineEdit *messageEdit;
    QLabel *currentChatLabel;

    QList<QMap<QString, QVariant>> chatData;
};

#endif // MESSAGESPAGE_H
