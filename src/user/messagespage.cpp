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
    setupUI();
    loadChatHistory();
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

    // 模拟聊天数据
    QList<QStringList> chatData = {
        {"张三同学", "二手iPhone 12", "2024-03-20 10:30", "你好，这个手机还在吗？"},
        {"李四同学", "大学物理教材", "2024-03-18 14:20", "书我已经收到了，很满意！"},
        {"王五同学", "篮球鞋 Nike Air", "2024-03-19 16:45", "可以便宜一点吗？"},
        {"赵六同学", "笔记本电脑戴尔", "2024-03-15 09:15", "电脑配置能再详细说一下吗？"},
        {"钱七同学", "小米手环6", "2024-03-12 11:20", "手环还在保修期内吗？"},
        {"孙八同学", "吉他雅马哈", "2024-03-10 08:45", "吉他的琴弦是新的吗？"}
    };

    for (const auto &data : chatData) {
        QString chatInfo = QString("%1 - %2\n%3").arg(data[0], data[1], data[2]);
        QListWidgetItem *item = new QListWidgetItem(chatInfo, chatList);
        item->setData(Qt::UserRole, data[0]); // 存储对方用户名
        item->setData(Qt::UserRole + 1, data[3]); // 存储最后一条消息
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
    QString lastMessage = item->data(Qt::UserRole + 1).toString();

    // 更新聊天头部
    currentChatLabel->setText(QString("与 %1 的对话").arg(chatWith));

    // 启用输入框和按钮
    messageEdit->setEnabled(true);
    messageEdit->setFocus();

    // 启用发送按钮（通过查找父窗口的按钮）
    QWidget *parent = this->parentWidget();
    if (parent) {
        QList<QPushButton*> buttons = parent->findChildren<QPushButton*>();
        for (QPushButton *btn : buttons) {
            if (btn->text() == "发送"||btn->text() == "文件"||btn->text() == "举报") {
                btn->setEnabled(true);
            }
        }
    }

    // 清空并加载聊天历史
    chatArea->clear();
    chatArea->setText("");

    // 添加模拟的聊天历史
    addMessage(chatWith, "你好，我想咨询一下商品详情。", false);
    addMessage(chatWith, lastMessage, false);
    addMessage(chatWith, "这个商品还能便宜点吗？", false);
    addMessage("我", "你好，有什么可以帮你的？", true);
    addMessage("我", "价格已经是最低了，可以包邮哦。", true);

    // 发射信号通知聊天被选中
    int chatId = chatList->row(item);
    emit chatSelected(chatId);
}
