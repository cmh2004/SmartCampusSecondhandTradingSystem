#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QMessageBox>
#include <QDateTime>
#include <QTableWidgetItem>
#include "OrdersPage.h"

OrdersPage::OrdersPage(QWidget *parent) : QWidget(parent) {
    setupUI();
    loadOrderExamples();
}

void OrdersPage::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // 订单筛选区域
    QWidget *filterWidget = new QWidget();
    QHBoxLayout *filterLayout = new QHBoxLayout(filterWidget);
    filterLayout->setContentsMargins(0, 0, 0, 0);
    filterLayout->setSpacing(12);

    // 状态筛选
    QLabel *statusLabel = new QLabel("订单状态:");
    statusLabel->setStyleSheet("color: #475569;");

    statusCombo = new QComboBox();
    statusCombo->addItems({"全部订单", "待付款", "待发货", "待收货", "已完成", "已取消", "纠纷处理中"});
    statusCombo->setFixedWidth(120);
    statusCombo->setStyleSheet(R"(
        QComboBox {
            border: 2px solid #E2E8F0;
            border-radius: 6px;
            padding: 5px 10px;
            background-color: white;
            font-size: 13px;
        }
        QComboBox:hover {
            border-color: #CBD5E1;
        }
        QComboBox::drop-down {
            border: none;
            width: 20px;
        }
    )");

    // 搜索框
    orderSearchEdit = new QLineEdit();
    orderSearchEdit->setPlaceholderText("搜索订单号、商品名称、卖家...");
    orderSearchEdit->setMinimumHeight(36);
    orderSearchEdit->setStyleSheet(R"(
        QLineEdit {
            border: 2px solid #E2E8F0;
            border-radius: 8px;
            padding: 8px 15px;
            font-size: 13px;
            background-color: white;
        }
        QLineEdit:focus {
            border-color: #3B82F6;
            outline: none;
        }
        QLineEdit::placeholder {
            color: #94A3B8;
        }
    )");

    // 筛选按钮
    filterBtn = new QPushButton("筛选");
    filterBtn->setObjectName("primaryBtn");
    filterBtn->setFixedSize(80, 36);

    refreshBtn = new QPushButton("刷新");
    refreshBtn->setObjectName("secondaryBtn");
    refreshBtn->setFixedSize(80, 36);

    filterLayout->addWidget(statusLabel);
    filterLayout->addWidget(statusCombo);
    filterLayout->addSpacing(20);
    filterLayout->addWidget(orderSearchEdit, 1);
    filterLayout->addWidget(filterBtn);
    filterLayout->addWidget(refreshBtn);

    mainLayout->addWidget(filterWidget);

    // 订单表格
    ordersTable = new QTableWidget(0, 7);
    ordersTable->setHorizontalHeaderLabels({"订单号", "商品", "价格", "状态", "下单时间", "卖家", "操作"});

    // 设置表格样式
    ordersTable->setStyleSheet(R"(
        QTableWidget {
            border: 1px solid #E2E8F0;
            border-radius: 8px;
            background-color: white;
            gridline-color: transparent;
        }
        QTableWidget::item {
            padding: 12px 8px;
            border-bottom: 1px solid #F1F5F9;
            font-size: 13px;
        }
        QTableWidget::item:selected {
            background-color: #EFF6FF;
            color: #1E40AF;
            border-radius: 4px;
        }
        QHeaderView::section {
            background-color: #F8FAFC;
            border: none;
            border-bottom: 2px solid #E2E8F0;
            padding: 14px 8px;
            color: #475569;
            font-size: 13px;
            font-weight: 600;
        }
        QHeaderView::section:first {
            border-top-left-radius: 8px;
        }
        QHeaderView::section:last {
            border-top-right-radius: 8px;
        }
    )");

    // 设置列宽
    ordersTable->setColumnWidth(0, 100);  // 订单号
    ordersTable->setColumnWidth(1, 250);  // 商品
    ordersTable->setColumnWidth(2, 110);  // 价格
    ordersTable->setColumnWidth(3, 110);  // 状态
    ordersTable->setColumnWidth(4, 150);  // 下单时间
    ordersTable->setColumnWidth(5, 130);  // 卖家
    ordersTable->horizontalHeader()->setStretchLastSection(true);  // 操作列自适应

    ordersTable->verticalHeader()->setVisible(false);
    ordersTable->setAlternatingRowColors(true);
    ordersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ordersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    mainLayout->addWidget(ordersTable, 1);

    // 底部统计信息
    QWidget *statsWidget = new QWidget();
    QHBoxLayout *statsLayout = new QHBoxLayout(statsWidget);
    statsLayout->setContentsMargins(0, 10, 0, 0);

    QLabel *totalLabel = new QLabel("共 6 个订单");
    totalLabel->setStyleSheet("color: #64748B; font-size: 13px;");

    statsLayout->addWidget(totalLabel);
    statsLayout->addStretch();

    exportBtn = new QPushButton("导出订单");
    exportBtn->setObjectName("primaryBtn");
    exportBtn->setFixedSize(100, 30);
    statsLayout->addWidget(exportBtn);

    mainLayout->addWidget(statsWidget);

    // 连接信号槽
    connect(filterBtn, &QPushButton::clicked, this, &OrdersPage::onFilterOrders);
    connect(refreshBtn, &QPushButton::clicked, this, &OrdersPage::onRefreshOrders);
    connect(exportBtn, &QPushButton::clicked, this, &OrdersPage::exportOrdersRequested);
    connect(statusCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &OrdersPage::onFilterOrders);
    connect(orderSearchEdit, &QLineEdit::returnPressed,
            this, &OrdersPage::onFilterOrders);
}

