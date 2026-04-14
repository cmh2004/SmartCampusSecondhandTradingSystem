#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QListWidgetItem>
#include <QDateTime>
#include <QScrollBar>
#include <QFrame>
#include <QJsonArray>
#include <QMessageBox>
#include "..\apiservice.h"
#include "ReportSubmitDialog.h"
#include "MessagesPage.h"

MessagesPage::MessagesPage(QWidget *parent) : QWidget(parent) {
    chatData = QList<QMap<QString, QVariant>>();
    setupUI();
    loadChatHistory();
    connect(ApiService::instance(), &ApiService::newMessageReceived, this, &MessagesPage::onNewMessage);
}

void MessagesPage::openOrCreateChat(int goodsId, const QString &sellerName) {
    // 查找是否已经存在该商品的聊天
    int existingChatId = findChatByGoodsId(goodsId);

    if (existingChatId >= 0) {
        // 如果已存在，直接选中该聊天
        if (existingChatId < chatList->count()) {
            chatList->setCurrentRow(existingChatId);
            onChatItemClicked(chatList->item(existingChatId));
        }
    } else {
        // 创建新聊天
        createNewChat(goodsId, sellerName);
    }
}

int MessagesPage::findChatByGoodsId(int goodsId) {
    for (int i = 0; i < chatData.size(); i++) {
        if (chatData[i]["goodsId"].toInt() == goodsId) {
            return i;
        }
    }
    return -1;
}

void MessagesPage::createNewChat(int goodsId, const QString &sellerName) {
    // 创建聊天数据
    QMap<QString, QVariant> newChat;
    newChat["goodsId"] = goodsId;
    newChat["sellerName"] = sellerName;
    newChat["lastMessage"] = "开始对话";
    newChat["lastTime"] = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm");

    chatData.append(newChat);

    // 添加到聊天列表
    QString chatInfo = QString("%1 - 商品#%2\n%3").arg(sellerName).arg(goodsId).arg(newChat["lastTime"].toString());
    QListWidgetItem *item = new QListWidgetItem(chatInfo, chatList);
    item->setData(Qt::UserRole, sellerName);
    item->setData(Qt::UserRole + 1, goodsId);
    item->setData(Qt::UserRole+2, goodsId);

    // 自动选中新创建的聊天
    chatList->setCurrentItem(item);
    onChatItemClicked(item);
}

