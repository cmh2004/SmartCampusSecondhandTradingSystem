#include <QApplication>
#include <QDateTime>
#include <QListWidgetItem>
#include <QScrollBar>
#include <QInputDialog>
#include <QMessageBox>
#include "chatdialog.h"

ChatDialog::ChatDialog(QWidget *parent, int goodsId, QString sellerId)
    : QDialog(parent), goodsId(goodsId), sellerId(sellerId) {
    setWindowTitle("聊天窗口");
    setMinimumSize(600, 500);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    setupUI();
    loadChatHistory();

    // 初始化定时器
    riskAnalysisTimer = new QTimer(this);
    connect(riskAnalysisTimer, &QTimer::timeout, this, &ChatDialog::analyzeChatForRisk);
    riskAnalysisTimer->start(30000); // 每30秒分析一次
}

void ChatDialog::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 风险提醒区域
    riskWidget = new QWidget();
    riskWidget->setVisible(false);
    riskWidget->setStyleSheet("background-color: #fff3cd; border: 1px solid #ffeaa7; border-radius: 4px; padding: 8px;");

    QHBoxLayout *riskLayout = new QHBoxLayout(riskWidget);
    QLabel *riskIcon = new QLabel("⚠️");
    riskIcon->setStyleSheet("font-size: 18px;");
    riskLabel = new QLabel("检测到风险对话");
    riskLabel->setStyleSheet("color: #856404; font-weight: bold;");

    QPushButton *riskCloseBtn = new QPushButton("×");
    riskCloseBtn->setFixedSize(20, 20);
    riskCloseBtn->setStyleSheet("border: none; color: #856404; font-size: 14px;");
    connect(riskCloseBtn, &QPushButton::clicked, [this]() {
        riskWidget->setVisible(false);
    });

    riskLayout->addWidget(riskIcon);
    riskLayout->addWidget(riskLabel, 1);
    riskLayout->addWidget(riskCloseBtn);

    mainLayout->addWidget(riskWidget);

    // 聊天区域
    QHBoxLayout *chatLayout = new QHBoxLayout();
    chatLayout->setContentsMargins(0, 0, 0, 0);
    chatLayout->setSpacing(5);

    // 左侧聊天列表
    QWidget *leftPanel = new QWidget();
    leftPanel->setFixedWidth(200);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);

    QLabel *listTitle = new QLabel("聊天列表");
    listTitle->setStyleSheet("font-weight: bold; margin-bottom: 10px;");

    chatList = new QListWidget();
    chatList->addItem("与卖家的对话");
    chatList->addItem("系统消息");
    chatList->setCurrentRow(0);

    leftLayout->addWidget(listTitle);
    leftLayout->addWidget(chatList);

    // 右侧聊天区域
    QWidget *rightPanel = new QWidget();
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);

    messageDisplay = new QTextEdit();
    messageDisplay->setReadOnly(true);
    messageDisplay->setMinimumHeight(300);

    // 添加一些示例消息
    messageDisplay->append("<div style='color: #666; text-align: center;'>--- 开始对话 ---</div>");
    messageDisplay->append("<div style='background-color: #e3f2fd; padding: 8px; border-radius: 10px; margin: 5px 50px 5px 5px;'>你好，这个手机还在吗？</div>");
    messageDisplay->append("<div style='background-color: #f5f5f5; padding: 8px; border-radius: 10px; margin: 5px 5px 5px 50px; text-align: right;'>还在的，需要的话可以联系我</div>");

    // 输入区域
    QWidget *inputArea = new QWidget();
    QVBoxLayout *inputLayout = new QVBoxLayout(inputArea);

    messageEdit = new QTextEdit();
    messageEdit->setMaximumHeight(80);
    messageEdit->setPlaceholderText("输入消息...");

    // 议价区域
    QWidget *offerArea = new QWidget();
    QHBoxLayout *offerLayout = new QHBoxLayout(offerArea);
    offerLayout->setContentsMargins(0, 5, 0, 5);

    priceOfferEdit = new QLineEdit();
    priceOfferEdit->setPlaceholderText("输入议价金额");

    priceOfferBtn = new QPushButton("发送议价");
    priceOfferBtn->setObjectName("secondaryBtn");
    connect(priceOfferBtn, &QPushButton::clicked, this, &ChatDialog::makePriceOffer);

    offerLayout->addWidget(new QLabel("议价:"));
    offerLayout->addWidget(priceOfferEdit, 1);
    offerLayout->addWidget(priceOfferBtn);

    // 按钮区域
    QWidget *buttonArea = new QWidget();
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonArea);

    sendBtn = new QPushButton("发送");
    sendBtn->setObjectName("primaryBtn");

    aiAssistantBtn = new QPushButton("AI助手");
    aiAssistantBtn->setObjectName("secondaryBtn");

    riskAnalysisBtn = new QPushButton("风险分析");
    riskAnalysisBtn->setObjectName("warningBtn");

    QPushButton *reportUserBtn = new QPushButton("举报用户");
    reportUserBtn->setObjectName("warningBtn");

    // 连接信号
    connect(reportUserBtn, &QPushButton::clicked, [this]() {
        emit reportUserRequested(sellerId);
    });

    buttonLayout->addWidget(aiAssistantBtn);
    buttonLayout->addWidget(riskAnalysisBtn);
    buttonLayout->addWidget(reportUserBtn);
    buttonLayout->addStretch();
    buttonLayout->addWidget(sendBtn);

    inputLayout->addWidget(messageEdit);
    inputLayout->addWidget(offerArea);
    inputLayout->addWidget(buttonArea);

    rightLayout->addWidget(messageDisplay, 1);
    rightLayout->addWidget(inputArea);

    chatLayout->addWidget(leftPanel);
    chatLayout->addWidget(rightPanel, 1);

    mainLayout->addLayout(chatLayout, 1);

    // 连接信号槽
    connect(sendBtn, &QPushButton::clicked, this, &ChatDialog::sendMessage);
    connect(aiAssistantBtn, &QPushButton::clicked, this, &ChatDialog::onAIChatAssistant);
    connect(riskAnalysisBtn, &QPushButton::clicked, this, &ChatDialog::analyzeChatForRisk);

    // 样式
    setStyleSheet(R"(
        QListWidget {
            border: 1px solid #ddd;
            border-radius: 4px;
        }
        QListWidget::item {
            padding: 10px;
            border-bottom: 1px solid #eee;
        }
        QListWidget::item:selected {
            background-color: #e3f2fd;
        }
        QTextEdit {
            border: 1px solid #ddd;
            border-radius: 4px;
            padding: 5px;
        }
        #primaryBtn {
            background-color: #3498db;
            color: white;
            border-radius: 4px;
            padding: 8px 16px;
        }
        #secondaryBtn {
            background-color: #ecf0f1;
            color: #34495e;
            border-radius: 4px;
            padding: 6px 12px;
        }
        #warningBtn {
            background-color: #e74c3c;
            color: white;
            border-radius: 4px;
            padding: 6px 12px;
        }
    )");
}