void OrdersPage::loadOrderExamples() {
    if (!ordersTable) return;

    ordersTable->setRowCount(0);

    // 订单数据
    QList<QList<QVariant>> orderData = {
        {1001, "二手iPhone 12 128GB", 2500.00, "待付款", "2024-03-20 10:30", "张三同学"},
        {1002, "大学物理教材", 35.00, "已完成", "2024-03-18 14:20", "李四同学"},
        {1003, "篮球鞋 Nike Air", 280.00, "待收货", "2024-03-19 16:45", "王五同学"},
        {1004, "笔记本电脑戴尔", 3200.00, "已完成", "2024-03-15 09:15", "赵六同学"},
        {1005, "小米手环6", 150.00, "已取消", "2024-03-12 11:20", "钱七同学"},
        {1006, "吉他雅马哈", 800.00, "纠纷处理中", "2024-03-10 08:45", "孙八同学"}
    };

    for (const auto &data : orderData) {
        int row = ordersTable->rowCount();
        ordersTable->insertRow(row);
        ordersTable->setRowHeight(row, 50);

        // 订单号
        QTableWidgetItem *orderIdItem = new QTableWidgetItem(QString::number(data[0].toInt()));
        orderIdItem->setTextAlignment(Qt::AlignCenter);
        orderIdItem->setData(Qt::UserRole, data[0].toInt()); // 存储订单ID
        ordersTable->setItem(row, 0, orderIdItem);

        // 商品名称
        ordersTable->setItem(row, 1, new QTableWidgetItem(data[1].toString()));

        // 价格
        QTableWidgetItem *priceItem = new QTableWidgetItem(QString("¥%1").arg(data[2].toDouble(), 0, 'f', 2));
        priceItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        priceItem->setFont(QFont("Arial", -1, QFont::Bold));
        ordersTable->setItem(row, 2, priceItem);

        // 状态
        QString status = data[3].toString();
        QTableWidgetItem *statusItem = new QTableWidgetItem(status);
        statusItem->setTextAlignment(Qt::AlignCenter);
        statusItem->setFont(QFont("Microsoft YaHei", -1, QFont::Bold));

        // 根据状态设置颜色
        if (status == "待付款") {
            statusItem->setForeground(QColor(231, 76, 60));  // 红色
            statusItem->setBackground(QColor(253, 237, 236)); // 浅红色背景
        } else if (status == "已完成") {
            statusItem->setForeground(QColor(46, 204, 113));  // 绿色
            statusItem->setBackground(QColor(237, 247, 240)); // 浅绿色背景
        } else if (status == "待收货") {
            statusItem->setForeground(QColor(241, 196, 15));  // 黄色
            statusItem->setBackground(QColor(254, 252, 232)); // 浅黄色背景
        } else if (status == "已取消") {
            statusItem->setForeground(QColor(149, 165, 166)); // 灰色
            statusItem->setBackground(QColor(245, 246, 246)); // 浅灰色背景
        } else if (status == "纠纷处理中") {
            statusItem->setForeground(QColor(230, 126, 34));  // 橙色
            statusItem->setBackground(QColor(253, 237, 236)); // 浅红色背景
        }

        ordersTable->setItem(row, 3, statusItem);

        // 下单时间
        ordersTable->setItem(row, 4, new QTableWidgetItem(data[4].toString()));

        // 卖家
        QTableWidgetItem *sellerItem = new QTableWidgetItem(data[5].toString());
        sellerItem->setTextAlignment(Qt::AlignCenter);
        ordersTable->setItem(row, 5, sellerItem);

        // 操作按钮
        createActionButtons(row, status, data[0].toInt());
    }
}