void MessagesPage::setupUI() {
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ========== 左侧：聊天列表容器 ==========
    QWidget *leftContainer = new QWidget();
    leftContainer->setFixedWidth(250);
    leftContainer->setStyleSheet("background-color: #f8fafc;");
    QVBoxLayout *listLayout = new QVBoxLayout(leftContainer);
    listLayout->setContentsMargins(0, 0, 0, 0);
    listLayout->setSpacing(0);

    QLabel *listTitle = new QLabel("💬 聊天列表");
    listTitle->setFixedHeight(60);
    listTitle->setStyleSheet(R"(
        QLabel {
            font-weight: bold;
            font-size: 16px;
            padding: 15px 20px;
            border-bottom: 1px solid #E2E8F0;
            background-color: #F8FAFC;
            color: #2D3748;
        }
    )");

    chatList = new QListWidget();
    chatList->setStyleSheet(R"(
        QListWidget {
            border: none;
            background-color: white;
            outline: none;
        }
        QListWidget::item {
            padding: 12px 15px;
            border-bottom: 1px solid #F1F5F9;
            color: #475569;
            outline: none;
        }
        QListWidget::item:hover {
            background-color: #F8FAFC;
            color: #3B82F6;
            outline: none;
        }
        QListWidget::item:selected {
            background-color: #EFF6FF;
            color: #1D4ED8;
            font-weight: 500;
            outline: none;
        }
        QListWidget::item:selected:hover {
            background-color: #DBEAFE;
            outline: none;
        }
    )");
    chatList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    listLayout->addWidget(listTitle);
    listLayout->addWidget(chatList, 1);

    // ========== 右侧：聊天区域容器 ==========
    QWidget *rightContainer = new QWidget();
    rightContainer->setStyleSheet("background-color: #f8fafc;");
    QVBoxLayout *chatLayout = new QVBoxLayout(rightContainer);
    chatLayout->setContentsMargins(0, 0, 0, 0);
    chatLayout->setSpacing(0);

    // 聊天头部
    QWidget *chatHeader = new QWidget();
    chatHeader->setFixedHeight(60);
    chatHeader->setStyleSheet(R"(
        background-color: #F8FAFC;
        border-bottom: 1px solid #E2E8F0;
    )");
    QHBoxLayout *headerLayout = new QHBoxLayout(chatHeader);
    headerLayout->setContentsMargins(20, 0, 20, 0);

    currentChatLabel = new QLabel("请选择一个聊天");
    currentChatLabel->setStyleSheet("font-weight: bold; font-size: 16px; color: #1E293B;");

    QPushButton *reportBtn = new QPushButton("举报");
    reportBtn->setObjectName("warningBtn");
    reportBtn->setFixedSize(70, 30);
    reportBtn->setEnabled(false);
    reportBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #fef2f2;
            color: #dc2626;
            border: 1px solid #fecaca;
            border-radius: 6px;
            font-size: 13px;
            font-weight: 500;
        }
        QPushButton:hover {
            background-color: #fee2e2;
            border-color: #fca5a5;
        }
        QPushButton:pressed {
            background-color: #fecaca;
        }
    )");

    headerLayout->addWidget(currentChatLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(reportBtn);

    // 消息列表（右侧气泡区域）
    messageListWidget = new QListWidget();
    messageListWidget->setStyleSheet(R"(
        QListWidget {
            border: none;
            background-color: #f8fafc;
            outline: none;
        }
        QListWidget::item {
            padding: 0;
            margin: 0;
            border: none;
        }
    )");

    // 消息输入区域
    QWidget *inputWidget = new QWidget();
    inputWidget->setMinimumHeight(130);
    inputWidget->setStyleSheet("background-color: #F8FAFC; border-top: 1px solid #E2E8F0;");
    QVBoxLayout *inputLayout = new QVBoxLayout(inputWidget);
    inputLayout->setContentsMargins(20, 15, 20, 15);

    messageEdit = new QLineEdit();
    messageEdit->setPlaceholderText("输入消息...");
    messageEdit->setEnabled(false);
    messageEdit->setMinimumHeight(36);
    messageEdit->setStyleSheet(R"(
        QLineEdit {
            border: 2px solid #E2E8F0;
            border-radius: 8px;
            padding: 8px 15px;
            font-size: 14px;
            background-color: white;
        }
        QLineEdit:focus {
            border-color: #3B82F6;
            outline: none;
        }
    )");

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setContentsMargins(0, 10, 0, 0);

    // QPushButton *fileBtn = new QPushButton("文件");
    // fileBtn->setObjectName("secondaryBtn");
    // fileBtn->setFixedSize(80, 30);
    // fileBtn->setEnabled(false);
    // fileBtn->setToolTip("发送文件");

    QPushButton *sendBtn = new QPushButton("发送");
    sendBtn->setObjectName("primaryBtn");
    sendBtn->setFixedSize(80, 30);
    sendBtn->setEnabled(false);

    // fileBtn->setStyleSheet(R"(
    //     QPushButton {
    //         background-color: #f1f5f9;
    //         color: #475569;
    //         border: 1px solid #e2e8f0;
    //         border-radius: 8px;
    //         font-size: 13px;
    //         font-weight: 500;
    //     }
    //     QPushButton:hover {
    //         background-color: #e2e8f0;
    //         border-color: #cbd5e1;
    //     }
    //     QPushButton:pressed {
    //         background-color: #cbd5e1;
    //     }
    // )");

    sendBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #3b82f6;
            color: white;
            border: none;
            border-radius: 8px;
            font-size: 14px;
            font-weight: 600;
        }
        QPushButton:hover {
            background-color: #2563eb;
        }
        QPushButton:pressed {
            background-color: #1d4ed8;
        }
        QPushButton:disabled {
            background-color: #cbd5e1;
            color: #94a3b8;
        }
    )");

    // btnLayout->addWidget(fileBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(sendBtn);

    inputLayout->addWidget(messageEdit);
    inputLayout->addLayout(btnLayout);

    chatLayout->addWidget(chatHeader);
    chatLayout->addWidget(messageListWidget, 1);
    chatLayout->addWidget(inputWidget);

    // 分割线
    QFrame *separator = new QFrame();
    separator->setFrameShape(QFrame::VLine);
    separator->setFrameShadow(QFrame::Sunken);
    separator->setLineWidth(1);
    separator->setMidLineWidth(0);
    separator->setStyleSheet("background-color: #e2e8f0; border: none;");

    // 添加到主布局
    mainLayout->addWidget(leftContainer);
    mainLayout->addWidget(separator);
    mainLayout->addWidget(rightContainer, 1);

    // 连接信号槽
    connect(chatList, &QListWidget::itemClicked, this, &MessagesPage::onChatItemClicked);
    connect(messageEdit, &QLineEdit::returnPressed, this, &MessagesPage::onSendMessage);
    connect(sendBtn, &QPushButton::clicked, this, &MessagesPage::onSendMessage);
    // connect(fileBtn, &QPushButton::clicked, this, [this]() {
    //     QMessageBox::information(this, "文件", "文件上传功能开发中...");
    // });
    connect(reportBtn, &QPushButton::clicked, this, &MessagesPage::onReportClicked);
}

