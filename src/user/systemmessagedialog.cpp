#include "systemmessagedialog.h"
#include "..\apiservice.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDateTime>

SystemMessageDialog::SystemMessageDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    loadMessages();
    updateUnreadCount();
}

void SystemMessageDialog::setupUI()
{
    setWindowTitle("系统消息");
    setMinimumSize(800, 500);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 顶部工具栏
    QWidget *toolBar = new QWidget();
    QHBoxLayout *toolLayout = new QHBoxLayout(toolBar);
    toolLayout->setContentsMargins(0, 0, 0, 0);

    m_unreadLabel = new QLabel("未读消息: 0");
    m_unreadLabel->setStyleSheet("color: #e74c3c; font-weight: bold;");

    m_refreshBtn = new QPushButton("刷新");
    m_markReadBtn = new QPushButton("标记选中为已读");
    m_markAllReadBtn = new QPushButton("全部标记已读");

    m_refreshBtn->setObjectName("primaryBtn");
    m_markReadBtn->setObjectName("secondaryBtn");
    m_markAllReadBtn->setObjectName("secondaryBtn");

    toolLayout->addWidget(m_unreadLabel);
    toolLayout->addStretch();
    toolLayout->addWidget(m_refreshBtn);
    toolLayout->addWidget(m_markReadBtn);
    toolLayout->addWidget(m_markAllReadBtn);

    // 消息表格
    m_table = new QTableWidget(0, 5);
    m_table->setHorizontalHeaderLabels({"标题", "内容", "时间", "状态", "消息ID"});
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->setEditTriggers(QTableWidget::NoEditTriggers);
    m_table->setSelectionBehavior(QTableWidget::SelectRows);
    m_table->setAlternatingRowColors(true);
    m_table->verticalHeader()->setVisible(false);
    // 隐藏消息ID列（列索引4）
    m_table->setColumnHidden(4, true);
    // 设置列宽
    m_table->setColumnWidth(0, 150);
    m_table->setColumnWidth(1, 350);
    m_table->setColumnWidth(2, 180);
    m_table->setColumnWidth(3, 50);

    mainLayout->addWidget(toolBar);
    mainLayout->addWidget(m_table, 1);

    // 连接信号
    connect(m_refreshBtn, &QPushButton::clicked, this, &SystemMessageDialog::onRefresh);
    connect(m_markReadBtn, &QPushButton::clicked, this, &SystemMessageDialog::onMarkRead);
    connect(m_markAllReadBtn, &QPushButton::clicked, this, &SystemMessageDialog::onMarkAllRead);

    // 样式
    setStyleSheet(R"(
        QPushButton#primaryBtn {
            background-color: #3498db;
            color: white;
            border-radius: 4px;
            padding: 6px 12px;
        }
        QPushButton#secondaryBtn {
            background-color: #95a5a6;
            color: white;
            border-radius: 4px;
            padding: 6px 12px;
        }
        QPushButton:hover {
            opacity: 0.9;
        }
        QTableWidget {
            border: 1px solid #ddd;
            border-radius: 6px;
            background-color: white;
            gridline-color: #eee;
        }
        QTableWidget::item {
            padding: 8px;
        }
        QHeaderView::section {
            background-color: #f5f7fa;
            padding: 8px;
            border: none;
            border-bottom: 1px solid #ddd;
            font-weight: bold;
        }
    )");
}

void SystemMessageDialog::loadMessages()
{
    // 调用 ApiService 获取系统消息列表（不分页，取最近100条）
    QJsonArray messages = ApiService::instance()->getSystemMessages(false, 1, 100);
    m_table->setRowCount(0);

    for (const QJsonValue &val : messages) {
        QJsonObject obj = val.toObject();
        int row = m_table->rowCount();
        m_table->insertRow(row);
        m_table->setRowHeight(row, 45);

        QString title = obj.value("title").toString();
        QString content = obj.value("content").toString();
        QString createTime = obj.value("create_time").toString();
        int isRead = obj.value("is_read").toString().toInt();
        int msgId = obj.value("id").toInt();

        m_table->setItem(row, 0, new QTableWidgetItem(title));
        m_table->setItem(row, 1, new QTableWidgetItem(content));
        m_table->setItem(row, 2, new QTableWidgetItem(createTime));
        QTableWidgetItem *statusItem = new QTableWidgetItem(isRead ? "已读" : "未读");
        if (!isRead) {
            statusItem->setForeground(QColor(231, 76, 60)); // 红色
            statusItem->setFont(QFont("", -1, QFont::Bold));
        }
        m_table->setItem(row, 3, statusItem);
        m_table->setItem(row, 4, new QTableWidgetItem(QString::number(msgId)));
    }

    // 调整表格内容自适应
    m_table->resizeRowsToContents();
}

void SystemMessageDialog::onMarkRead()
{
    int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "提示", "请先选中一条消息");
        return;
    }

    int msgId = m_table->item(row, 4)->text().toInt();
    if (ApiService::instance()->markSystemMessageRead(msgId)) {
        loadMessages();      // 刷新列表
        updateUnreadCount(); // 更新未读数
    } else {
        QMessageBox::warning(this, "失败", "标记失败，请重试");
    }
}

void SystemMessageDialog::onMarkAllRead()
{
    // 获取所有未读消息ID
    QList<int> unreadIds;
    for (int row = 0; row < m_table->rowCount(); ++row) {
        QTableWidgetItem *statusItem = m_table->item(row, 3);
        if (statusItem && statusItem->text() == "未读") {
            int msgId = m_table->item(row, 4)->text().toInt();
            unreadIds.append(msgId);
        }
    }

    if (unreadIds.isEmpty()) {
        QMessageBox::information(this, "提示", "没有未读消息");
        return;
    }

    bool allSuccess = true;
    for (int msgId : unreadIds) {
        if (!ApiService::instance()->markSystemMessageRead(msgId)) {
            allSuccess = false;
        }
    }

    if (allSuccess) {
        loadMessages();
        updateUnreadCount();
        QMessageBox::information(this, "成功", "所有消息已标记为已读");
    } else {
        QMessageBox::warning(this, "失败", "部分消息标记失败，请重试");
    }
}

void SystemMessageDialog::onRefresh()
{
    loadMessages();
    updateUnreadCount();
}

void SystemMessageDialog::updateUnreadCount()
{
    int unreadCount = ApiService::instance()->getUnreadSystemMessageCount();
    m_unreadLabel->setText(QString("未读消息: %1").arg(unreadCount));
    if (unreadCount > 0) {
        m_unreadLabel->setStyleSheet("color: #e74c3c; font-weight: bold;");
    } else {
        m_unreadLabel->setStyleSheet("color: #2ecc71; font-weight: bold;");
    }
}