void OrdersPage::createActionButtons(int row, const QString &status, int orderId) {
    QWidget *actionWidget = new QWidget();
    QHBoxLayout *actionLayout = new QHBoxLayout(actionWidget);
    actionLayout->setContentsMargins(5, 2, 5, 2);
    actionLayout->setSpacing(15);

    // 按钮样式
    QString buttonStyle = R"(
        QPushButton {
            padding: 2px 8px;
            border-radius: 4px;
            font-size: 12px;
            font-weight: 500;
            border: 1px solid transparent;
            min-width: 60px;
        }
        QPushButton:hover {
            opacity: 0.9;
        }
        QPushButton:pressed {
            opacity: 0.8;
        }
    )";

    // 根据状态显示不同的操作按钮
    if (status == "待付款") {
        QPushButton *payBtn = new QPushButton("去支付");
        payBtn->setStyleSheet(buttonStyle + R"(
            QPushButton {
                background-color: #3B82F6;
                color: white;
            }
            QPushButton:hover {
                background-color: #2563EB;
            }
        )");
        payBtn->setProperty("orderId", orderId);
        connect(payBtn, &QPushButton::clicked, [this, orderId]() {
            // 从数据中获取实际金额
            double amount = 0;
            for (int i = 0; i < ordersTable->rowCount(); i++) {
                QTableWidgetItem *idItem = ordersTable->item(i, 0);
                if (idItem && idItem->text().toInt() == orderId) {
                    QTableWidgetItem *priceItem = ordersTable->item(i, 2);
                    if (priceItem) {
                        QString priceText = priceItem->text();
                        priceText = priceText.replace("¥", "").trimmed();
                        amount = priceText.toDouble();
                        break;
                    }
                }
            }
            emit paymentRequested(orderId, amount);
        });

        QPushButton *cancelBtn = new QPushButton("取消");
        cancelBtn->setStyleSheet(buttonStyle + R"(
            QPushButton {
                background-color: #6B7280;
                color: white;
            }
            QPushButton:hover {
                background-color: #4B5563;
            }
        )");
        cancelBtn->setProperty("orderId", orderId);
        connect(cancelBtn, &QPushButton::clicked, [this, orderId]() {
            onCancelOrder(orderId);
        });

        actionLayout->addWidget(payBtn);
        actionLayout->addWidget(cancelBtn);

    } else if (status == "待收货") {
        QPushButton *confirmBtn = new QPushButton("确认收货");
        confirmBtn->setStyleSheet(buttonStyle + R"(
            QPushButton {
                background-color: #10B981;
                color: white;
            }
            QPushButton:hover {
                background-color: #059669;
            }
        )");
        confirmBtn->setProperty("orderId", orderId);
        connect(confirmBtn, &QPushButton::clicked, [this, orderId]() {
            onConfirmReceipt(orderId);
        });

        QPushButton *disputeBtn = new QPushButton("售后");
        disputeBtn->setStyleSheet(buttonStyle + R"(
            QPushButton {
                background-color: #F59E0B;
                color: white;
            }
            QPushButton:hover {
                background-color: #D97706;
            }
        )");
        disputeBtn->setProperty("orderId", orderId);
        connect(disputeBtn, &QPushButton::clicked, [this, orderId]() {
            onShowDispute(orderId);
        });

        actionLayout->addWidget(confirmBtn);
        actionLayout->addWidget(disputeBtn);

    } else if (status == "已完成") {
        QPushButton *reviewBtn = new QPushButton("评价");
        reviewBtn->setStyleSheet(buttonStyle + R"(
            QPushButton {
                background-color: #8B5CF6;
                color: white;
            }
            QPushButton:hover {
                background-color: #7C3AED;
            }
        )");
        reviewBtn->setProperty("orderId", orderId);
        // 从数据中获取卖家名称
        QString sellerName;
        for (int i = 0; i < ordersTable->rowCount(); i++) {
            QTableWidgetItem *idItem = ordersTable->item(i, 0);
            if (idItem && idItem->text().toInt() == orderId) {
                QTableWidgetItem *sellerItem = ordersTable->item(i, 5);
                if (sellerItem) {
                    sellerName = sellerItem->text();
                    break;
                }
            }
        }
        reviewBtn->setProperty("sellerName", sellerName);
        connect(reviewBtn, &QPushButton::clicked, [this, orderId, reviewBtn]() {
            QString sellerName = reviewBtn->property("sellerName").toString();
            emit reviewRequested(orderId, sellerName);
        });

        QPushButton *disputeBtn = new QPushButton("售后");
        disputeBtn->setStyleSheet(buttonStyle + R"(
            QPushButton {
                background-color: #F59E0B;
                color: white;
            }
            QPushButton:hover {
                background-color: #D97706;
            }
        )");
        disputeBtn->setProperty("orderId", orderId);
        connect(disputeBtn, &QPushButton::clicked, [this, orderId]() {
            onShowDispute(orderId);
        });

        actionLayout->addWidget(reviewBtn);
        actionLayout->addWidget(disputeBtn);

    } else if (status == "纠纷处理中") {
        QPushButton *viewBtn = new QPushButton("查看详情");
        viewBtn->setStyleSheet(buttonStyle + R"(
            QPushButton {
                background-color: #EF4444;
                color: white;
            }
            QPushButton:hover {
                background-color: #DC2626;
            }
        )");
        viewBtn->setProperty("orderId", orderId);
        connect(viewBtn, &QPushButton::clicked, [this, orderId]() {
            QMessageBox::information(this, "纠纷详情",
                                     QString("订单 #%1 纠纷处理中\n\n"
                                             "纠纷类型：商品质量问题\n"
                                             "提交时间：2024-03-20 14:30\n"
                                             "当前状态：管理员审核中\n"
                                             "预计处理时间：1-3个工作日").arg(orderId));
        });

        actionLayout->addWidget(viewBtn);
    }

    actionLayout->addStretch();
    ordersTable->setCellWidget(row, 6, actionWidget);
}