void MessagesPage::loadChatHistory() {
    QJsonArray chats = ApiService::instance()->getChatList();
    chatList->clear();
    chatData.clear();
    for (const QJsonValue &val : chats) {
        QJsonObject chat = val.toObject();
        QString sessionId = chat.value("session_id").toString();
        QString lastMessage = chat.value("last_content").toString();
        QString lastTime = chat.value("last_time").toString();
        QString otherName = chat.value("other_name").toString();
        int goodsId = chat.value("goods_id").toInt();
        int otherId = chat.value("other_id").toInt();

        QMap<QString, QVariant> chatItem;
        chatItem["sessionId"] = sessionId;
        chatItem["otherName"] = otherName;
        chatItem["goodsId"] = goodsId;
        chatItem["lastMessage"] = lastMessage;
        chatItem["lastTime"] = lastTime;
        chatItem["otherId"] = otherId;
        chatData.append(chatItem);

        QString display = QString("%1 - 商品#%2\n%3\n%4").arg(otherName).arg(goodsId).arg(lastMessage).arg(lastTime);
        QListWidgetItem *item = new QListWidgetItem(display, chatList);
        item->setData(Qt::UserRole, sessionId);
        item->setData(Qt::UserRole+1, otherName);
        item->setData(Qt::UserRole+2, goodsId);
        item->setData(Qt::UserRole+3, otherId);
    }
}

void MessagesPage::addMessage(const QString &sender, const QString &message, bool isSelf, const QString &timestamp) {
    QString formattedTime;
    if (timestamp.isEmpty()) {
        formattedTime = QDateTime::currentDateTime().toString("HH:mm");
    } else {
        formattedTime = formatMessageTime(timestamp);
    }

    // 创建消息条目
    QListWidgetItem* item = new QListWidgetItem();
    messageListWidget->addItem(item);

    // 创建消息控件，传入格式化后的时间
    QWidget* msgWidget = createMessageWidget(sender, message, isSelf, formattedTime);

    item->setSizeHint(msgWidget->sizeHint());
    messageListWidget->setItemWidget(item, msgWidget);

    messageListWidget->scrollToBottom();
}

