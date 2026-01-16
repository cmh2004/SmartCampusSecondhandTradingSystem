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

signals:
    void sendMessage(const QString &receiver,const QString &message);
    void chatSelected(int chatId);

private slots:
    void onSendMessage();
    void onChatItemClicked(QListWidgetItem *item);

private:
    void setupUI();

    QListWidget *chatList;
    QTextEdit *chatArea;
    QLineEdit *messageEdit;
    QLabel *currentChatLabel;
};

#endif // MESSAGESPAGE_H