void OrdersPage::onFilterOrders() {
    QString status = statusCombo->currentText();
    QString keyword = orderSearchEdit->text().trimmed();
    filterOrders(status, keyword);
}

void OrdersPage::onRefreshOrders() {
    loadOrderExamples();
}

void OrdersPage::filterOrders(const QString &status, const QString &keyword) {
    if (!ordersTable) return;

    int visibleCount = 0;

    for (int row = 0; row < ordersTable->rowCount(); row++) {
        bool showRow = true;

        // 按状态筛选
        if (status != "全部订单") {
            QTableWidgetItem *statusItem = ordersTable->item(row, 3);
            if (statusItem && statusItem->text() != status) {
                showRow = false;
            }
        }

        // 按关键词筛选
        if (showRow && !keyword.isEmpty()) {
            bool found = false;
            for (int col = 0; col < ordersTable->columnCount(); col++) {
                QTableWidgetItem *item = ordersTable->item(row, col);
                if (item && item->text().contains(keyword, Qt::CaseInsensitive)) {
                    found = true;
                    break;
                }
            }
            showRow = found;
        }

        ordersTable->setRowHidden(row, !showRow);
        if (showRow) visibleCount++;
    }

    // 更新统计信息
    QLabel *totalLabel = findChild<QLabel*>();
    if (totalLabel) {
        totalLabel->setText(QString("共 %1 个订单").arg(visibleCount));
    }
}

