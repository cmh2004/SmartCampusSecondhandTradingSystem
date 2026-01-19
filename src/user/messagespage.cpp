#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QListWidgetItem>
#include <QDateTime>
#include <QScrollBar>
#include <QFrame>
#include <QMessageBox>
#include "MessagesPage.h"

MessagesPage::MessagesPage(QWidget *parent) : QWidget(parent) {
    chatData = QList<QMap<QString, QVariant>>();
    setupUI();
    loadChatHistory();
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

    // 自动选中新创建的聊天
    chatList->setCurrentItem(item);
    onChatItemClicked(item);
}

void MessagesPage::setupUI() {
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 左侧聊天列表
    QWidget *chatListWidget = new QWidget();
    chatListWidget->setFixedWidth(250);
    chatListWidget->setStyleSheet("background-color: #f8fafc;");
    QVBoxLayout *listLayout = new QVBoxLayout(chatListWidget);
    listLayout->setContentsMargins(0, 0, 0, 0);
    listLayout->setSpacing(0);

    // 列表标题
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

    // 右侧聊天区域
    QWidget *chatAreaWidget = new QWidget();
     chatAreaWidget->setStyleSheet("background-color: #f8fafc;");
    QVBoxLayout *chatLayout = new QVBoxLayout(chatAreaWidget);
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
    currentChatLabel->setStyleSheet(R"(
        font-weight: bold;
        font-size: 16px;
        color: #1E293B;
    )");

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

    // 聊天消息区域
    chatArea = new QTextEdit();
    chatArea->setReadOnly(true);
    chatArea->setStyleSheet(R"(
        QTextEdit {
            border: none;
            background-color: white;
            font-size: 14px;
            color: #334155;
            border-radius: 0 0 12px 0;
            padding: 20px;
        }
    )");

    chatArea->setText(R"(
        <div style="text-align: center; padding: 60px 20px; color: #94a3b8; font-size: 15px;">
            <div style="font-size: 48px; margin-bottom: 15px;">💬</div>
            <div style="font-weight: 500; color: #64748b; margin-bottom: 8px;">选择一个聊天开始对话</div>
            <div style="color: #94a3b8; font-size: 13px;">与对方进行安全、便捷的沟通</div>
        </div>
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

    QPushButton *fileBtn = new QPushButton("文件");
    fileBtn->setObjectName("secondaryBtn");
    fileBtn->setFixedSize(80, 30);
    fileBtn->setEnabled(false);
    fileBtn->setToolTip("发送文件");

    QPushButton *sendBtn = new QPushButton("发送");
    sendBtn->setObjectName("primaryBtn");
    sendBtn->setFixedSize(80, 30);
    sendBtn->setEnabled(false);

    fileBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #f1f5f9;
            color: #475569;
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            font-size: 13px;
            font-weight: 500;
        }
        QPushButton:hover {
            background-color: #e2e8f0;
            border-color: #cbd5e1;
        }
        QPushButton:pressed {
            background-color: #cbd5e1;
        }
    )");

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

    btnLayout->addWidget(fileBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(sendBtn);

    inputLayout->addWidget(messageEdit);
    inputLayout->addLayout(btnLayout);

    chatLayout->addWidget(chatHeader);
    chatLayout->addWidget(chatArea, 1);
    chatLayout->addWidget(inputWidget);

    QFrame *separator = new QFrame();
    separator->setFrameShape(QFrame::VLine);  // 设置为垂直线
    separator->setFrameShadow(QFrame::Sunken);  // 设置阴影效果
    separator->setLineWidth(1);  // 设置线宽
    separator->setMidLineWidth(0);  // 设置中线宽度
    separator->setStyleSheet("background-color: #e2e8f0; border: none;");  // 设置颜色

    mainLayout->addWidget(chatListWidget);
    mainLayout->addWidget(separator);
    mainLayout->addWidget(chatAreaWidget, 1);

    // 连接信号槽
    connect(chatList, &QListWidget::itemClicked, this, &MessagesPage::onChatItemClicked);
    connect(messageEdit, &QLineEdit::returnPressed, this, &MessagesPage::onSendMessage);
    connect(sendBtn, &QPushButton::clicked, this, &MessagesPage::onSendMessage);
    connect(fileBtn, &QPushButton::clicked, this, [this]() {
        QMessageBox::information(this, "文件", "文件上传功能开发中...");
    });
}