void MessagesPage::onSendMessage() {
    QString message = messageEdit->text().trimmed();
    if (message.isEmpty()) return;

    if (m_currentChatOtherId <= 0) {
        QMessageBox::warning(this, "错误", "无法获取接收者信息");
        return;
    }

    QListWidgetItem *currentItem = chatList->currentItem();
    if (!currentItem) return;

    QString sessionId = currentItem->data(Qt::UserRole).toString();
    QString receiverName = currentItem->data(Qt::UserRole+1).toString();
    int goodsId = currentItem->data(Qt::UserRole+2).toInt();

    QJsonObject result = ApiService::instance()->sendMessage(QString::number(m_currentChatOtherId), message, goodsId);
    if (result.value("success").toBool()) {
        // 在界面上添加自己发送的消息
        addMessage("我", message, true);
        messageEdit->clear();
    } else {
        QMessageBox::warning(this, "发送失败", result.value("error").toString());
    }
}

void MessagesPage::onChatItemClicked(QListWidgetItem *item) {
    if (!item) return;

    QString sessionId = item->data(Qt::UserRole).toString();   // 获取 sessionId
    QString chatWith = item->data(Qt::UserRole+1).toString(); // 对方昵称
    int goodsId = item->data(Qt::UserRole+2).toInt();         // 商品ID
    m_currentChatOtherId = item->data(Qt::UserRole + 3).toInt(); // 存储对方ID

    // 更新聊天头部
    currentChatLabel->setText(QString("与 %1 的对话 (商品#%2)").arg(chatWith).arg(goodsId));

    // 启用输入框和按钮
    messageEdit->setEnabled(true);
    messageEdit->setFocus();

    // 启用发送按钮
    QWidget *parent = this->parentWidget();
    if (parent) {
        QList<QPushButton*> buttons = parent->findChildren<QPushButton*>();
        for (QPushButton *btn : buttons) {
            if (btn->text() == "发送" || btn->text() == "文件" || btn->text() == "举报") {
                btn->setEnabled(true);
            }
        }
    }

    // 加载该聊天的消息历史
    loadChatMessages(sessionId, 1, 30);
}

void MessagesPage::loadChatMessages(const QString &sessionId, int page, int pageSize) {
    qDebug() << "loadChatMessages: sessionId=" << sessionId;
    QJsonArray messages = ApiService::instance()->getMessageHistory(sessionId, page, pageSize);
    qDebug() << "loadChatMessages: received messages count=" << messages.size();
    messageListWidget->clear();  // 清空消息列表
    // 注意：返回的消息可能是倒序的，需要反转或按时间正序添加
    for (int i = messages.size() - 1; i >= 0; --i) {
        QJsonObject msg = messages[i].toObject();
        qDebug()<<msg;
        int senderId = msg.value("sender_id").toInt();
        QString content = msg.value("content").toString();
        QString timestamp = msg.value("create_time").toString();
        bool isSelf = (senderId == ApiService::instance()->getCurrentUserId());
        addMessage(isSelf ? "我" : msg.value("sender_name").toString(), content, isSelf,timestamp);
    }
}

void MessagesPage::onNewMessage(const QJsonObject &message) {
    QString senderId = message.value("sender_id").toString();
    QString content = message.value("content").toString();
    QString sessionId = message.value("session_id").toString();
    // 检查当前打开的会话是否就是这个 sessionId
    QListWidgetItem *current = chatList->currentItem();
    if (current && current->data(Qt::UserRole).toString() == sessionId) {
        // 当前正在查看这个聊天，直接添加消息
        QString timestamp = message.value("timestamp").toString();
        addMessage(senderId, content, false,timestamp);
        // 标记已读（调用 API）
        ApiService::instance()->markMessageRead(sessionId);
    } else {
        // 否则更新聊天列表中的最后一条消息显示
        updateChatListLastMessage(sessionId, content);
    }
}

void MessagesPage::updateChatListLastMessage(const QString &sessionId, const QString &lastMessage) {
    for (int i = 0; i < chatList->count(); ++i) {
        QListWidgetItem *item = chatList->item(i);
        if (item->data(Qt::UserRole).toString() == sessionId) {
            QString otherName = item->data(Qt::UserRole+1).toString();
            int goodsId = item->data(Qt::UserRole+2).toInt();
            QString display = QString("%1 - 商品#%2\n%3\n%4")
                                  .arg(otherName)
                                  .arg(goodsId)
                                  .arg(lastMessage)
                                  .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm"));
            item->setText(display);
            break;
        }
    }
}