void ChatDialog::loadChatHistory() {
    // 加载聊天历史
    // 这里应该从数据库加载
}

void ChatDialog::sendMessage() {
    QString message = messageEdit->toPlainText().trimmed();
    if (message.isEmpty()) {
        return;
    }

    // 保存消息
    saveChatMessage("我", message);

    // 清空输入框
    messageEdit->clear();

    // 这里可以添加发送到服务器的逻辑
    QMessageBox::information(this, "发送成功", "消息已发送");
}

void ChatDialog::makePriceOffer() {
    QString price = priceOfferEdit->text().trimmed();
    if (price.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入议价金额");
        return;
    }

    // 保存议价消息
    saveChatMessage("我", QString("议价: ¥%1").arg(price), true);
    priceOfferEdit->clear();

    QMessageBox::information(this, "议价成功", QString("已向卖家发送议价: ¥%1").arg(price));
}

void ChatDialog::analyzeChatForRisk() {
    // 模拟风险分析
    // 这里可以调用AI服务分析聊天内容
    QString riskType = "价格欺诈风险";
    QString suggestion = "检测到异常议价行为，建议谨慎交易";

    onRiskDetected(riskType, suggestion);
}

void ChatDialog::onAIChatAssistant() {
    QStringList suggestions = {
        "建议询问商品详细情况",
        "建议确认交易方式",
        "建议要求更多图片",
        "建议询问保修情况"
    };

    QString suggestion = QInputDialog::getItem(this, "AI助手建议",
                                               "选择建议:", suggestions, 0, false);

    if (!suggestion.isEmpty()) {
        saveChatMessage("AI助手", suggestion);
    }
}

void ChatDialog::onRiskDetected(const QString &riskType, const QString &suggestion) {
    // 显示风险提醒
    riskWidget->setVisible(true);
    riskLabel->setText(QString("%1 - %2").arg(riskType).arg(suggestion));

    addRiskWarning(QString("系统检测到风险: %1").arg(riskType));
}

void ChatDialog::saveChatMessage(const QString &sender, const QString &message, bool isPriceOffer) {
    QString time = QDateTime::currentDateTime().toString("hh:mm");
    QString formattedMessage;

    if (isPriceOffer) {
        formattedMessage = QString("[%1] %2: <span style='color: #e67e22;'>%3</span>").arg(time).arg(sender).arg(message);
    } else {
        formattedMessage = QString("[%1] %2: %3").arg(time).arg(sender).arg(message);
    }

    // 这里应该保存到数据库
    // 暂时只是打印
    qDebug() << "Chat message saved:" << formattedMessage;
}

void ChatDialog::addRiskWarning(const QString &warning) {
    // 添加风险警告到聊天记录
    QString time = QDateTime::currentDateTime().toString("hh:mm");
    QString formattedWarning = QString("[%1] <span style='color: #e74c3c;'>⚠️ %2</span>").arg(time).arg(warning);

    // 这里应该添加到聊天显示
    qDebug() << "Risk warning:" << formattedWarning;
}