void MessagesPage::loadChatHistory() {
    // 清空现有数据
    chatList->clear();
    chatData.clear();

    // 模拟从数据库加载现有聊天数据
    // 这里可以添加从数据库加载的逻辑

    // 模拟数据
    QList<QStringList> existingChats = {
        {"王五同学", "1001", "2024-03-20 10:30", "你好，这个手机还在吗？"},
        {"李四同学", "1002", "2024-03-18 14:20", "书我已经收到了，很满意！"}
    };

    for (const auto &data : existingChats) {
        QMap<QString, QVariant> chat;
        chat["sellerName"] = data[0];
        chat["goodsId"] = data[1].toInt();
        chat["lastTime"] = data[2];
        chat["lastMessage"] = data[3];

        chatData.append(chat);

        QString chatInfo = QString("%1 - 商品#%2\n%3").arg(data[0]).arg(data[1]).arg(data[2]);
        QListWidgetItem *item = new QListWidgetItem(chatInfo, chatList);
        item->setData(Qt::UserRole, data[0]);
        item->setData(Qt::UserRole + 1, data[1].toInt());
    }
}

void MessagesPage::addMessage(const QString &sender, const QString &message, bool isSelf) {
    QString time = QDateTime::currentDateTime().toString("HH:mm");
    QString senderName = isSelf ? "我" : sender;

    // 使用HTML格式化消息
    QString htmlMessage = QString(
                              "<div style='margin-bottom: 15px;'>"
                              "  <div style='font-size: 12px; color: #64748B; margin-bottom: 3px;'>"
                              "    %1 · %2"
                              "  </div>"
                              "  <div style='%3 padding: 10px 15px; border-radius: 8px; "
                              "        max-width: 70%%; word-wrap: break-word;'>"
                              "    %4"
                              "  </div>"
                              "</div>"
                              ).arg(
                                  senderName,
                                  time,
                                  isSelf ? "background-color: #EFF6FF; color: #1E40AF; margin-left: auto;"
                                         : "background-color: #F1F5F9; color: #334155; margin-right: auto;",
                                  message.toHtmlEscaped()
                                  );

    // 添加消息到聊天区域
    chatArea->append(htmlMessage);

    // 滚动到底部
    QScrollBar *scrollbar = chatArea->verticalScrollBar();
    scrollbar->setValue(scrollbar->maximum());
}

void MessagesPage::onSendMessage() {
    QString message = messageEdit->text().trimmed();
    if (message.isEmpty()) {
        return;
    }

    // 获取当前选中的聊天
    QListWidgetItem *currentItem = chatList->currentItem();
    if (!currentItem) {
        return;
    }

    QString receiver = currentItem->data(Qt::UserRole).toString();

    // 添加消息到界面
    addMessage(receiver, message, true);

    // 发射信号用于网络发送
    emit sendMessage(receiver, message);

    // 清空输入框
    messageEdit->clear();
}

void MessagesPage::onChatItemClicked(QListWidgetItem *item) {
    if (!item) return;

    QString chatWith = item->data(Qt::UserRole).toString();
    int goodsId = item->data(Qt::UserRole + 1).toInt();

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

    // 清空并加载聊天历史
    chatArea->clear();

    // 加载该聊天的消息历史
    loadChatMessages(chatList->row(item));

    // 发射信号通知聊天被选中
    emit chatSelected(goodsId);
}

void MessagesPage::loadChatMessages(int chatIndex) {
    if (chatIndex < 0 || chatIndex >= chatData.size()) {
        return;
    }

    // 模拟加载聊天消息
    // 在实际应用中，这里应该从数据库加载该聊天的所有消息

    QString sellerName = chatData[chatIndex]["sellerName"].toString();

    // 添加模拟消息
    addMessage(sellerName, "你好，我想咨询一下商品详情。", false);
    addMessage("我", "有什么问题请尽管问。", true);
    addMessage(sellerName, "这个商品还能便宜点吗？", false);
    addMessage("我", "价格已经是最低了，可以包邮哦。", true);

    // 如果有最后一条消息，显示它
    QString lastMessage = chatData[chatIndex]["lastMessage"].toString();
    if (!lastMessage.isEmpty() && lastMessage != "开始对话") {
        addMessage(sellerName, lastMessage, false);
    }
}