void MessagesPage::onReportClicked() {
    QListWidgetItem *currentItem = chatList->currentItem();
    if (!currentItem) {
        QMessageBox::warning(this, "提示", "请先选择一个聊天");
        return;
    }

    // 获取对方用户信息
    int otherUserId = currentItem->data(Qt::UserRole + 3).toInt(); // 需要存储对方ID
    QString otherName = currentItem->data(Qt::UserRole + 1).toString();

    if (otherUserId <= 0) {
        QMessageBox::warning(this, "提示", "无法获取举报对象信息");
        return;
    }

    ReportSubmitDialog *dialog = new ReportSubmitDialog(this, otherUserId, "user", otherName);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

QWidget* MessagesPage::createMessageWidget(const QString &senderName, const QString &message, bool isSelf, const QString &timestamp) {
    QWidget *container = new QWidget();
    QHBoxLayout *mainLayout = new QHBoxLayout(container);
    mainLayout->setContentsMargins(10, 8, 10, 8);
    mainLayout->setSpacing(8);

    // 气泡
    QWidget *bubble = new QWidget();
    bubble->setObjectName("bubble");
    QVBoxLayout *bubbleLayout = new QVBoxLayout(bubble);
    bubbleLayout->setContentsMargins(10, 8, 10, 8);
    bubbleLayout->setSpacing(4);

    // 昵称 + 时间（放在同一行）
    QHBoxLayout *infoLayout = new QHBoxLayout();
    QLabel *nameLabel = new QLabel(isSelf ? "我   " : senderName+"   ");
    nameLabel->setStyleSheet("font-size: 13px; font-weight: 500; color: #475569; background-color: transparent;");
    QLabel *timeLabel = new QLabel(timestamp);
    timeLabel->setStyleSheet("font-size: 11px; color: #94a3b8; background-color: transparent;");
    infoLayout->addWidget(nameLabel);
    infoLayout->addWidget(timeLabel);
    timeLabel->setAlignment(Qt::AlignLeft);
    timeLabel->setAlignment(Qt::AlignLeft);

    // 消息内容
    QLabel *msgLabel = new QLabel(message);
    msgLabel->setWordWrap(true);
    msgLabel->setStyleSheet("font-size: 14px; color: #1f2937; background-color: transparent;");
    msgLabel->setMaximumWidth(300);

    // 组装气泡内容
    bubbleLayout->addLayout(infoLayout);
    bubbleLayout->addWidget(msgLabel);

    // 根据 isSelf 设置气泡位置和颜色
    if (isSelf) {
        bubble->setStyleSheet(R"(
            QWidget#bubble {
                background-color: #dbeafe;
                border-radius: 14px;
                border-top-right-radius: 4px;
            }
        )");
        mainLayout->addStretch();
        mainLayout->addWidget(bubble);
    } else {
        bubble->setStyleSheet(R"(
            QWidget#bubble {
                background-color: #f3f4f6;
                border-radius: 14px;
                border-top-left-radius: 4px;
            }
        )");
        mainLayout->addWidget(bubble);
        mainLayout->addStretch();
    }
    return container;
}

QString MessagesPage::formatMessageTime(const QString &timestamp) {
    QDateTime msgTime = QDateTime::fromString(timestamp, "yyyy-MM-dd HH:mm:ss");
    if (!msgTime.isValid()) {
        // 尝试其他格式或返回当前时间
        return QDateTime::currentDateTime().toString("HH:mm");
    }

    QDate today = QDate::currentDate();
    QDate msgDate = msgTime.date();

    if (msgDate == today) {
        return msgTime.toString("HH:mm");
    } else if (msgDate == today.addDays(-1)) {
        return QString("昨天 %1").arg(msgTime.toString("HH:mm"));
    } else {
        // 显示月-日 时:分
        return msgTime.toString("MM-dd HH:mm");
    }
}