void OrdersPage::onCancelOrder(int orderId) {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "确认取消",
        QString("确定要取消订单 #%1 吗？").arg(orderId),
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        // 在实际项目中，这里应该更新数据库
        // 更新界面状态
        for (int row = 0; row < ordersTable->rowCount(); row++) {
            QTableWidgetItem *orderIdItem = ordersTable->item(row, 0);
            if (orderIdItem && orderIdItem->text().toInt() == orderId) {
                QTableWidgetItem *statusItem = ordersTable->item(row, 3);
                if (statusItem) {
                    statusItem->setText("已取消");
                    statusItem->setForeground(QColor(149, 165, 166));
                    statusItem->setBackground(QColor(245, 246, 246));

                    // 更新操作按钮
                    QWidget *oldWidget = ordersTable->cellWidget(row, 6);
                    if (oldWidget) oldWidget->deleteLater();

                    // 创建新的操作按钮
                    QWidget *actionWidget = new QWidget();
                    QHBoxLayout *actionLayout = new QHBoxLayout(actionWidget);
                    actionLayout->setContentsMargins(5, 2, 5, 2);
                    actionLayout->setSpacing(5);

                    QPushButton *reorderBtn = new QPushButton("重新购买");
                    reorderBtn->setStyleSheet(R"(
                        QPushButton {
                            padding: 4px 8px;
                            border-radius: 4px;
                            font-size: 12px;
                            background-color: #6B7280;
                            color: white;
                        }
                    )");
                    actionLayout->addWidget(reorderBtn);
                    actionLayout->addStretch();

                    ordersTable->setCellWidget(row, 6, actionWidget);
                }
                break;
            }
        }

        emit cancelOrderRequested(orderId);
        QMessageBox::information(this, "取消成功", "订单已取消");
    }
}

void OrdersPage::onConfirmReceipt(int orderId) {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "确认收货",
        QString("确认已收到订单 #%1 的商品吗？\n\n"
                "确认后订单将变为【已完成】状态。").arg(orderId),
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        // 更新订单状态为已完成
        for (int row = 0; row < ordersTable->rowCount(); row++) {
            QTableWidgetItem *orderIdItem = ordersTable->item(row, 0);
            if (orderIdItem && orderIdItem->text().toInt() == orderId) {
                QTableWidgetItem *statusItem = ordersTable->item(row, 3);
                if (statusItem) {
                    statusItem->setText("已完成");
                    statusItem->setForeground(QColor(46, 204, 113));
                    statusItem->setBackground(QColor(237, 247, 240));

                    // 更新操作按钮
                    QWidget *oldWidget = ordersTable->cellWidget(row, 6);
                    if (oldWidget) oldWidget->deleteLater();
                    createActionButtons(row, "已完成", orderId);
                }
                break;
            }
        }

        emit confirmReceiptRequested(orderId);
        QMessageBox::information(this, "确认成功", "订单状态已更新为【已完成】");
    }
}

void OrdersPage::onShowDispute(int orderId) {
    emit disputeRequested(orderId);
}
