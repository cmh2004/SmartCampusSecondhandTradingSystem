#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QMessageBox>
#include <QDateTime>
#include <QTableWidgetItem>
#include <QJsonArray>
#include "PaymentDialog.h"
#include "DisputeSubmitDialog.h"
#include "ReviewDialog.h"
#include "..\apiservice.h"
#include "OrdersPage.h"

OrdersPage::OrdersPage(QWidget *parent) : QWidget(parent) {
    setupUI();
    loadOrdersFromServer("","",1,10);
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

void OrdersPage::loadOrdersFromServer(const QString &status, const QString &keyword, int page, int pageSize) {
    // 保存当前筛选条件，便于刷新
    m_currentStatus = status;
    m_currentKeyword = keyword;
    m_currentPage = page;
    m_currentPageSize = pageSize;

    // 调用 ApiService 获取订单列表
    QJsonArray orders = ApiService::instance()->getOrderList(status, page, pageSize, keyword);

    // 清空表格（保留表头）
    ordersTable->setRowCount(0);

    for (const QJsonValue &val : orders) {
        QJsonObject order = val.toObject();
        int orderId = order.value("order_id").toInt();
        QString goodsName = order.value("goods_title").toString();
        double amount = order.value("deal_price").toDouble();
        QString orderStatus = order.value("status").toString(); // 假设返回 "待付款", "已完成" 等
        QString createTime = order.value("create_time").toString();
        QString seller = order.value("seller_name").toString();

        int row = ordersTable->rowCount();
        ordersTable->insertRow(row);
        ordersTable->setRowHeight(row, 50);

        // 设置各列数据
        ordersTable->setItem(row, 0, new QTableWidgetItem(QString::number(orderId)));
        ordersTable->setItem(row, 1, new QTableWidgetItem(goodsName));
        ordersTable->setItem(row, 2, new QTableWidgetItem(QString("¥%1").arg(amount)));
        ordersTable->setItem(row, 3, new QTableWidgetItem(orderStatus));
        ordersTable->setItem(row, 4, new QTableWidgetItem(createTime));
        ordersTable->setItem(row, 5, new QTableWidgetItem(seller));

        // 根据状态创建操作按钮
        createActionButtons(row, orderStatus, orderId);
    }
}

void OrdersPage::createActionButtons(int row, const QString &status, int orderId) {
    QWidget *actionWidget = new QWidget();
    QHBoxLayout *actionLayout = new QHBoxLayout(actionWidget);
    actionLayout->setContentsMargins(5, 2, 5, 2);
    actionLayout->setSpacing(15);

    // 按钮样式（沿用原样式）
    QString buttonStyle = R"(
        QPushButton {
            padding: 2px 8px;
            border-radius: 4px;
            font-size: 12px;
            font-weight: 500;
            border: 1px solid transparent;
            min-width: 60px;
        }
        QPushButton:hover { opacity: 0.9; }
        QPushButton:pressed { opacity: 0.8; }
    )";

    // 根据状态创建不同按钮
    if (status == "待付款") {
        QPushButton *payBtn = new QPushButton("去支付");
        payBtn->setStyleSheet(buttonStyle + R"(
            QPushButton { background-color: #3B82F6; color: white; }
            QPushButton:hover { background-color: #2563EB; }
        )");
        connect(payBtn, &QPushButton::clicked, [this, orderId]() {
            // 获取订单金额（可以从表格中获取或从全局数据获取）
            double amount = 0;
            for (int i = 0; i < ordersTable->rowCount(); i++) {
                QTableWidgetItem *idItem = ordersTable->item(i, 0);
                if (idItem && idItem->text().toInt() == orderId) {
                    QTableWidgetItem *priceItem = ordersTable->item(i, 2);
                    if (priceItem) {
                        amount = priceItem->text().replace("¥", "").toDouble();
                        break;
                    }
                }
            }

            // 弹出支付对话框
            PaymentDialog *dialog = new PaymentDialog(this, orderId, amount);
            connect(dialog, &PaymentDialog::accepted, [this, orderId,amount]() {
                // 支付成功，调用支付 API
                QJsonObject result = ApiService::instance()->payOrder(orderId, "微信支付",amount);
                if (result.value("success").toBool()) {
                    QMessageBox::information(this, "支付成功", "订单已支付，等待发货");
                    loadOrdersFromServer(m_currentStatus, m_currentKeyword, m_currentPage, m_currentPageSize);
                } else {
                    QMessageBox::warning(this, "支付失败", result.value("error").toString());
                }
            });
            dialog->show();
        });
        actionLayout->addWidget(payBtn);

        QPushButton *cancelBtn = new QPushButton("取消");
        cancelBtn->setStyleSheet(buttonStyle + R"(
            QPushButton { background-color: #6B7280; color: white; }
            QPushButton:hover { background-color: #4B5563; }
        )");
        connect(cancelBtn, &QPushButton::clicked, [this, orderId]() {
            QMessageBox::StandardButton reply = QMessageBox::question(
                this, "确认取消", QString("确定要取消订单 #%1 吗？").arg(orderId),
                QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::Yes) {
                QJsonObject result = ApiService::instance()->cancelOrder(orderId, "用户取消");
                if (result.value("success").toBool()) {
                    QMessageBox::information(this, "取消成功", "订单已取消");
                    loadOrdersFromServer(m_currentStatus, m_currentKeyword, m_currentPage, m_currentPageSize);
                } else {
                    QMessageBox::warning(this, "取消失败", result.value("error").toString());
                }
            }
        });
        actionLayout->addWidget(cancelBtn);

    } else if (status == "待收货") {
        QPushButton *confirmBtn = new QPushButton("确认收货");
        confirmBtn->setStyleSheet(buttonStyle + R"(
            QPushButton { background-color: #10B981; color: white; }
            QPushButton:hover { background-color: #059669; }
        )");
        connect(confirmBtn, &QPushButton::clicked, [this, orderId]() {
            QMessageBox::StandardButton reply = QMessageBox::question(
                this, "确认收货", "请确认已收到商品，确认后订单将完成。",
                QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::Yes) {
                QJsonObject result = ApiService::instance()->confirmOrder(orderId);
                if (result.value("success").toBool()) {
                    QMessageBox::information(this, "收货成功", "订单已完成，感谢您的购买！");
                    loadOrdersFromServer(m_currentStatus, m_currentKeyword, m_currentPage, m_currentPageSize);
                } else {
                    QMessageBox::warning(this, "操作失败", result.value("error").toString());
                }
            }
        });
        actionLayout->addWidget(confirmBtn);

        QPushButton *disputeBtn = new QPushButton("售后");
        disputeBtn->setStyleSheet(buttonStyle + R"(
            QPushButton { background-color: #F59E0B; color: white; }
            QPushButton:hover { background-color: #D97706; }
        )");
        connect(disputeBtn, &QPushButton::clicked, [this, orderId]() {
            // 弹出纠纷对话框
            DisputeSubmitDialog *dialog = new DisputeSubmitDialog(this, orderId);
            connect(dialog, &DisputeSubmitDialog::accepted, [this]() {
                loadOrdersFromServer(m_currentStatus, m_currentKeyword, m_currentPage, m_currentPageSize);
            });
            dialog->show();
        });
        actionLayout->addWidget(disputeBtn);

    } else if (status == "已完成") {
        QPushButton *reviewBtn = new QPushButton("评价");
        reviewBtn->setStyleSheet(buttonStyle + R"(
            QPushButton { background-color: #8B5CF6; color: white; }
            QPushButton:hover { background-color: #7C3AED; }
        )");
        connect(reviewBtn, &QPushButton::clicked, [this, orderId]() {
            // 获取卖家名称（从表格中获取）
            QString sellerName;
            for (int i = 0; i < ordersTable->rowCount(); i++) {
                QTableWidgetItem *idItem = ordersTable->item(i, 0);
                if (idItem && idItem->text().toInt() == orderId) {
                    QTableWidgetItem *sellerItem = ordersTable->item(i, 5);
                    if (sellerItem) sellerName = sellerItem->text();
                    break;
                }
            }
            ReviewDialog *dialog = new ReviewDialog(this, orderId, sellerName);
            connect(dialog, &ReviewDialog::reviewSubmitted, [this, orderId](int,int rating, const QString &comment) {
                QJsonObject result = ApiService::instance()->submitReview(orderId, rating, comment);
                if (result.value("success").toBool()) {
                    QMessageBox::information(this, "评价成功", "感谢您的评价！");
                    // 评价后订单状态不变，但可以不刷新，或者刷新以显示已评价
                } else {
                    QMessageBox::warning(this, "评价失败", result.value("error").toString());
                }
            });
            dialog->show();
        });
        actionLayout->addWidget(reviewBtn);

        QPushButton *disputeBtn = new QPushButton("售后");
        disputeBtn->setStyleSheet(buttonStyle + R"(
            QPushButton { background-color: #F59E0B; color: white; }
            QPushButton:hover { background-color: #D97706; }
        )");
        connect(disputeBtn, &QPushButton::clicked, [this, orderId]() {
            DisputeSubmitDialog *dialog = new DisputeSubmitDialog(this, orderId);
            connect(dialog, &DisputeSubmitDialog::accepted, [this]() { loadOrdersFromServer(m_currentStatus, m_currentKeyword, m_currentPage, m_currentPageSize); });
            dialog->show();
        });
        actionLayout->addWidget(disputeBtn);

    } else if (status == "纠纷处理中") {
        QPushButton *viewBtn = new QPushButton("查看详情");
        viewBtn->setStyleSheet(buttonStyle + R"(
            QPushButton { background-color: #EF4444; color: white; }
            QPushButton:hover { background-color: #DC2626; }
        )");
        connect(viewBtn, &QPushButton::clicked, [this, orderId]() {
            // 显示纠纷详情（可以从服务端获取）
            QJsonObject result = ApiService::instance()->getDisputeByOrder(orderId);
            if (result.value("success").toBool()) {
                QJsonObject dispute = result.value("data").toObject();
                QString detail = QString("纠纷类型：%1\n描述：%2\n处理结果：%3")
                                     .arg(dispute.value("type").toString())
                                     .arg(dispute.value("description").toString())
                                     .arg(dispute.value("handle_result").toString());
                QMessageBox::information(this, "纠纷详情", detail);
            } else {
                QMessageBox::warning(this, "提示", "未找到相关纠纷信息");
            }
        });
        actionLayout->addWidget(viewBtn);
    }

    actionLayout->addStretch();
    ordersTable->setCellWidget(row, 6, actionWidget);
}

void OrdersPage::onFilterOrders() {
    // 获取当前筛选条件
    QString status = statusCombo->currentText();
    QString keyword = orderSearchEdit->text().trimmed();

    // 将状态文本转换为服务端接受的格式（例如 "待付款" -> "pending_payment"）
    QString statusParam;
    if (status == "全部订单") statusParam = "";
    else if (status == "待付款") statusParam = "pending_payment";
    else if (status == "待发货") statusParam = "paid";
    else if (status == "待收货") statusParam = "shipped";
    else if (status == "已完成") statusParam = "completed";
    else if (status == "已取消") statusParam = "cancelled";
    else if (status == "纠纷处理中") statusParam = "dispute";

    // 将关键词作为额外参数传给 loadOrdersFromServer（但需要服务端支持按关键词搜索订单）
    // 如果服务端不支持，可以在本地对返回结果再次过滤，但推荐服务端支持
    // 这里我们假设 loadOrdersFromServer 已经支持 keyword 参数
    loadOrdersFromServer(statusParam, keyword, 1, 20);
}

void OrdersPage::onRefreshOrders() {
    loadOrdersFromServer(m_currentStatus, m_currentKeyword, m_currentPage, m_currentPageSize);
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
