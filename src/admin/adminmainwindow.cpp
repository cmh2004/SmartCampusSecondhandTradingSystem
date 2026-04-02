#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QDateTime>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include "../apiservice.h"
#include "adminmainwindow.h"

AdminMainWindow::AdminMainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("校园二手交易系统 - 管理员后台");
    setMinimumSize(1200, 800);

    setupUI();
    loadGoodsReviewData();
    loadUserManagementData();
    loadDisputeData();

    // 连接信号
    connect(mainTabWidget, &QTabWidget::currentChanged, this, &AdminMainWindow::onTabChanged);
}

void AdminMainWindow::setupUI() {
    // 创建主窗口部件
    QWidget *mainWidget = new QWidget(this);
    setCentralWidget(mainWidget);

    // 创建菜单栏
    QMenuBar *menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    // 系统菜单
    QMenu *systemMenu = menuBar->addMenu("系统");
    QAction *logoutAction = systemMenu->addAction("退出登录");
    connect(logoutAction, &QAction::triggered, this, &AdminMainWindow::onLogoutClicked);

    // 主标签页
    mainTabWidget = new QTabWidget(mainWidget);
    mainTabWidget->setTabPosition(QTabWidget::North);

    // 创建各个页面
    QWidget *goodsReviewPage = createGoodsReviewPage();
    QWidget *userManagementPage = createUserManagementPage();
    QWidget *disputeManagementPage = createDisputeManagementPage();

    mainTabWidget->addTab(goodsReviewPage, "商品审核");
    mainTabWidget->addTab(userManagementPage, "用户管理");
    mainTabWidget->addTab(disputeManagementPage, "纠纷处理");

    QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->addWidget(mainTabWidget);

    // 状态栏
    QStatusBar *statusBar = this->statusBar();
    QLabel *statusLabel = new QLabel("欢迎使用管理员后台系统");
    statusBar->addWidget(statusLabel);

    // 设置样式
    setStyleSheet(R"(
        QMainWindow {
            background-color: #f5f7fa;
        }
        QTabWidget::pane {
            border: 1px solid #ddd;
            border-radius: 8px;
            background-color: white;
            margin-top: 5px;
        }
        QTabBar::tab {
            padding: 12px 24px;
            background-color: #ecf0f1;
            border-radius: 6px 6px 0 0;
            margin-right: 3px;
            font-size: 14px;
            min-width: 100px;
        }
        QTabBar::tab:selected {
            background-color: white;
            border-bottom: 3px solid #9b59b6;
            font-weight: bold;
            color: #9b59b6;
        }
        QGroupBox {
            font-weight: bold;
            border: 1px solid #ddd;
            border-radius: 6px;
            margin-top: 10px;
            padding-top: 10px;
            background-color: white;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
            color: #9b59b6;
        }
        QTableWidget {
            background-color: white;
            border: 1px solid #ddd;
            border-radius: 6px;
            alternate-background-color: #f9f9f9;
        }
        QTableWidget::item {
            padding: 8px;
        }
        QTableWidget::item:selected {
            background-color: #e8f4f8;
        }
        QHeaderView::section {
            background-color: #f8fafc;
            padding: 10px;
            border: none;
            border-bottom: 2px solid #e2e8f0;
            font-weight: 600;
            color: #475569;
        }
        #primaryBtn {
            background-color: #3498db;
            color: white;
            border-radius: 4px;
            padding: 0px 16px;
            text-align: center;/* 水平居中 */
            vertical-align: middle; /* 垂直居中 */
        }
        #successBtn {
            background-color: #27ae60;
            color: white;
            border-radius: 4px;
            padding: 0px 16px;
            text-align: center;/* 水平居中 */
            vertical-align: middle; /* 垂直居中 */
        }
        #warningBtn {
            background-color: #e74c3c;
            color: white;
            border-radius: 4px;
            padding: 0px 16px;
            text-align: center;/* 水平居中 */
            vertical-align: middle; /* 垂直居中 */
        }
        #secondaryBtn {
            background-color: #95a5a6;
            color: white;
            border-radius: 4px;
            padding: 0px 16px;
            text-align: center;/* 水平居中 */
            vertical-align: middle; /* 垂直居中 */
        }
    )");
}

QWidget* AdminMainWindow::createGoodsReviewPage() {
    QWidget *page = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // 筛选工具栏
    QWidget *filterWidget = new QWidget();
    QHBoxLayout *filterLayout = new QHBoxLayout(filterWidget);
    filterLayout->setContentsMargins(0, 0, 0, 0);
    filterLayout->setSpacing(10);

    filterLayout->addWidget(new QLabel("商品名称:"));
    goodsSearchEdit = new QLineEdit();
    goodsSearchEdit->setPlaceholderText("搜索商品名称...");
    goodsSearchEdit->setFixedWidth(200);
    filterLayout->addWidget(goodsSearchEdit);

    filterLayout->addWidget(new QLabel("审核状态:"));
    goodsStatusCombo = new QComboBox();
    goodsStatusCombo->addItems({"全部", "待审核", "已通过", "已拒绝"});
    filterLayout->addWidget(goodsStatusCombo);

    filterLayout->addWidget(new QLabel("发布时间:"));
    goodsDateFromEdit = new QDateEdit();
    goodsDateFromEdit->setDate(QDate::currentDate().addDays(-7));
    goodsDateFromEdit->setDisplayFormat("yyyy-MM-dd");
    goodsDateFromEdit->setFixedWidth(120);
    filterLayout->addWidget(goodsDateFromEdit);

    filterLayout->addWidget(new QLabel("至"));
    goodsDateToEdit = new QDateEdit();
    goodsDateToEdit->setDate(QDate::currentDate());
    goodsDateToEdit->setDisplayFormat("yyyy-MM-dd");
    goodsDateToEdit->setFixedWidth(120);
    filterLayout->addWidget(goodsDateToEdit);

    QPushButton *filterBtn = new QPushButton("筛选");
    filterBtn->setObjectName("primaryBtn");
    filterLayout->addWidget(filterBtn);

    QPushButton *exportBtn = new QPushButton("导出数据");
    exportBtn->setObjectName("secondaryBtn");
    filterLayout->addWidget(exportBtn);

    filterLayout->addStretch();

    connect(filterBtn, &QPushButton::clicked, this, &AdminMainWindow::onFilterGoods);
    connect(exportBtn, &QPushButton::clicked, this, &AdminMainWindow::onExportGoodsData);

    // 商品审核表格
    goodsReviewTable = new QTableWidget(0, 7);
    QStringList headers = {"商品ID", "商品名称", "发布者", "价格", "发布时间", "状态", "操作"};
    goodsReviewTable->setHorizontalHeaderLabels(headers);
    goodsReviewTable->verticalHeader()->setVisible(false);
    goodsReviewTable->setAlternatingRowColors(true);
    goodsReviewTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    goodsReviewTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 设置列宽
    goodsReviewTable->setColumnWidth(0, 80);
    goodsReviewTable->setColumnWidth(1, 250);
    goodsReviewTable->setColumnWidth(2, 120);
    goodsReviewTable->setColumnWidth(3, 100);
    goodsReviewTable->setColumnWidth(4, 150);
    goodsReviewTable->setColumnWidth(5, 100);
    goodsReviewTable->horizontalHeader()->setStretchLastSection(true);

    mainLayout->addWidget(filterWidget);
    mainLayout->addWidget(goodsReviewTable, 1);

    return page;
}

QWidget* AdminMainWindow::createUserManagementPage() {
    QWidget *page = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // 筛选工具栏
    QWidget *filterWidget = new QWidget();
    QHBoxLayout *filterLayout = new QHBoxLayout(filterWidget);
    filterLayout->setContentsMargins(0, 0, 0, 0);
    filterLayout->setSpacing(10);

    filterLayout->addWidget(new QLabel("用户搜索:"));
    userSearchEdit = new QLineEdit();
    userSearchEdit->setPlaceholderText("用户名/学号/手机号...");
    userSearchEdit->setFixedWidth(200);
    filterLayout->addWidget(userSearchEdit);

    filterLayout->addWidget(new QLabel("用户状态:"));
    userStatusCombo = new QComboBox();
    userStatusCombo->addItems({"全部", "正常", "已封禁", "限制交易"});
    filterLayout->addWidget(userStatusCombo);

    filterLayout->addWidget(new QLabel("信用等级:"));
    userCreditLevelCombo = new QComboBox();
    userCreditLevelCombo->addItems({"全部", "优秀", "良好", "中等", "较差"});
    filterLayout->addWidget(userCreditLevelCombo);

    QPushButton *searchBtn = new QPushButton("搜索");
    searchBtn->setObjectName("primaryBtn");
    filterLayout->addWidget(searchBtn);

    filterLayout->addStretch();

    connect(searchBtn, &QPushButton::clicked, this, &AdminMainWindow::onSearchUsers);

    // 用户管理表格
    userTable = new QTableWidget(0, 8);
    QStringList headers = {"用户ID", "用户名", "学号", "手机号", "注册时间", "信用分", "状态", "操作"};
    userTable->setHorizontalHeaderLabels(headers);
    userTable->verticalHeader()->setVisible(false);
    userTable->setAlternatingRowColors(true);
    userTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    userTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 设置列宽
    userTable->setColumnWidth(0, 80);
    userTable->setColumnWidth(1, 120);
    userTable->setColumnWidth(2, 120);
    userTable->setColumnWidth(3, 120);
    userTable->setColumnWidth(4, 150);
    userTable->setColumnWidth(5, 100);
    userTable->setColumnWidth(6, 100);
    userTable->horizontalHeader()->setStretchLastSection(true);

    mainLayout->addWidget(filterWidget);
    mainLayout->addWidget(userTable, 1);

    return page;
}

QWidget* AdminMainWindow::createDisputeManagementPage() {
    QWidget *page = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // 筛选工具栏
    QWidget *filterWidget = new QWidget();
    QHBoxLayout *filterLayout = new QHBoxLayout(filterWidget);
    filterLayout->setContentsMargins(0, 0, 0, 0);
    filterLayout->setSpacing(10);

    filterLayout->addWidget(new QLabel("纠纷类型:"));
    disputeTypeCombo = new QComboBox();
    disputeTypeCombo->addItems({"全部", "商品不符", "质量问题", "未发货", "价格纠纷", "其他"});
    filterLayout->addWidget(disputeTypeCombo);

    filterLayout->addWidget(new QLabel("处理状态:"));
    disputeStatusCombo = new QComboBox();
    disputeStatusCombo->addItems({"全部", "待处理", "处理中", "已解决", "已关闭"});
    filterLayout->addWidget(disputeStatusCombo);

    filterLayout->addWidget(new QLabel("提交日期:"));
    disputeDateEdit = new QDateEdit();
    disputeDateEdit->setDate(QDate::currentDate().addDays(-30));
    disputeDateEdit->setDisplayFormat("yyyy-MM-dd");
    disputeDateEdit->setFixedWidth(120);
    filterLayout->addWidget(disputeDateEdit);

    QPushButton *filterBtn = new QPushButton("筛选");
    filterBtn->setObjectName("primaryBtn");
    filterLayout->addWidget(filterBtn);

    filterLayout->addStretch();

    connect(filterBtn, &QPushButton::clicked, this, &AdminMainWindow::onFilterDisputes);

    // 纠纷处理表格
    disputeTable = new QTableWidget(0, 9);
    QStringList headers = {"纠纷ID", "订单号", "投诉方", "被投诉方", "纠纷类型", "提交时间", "当前状态", "处理进度", "操作"};
    disputeTable->setHorizontalHeaderLabels(headers);
    disputeTable->verticalHeader()->setVisible(false);
    disputeTable->setAlternatingRowColors(true);
    disputeTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    disputeTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 设置列宽
    disputeTable->setColumnWidth(0, 80);
    disputeTable->setColumnWidth(1, 100);
    disputeTable->setColumnWidth(2, 100);
    disputeTable->setColumnWidth(3, 100);
    disputeTable->setColumnWidth(4, 120);
    disputeTable->setColumnWidth(5, 150);
    disputeTable->setColumnWidth(6, 100);
    disputeTable->setColumnWidth(7, 120);
    disputeTable->horizontalHeader()->setStretchLastSection(true);

    mainLayout->addWidget(filterWidget);
    mainLayout->addWidget(disputeTable, 1);

    return page;
}

void AdminMainWindow::onLogoutClicked() {
    QMessageBox::StandardButton reply = QMessageBox::question(this, "退出登录", "确定要退出管理员账号吗？");
    if (reply == QMessageBox::Yes) {
        close();
    }
}

void AdminMainWindow::onTabChanged(int index) {
    // 根据标签页切换加载数据
    switch(index) {
    case 0: loadGoodsReviewData(); break;
    case 1: loadUserManagementData(); break;
    case 2: loadDisputeData(); break;
    }
}

// 数据加载函数
void AdminMainWindow::loadGoodsReviewData() {
    goodsReviewTable->setRowCount(0);

    // 调用 API 获取待审核商品（status = 0）
    QJsonArray goodsList = ApiService::instance()->getPendingGoods(1, 20);
    for (const QJsonValue &val : goodsList) {
        QJsonObject goods = val.toObject();
        int goodsId = goods.value("goods_id").toInt();
        QString name = goods.value("name").toString();
        QString seller = goods.value("seller_name").toString(); // 需确保服务端返回 seller_name
        double price = goods.value("price").toDouble();
        QString publishTime = goods.value("publish_time").toString();
        QString statusText = goods.value("status").toString(); // 0=待审核, 1=已上架, 4=已拒绝

        int row = goodsReviewTable->rowCount();
        goodsReviewTable->insertRow(row);
        goodsReviewTable->setRowHeight(row, 40);

        goodsReviewTable->setItem(row, 0, new QTableWidgetItem(QString::number(goodsId)));
        goodsReviewTable->setItem(row, 1, new QTableWidgetItem(name));
        goodsReviewTable->setItem(row, 2, new QTableWidgetItem(seller));
        goodsReviewTable->setItem(row, 3, new QTableWidgetItem(QString("¥%1").arg(price)));
        goodsReviewTable->setItem(row, 4, new QTableWidgetItem(publishTime));

        // 状态列
        QTableWidgetItem *statusItem = new QTableWidgetItem(statusText);
        if (statusText == "待审核") statusItem->setForeground(QColor(230, 126, 34));
        else if (statusText == "已上架") statusItem->setForeground(QColor(46, 204, 113));
        else if (statusText == "已拒绝") statusItem->setForeground(QColor(231, 76, 60));
        goodsReviewTable->setItem(row, 5, statusItem);

        // 操作按钮
        QWidget *actionWidget = new QWidget();
        QHBoxLayout *actionLayout = new QHBoxLayout(actionWidget);
        actionLayout->setContentsMargins(5, 1, 5, 1);
        actionLayout->setSpacing(5);

        if (statusText == "待审核") {
            QPushButton *approveBtn = new QPushButton("通过");
            approveBtn->setObjectName("successBtn");
            approveBtn->setFixedSize(60, 30);
            approveBtn->setProperty("goodsId", goodsId);
            connect(approveBtn, &QPushButton::clicked, [this, approveBtn]() {
                onReviewGoods(approveBtn->property("goodsId").toInt(), true);
            });

            QPushButton *rejectBtn = new QPushButton("拒绝");
            rejectBtn->setObjectName("warningBtn");
            rejectBtn->setFixedSize(60, 30);
            rejectBtn->setProperty("goodsId", goodsId);
            connect(rejectBtn, &QPushButton::clicked, [this, rejectBtn]() {
                onReviewGoods(rejectBtn->property("goodsId").toInt(), false);
            });

            QPushButton *viewBtn = new QPushButton("查看");
            viewBtn->setObjectName("secondaryBtn");
            viewBtn->setFixedSize(60, 30);
            connect(viewBtn, &QPushButton::clicked, [this, goodsId]() {
                // 可打开商品详情对话框
                QMessageBox::information(this, "查看详情", QString("商品ID: %1").arg(goodsId));
            });

            actionLayout->addWidget(approveBtn);
            actionLayout->addWidget(rejectBtn);
            actionLayout->addWidget(viewBtn);
        } else {
            QPushButton *viewBtn = new QPushButton("查看详情");
            viewBtn->setObjectName("secondaryBtn");
            viewBtn->setFixedSize(80, 30);
            connect(viewBtn, &QPushButton::clicked, [this, goodsId,statusText]() {
                QMessageBox::information(this, "查看详情", QString("商品ID: %1\n状态: %2").arg(goodsId).arg(statusText));
            });
            actionLayout->addWidget(viewBtn);
        }
        actionLayout->addStretch();
        goodsReviewTable->setCellWidget(row, 6, actionWidget);
    }
}

void AdminMainWindow::onReviewGoods(int goodsId, bool approve) {
    QString comment;
    if (!approve) {
        bool ok;
        comment = QInputDialog::getText(this, "拒绝原因",
                                        "请输入拒绝原因:",
                                        QLineEdit::Normal,
                                        "", &ok);
        if (!ok) return;
    }
    QJsonObject result = ApiService::instance()->reviewGoods(goodsId, approve, comment);
    if (result.value("success").toBool()) {
        QMessageBox::information(this, "审核结果", result.value("message").toString());
        loadGoodsReviewData(); // 刷新列表
    } else {
        QMessageBox::warning(this, "审核失败", result.value("error").toString());
    }
}

void AdminMainWindow::onFilterGoods() {
    QString keyword = goodsSearchEdit->text();
    QString status = goodsStatusCombo->currentText();
    QString dateFrom = goodsDateFromEdit->date().toString("yyyy-MM-dd");
    QString dateTo = goodsDateToEdit->date().toString("yyyy-MM-dd");

    QMessageBox::information(this, "筛选条件",
                             QString("商品名称: %1\n审核状态: %2\n时间范围: %3 至 %4")
                                 .arg(keyword).arg(status).arg(dateFrom).arg(dateTo));

    loadGoodsReviewData();
}

void AdminMainWindow::onExportGoodsData() {
    QString fileName = QFileDialog::getSaveFileName(this, "导出数据",
                                                    "商品审核数据.csv", "CSV文件 (*.csv)");
    if (!fileName.isEmpty()) {
        QMessageBox::information(this, "导出成功",
                                 QString("数据已导出到: %1").arg(fileName));
    }
}

// 用户管理相关
void AdminMainWindow::onBlockUser(const QString &userId, bool block) {
    QString newStatus = block ? "0" : "1";
    QJsonObject result = ApiService::instance()->updateUserStatus(userId, newStatus, block ? "管理员封禁" : "管理员解封");
    if (result.value("success").toBool()) {
        QMessageBox::information(this, "操作成功", result.value("message").toString());
        loadUserManagementData(); // 刷新列表
    } else {
        QMessageBox::warning(this, "操作失败", result.value("error").toString());
    }
}

void AdminMainWindow::onAdjustCreditScore(const QString &userId, int score) {
    // 注意：服务端可能需要一个专门调整信用分的接口，或者通过 updateUser 修改 credit_score
    // 这里假设 ApiService 有 updateUserStatus 或类似方法可以调整信用分
    // 如果没有，需要扩展 ApiService
    QJsonObject updates;
    updates["credit_score"] = score;
    QJsonObject result = ApiService::instance()->updateUserProfile(updates); // 可能需要根据 userId 调整
    if (result.value("success").toBool()) {
        QMessageBox::information(this, "调整成功", "信用分已更新");
        loadUserManagementData();
    } else {
        QMessageBox::warning(this, "调整失败", result.value("error").toString());
    }
}

void AdminMainWindow::onSearchUsers() {
    QString keyword = userSearchEdit->text();
    QString status = userStatusCombo->currentText();
    QString creditLevel = userCreditLevelCombo->currentText();

    QMessageBox::information(this, "搜索条件",
                             QString("关键词: %1\n状态: %2\n信用等级: %3")
                                 .arg(keyword).arg(status).arg(creditLevel));

    // 这里应该实现具体的搜索逻辑
    loadUserManagementData();
}

// 纠纷处理相关
void AdminMainWindow::onViewDisputeDetail(int disputeId) {
    QMessageBox::information(this, "纠纷详情",
                             QString("查看纠纷ID %1 的详细信息").arg(disputeId));
}

void AdminMainWindow::onProcessDispute(int disputeId, const QString &result) {
    QJsonObject response = ApiService::instance()->processDispute(disputeId, result, "");
    if (response.value("success").toBool()) {
        QMessageBox::information(this, "处理成功", "纠纷已处理");
        loadDisputeData();
    } else {
        QMessageBox::warning(this, "处理失败", response.value("error").toString());
    }
}

void AdminMainWindow::onFilterDisputes() {
    QString type = disputeTypeCombo->currentText();
    QString status = disputeStatusCombo->currentText();
    QString date = disputeDateEdit->date().toString("yyyy-MM-dd");

    QMessageBox::information(this, "筛选条件",
                             QString("纠纷类型: %1\n处理状态: %2\n提交日期: %3之后")
                                 .arg(type).arg(status).arg(date));

    loadDisputeData();
}

void AdminMainWindow::loadUserManagementData() {
    userTable->setRowCount(0);
    QJsonArray users = ApiService::instance()->getUserList("", 1, 20);
    for (const QJsonValue &val : users) {
        QJsonObject user = val.toObject();
        int userId = user.value("id").toInt();
        QString username = user.value("account").toString();
        QString studentId = user.value("student_id").toString(); // 需服务端返回
        QString phone = user.value("phone").toString();
        QString regTime = user.value("register_time").toString();
        int creditScore = user.value("credit_score").toInt();
        int status = user.value("status").toInt(); // 1=正常, 0=禁用

        int row = userTable->rowCount();
        userTable->insertRow(row);
        userTable->setItem(row, 0, new QTableWidgetItem(QString::number(userId)));
        userTable->setItem(row, 1, new QTableWidgetItem(username));
        userTable->setItem(row, 2, new QTableWidgetItem(studentId));
        userTable->setItem(row, 3, new QTableWidgetItem(phone));
        userTable->setItem(row, 4, new QTableWidgetItem(regTime));
        userTable->setItem(row, 5, new QTableWidgetItem(QString::number(creditScore)));

        QString statusText = (status == 1) ? "正常" : "已封禁";
        QTableWidgetItem *statusItem = new QTableWidgetItem(statusText);
        if (status == 0) statusItem->setForeground(QColor(231, 76, 60));
        userTable->setItem(row, 6, statusItem);

        // 操作按钮
        QWidget *actionWidget = new QWidget();
        QHBoxLayout *actionLayout = new QHBoxLayout(actionWidget);
        actionLayout->setContentsMargins(5, 1, 5, 1);
        actionLayout->setSpacing(5);

        if (status == 1) {
            QPushButton *blockBtn = new QPushButton("封禁");
            blockBtn->setObjectName("warningBtn");
            blockBtn->setFixedSize(60, 30);
            blockBtn->setProperty("userId", userId);
            connect(blockBtn, &QPushButton::clicked, [this, blockBtn]() {
                onBlockUser(blockBtn->property("userId").toString(), true);
            });
            actionLayout->addWidget(blockBtn);
        } else {
            QPushButton *unblockBtn = new QPushButton("解封");
            unblockBtn->setObjectName("successBtn");
            unblockBtn->setFixedSize(60, 30);
            unblockBtn->setProperty("userId", userId);
            connect(unblockBtn, &QPushButton::clicked, [this, unblockBtn]() {
                onBlockUser(unblockBtn->property("userId").toString(), false);
            });
            actionLayout->addWidget(unblockBtn);
        }

        QPushButton *adjustBtn = new QPushButton("调整信用");
        adjustBtn->setObjectName("secondaryBtn");
        adjustBtn->setFixedSize(80, 30);
        adjustBtn->setProperty("userId", userId);
        connect(adjustBtn, &QPushButton::clicked, [this, adjustBtn]() {
            bool ok;
            int newScore = QInputDialog::getInt(this, "调整信用分",
                                                "输入新信用分 (0-100):",
                                                50, 0, 100, 1, &ok);
            if (ok) {
                onAdjustCreditScore(adjustBtn->property("userId").toString(), newScore);
            }
        });
        actionLayout->addWidget(adjustBtn);

        actionLayout->addStretch();
        userTable->setCellWidget(row, 7, actionWidget);
    }
}

void AdminMainWindow::loadDisputeData() {
    disputeTable->setRowCount(0);
    QJsonArray disputes = ApiService::instance()->getDisputeList("", 1, 20);
    for (const QJsonValue &val : disputes) {
        QJsonObject dispute = val.toObject();
        int disputeId = dispute.value("id").toInt();
        int orderId = dispute.value("order_id").toInt();
        QString complainant = dispute.value("complainant_name").toString(); // 需服务端返回
        QString defendant = dispute.value("defendant_name").toString();
        QString type = dispute.value("type").toString();
        QString createTime = dispute.value("create_time").toString();
        int status = dispute.value("status").toInt(); // 0待处理,1处理中,2已解决,3已关闭
        QString progress = dispute.value("handle_result").toString();

        int row = disputeTable->rowCount();
        disputeTable->insertRow(row);
        disputeTable->setItem(row, 0, new QTableWidgetItem(QString::number(disputeId)));
        disputeTable->setItem(row, 1, new QTableWidgetItem(QString::number(orderId)));
        disputeTable->setItem(row, 2, new QTableWidgetItem(complainant));
        disputeTable->setItem(row, 3, new QTableWidgetItem(defendant));
        disputeTable->setItem(row, 4, new QTableWidgetItem(type));
        disputeTable->setItem(row, 5, new QTableWidgetItem(createTime));

        QString statusText;
        if (status == 0) statusText = "待处理";
        else if (status == 1) statusText = "处理中";
        else if (status == 2) statusText = "已解决";
        else statusText = "已关闭";
        disputeTable->setItem(row, 6, new QTableWidgetItem(statusText));
        disputeTable->setItem(row, 7, new QTableWidgetItem(progress));

        // 操作按钮
        QWidget *actionWidget = new QWidget();
        QHBoxLayout *actionLayout = new QHBoxLayout(actionWidget);
        actionLayout->setContentsMargins(5, 1, 5, 1);
        actionLayout->setSpacing(5);

        if (status == 0 || status == 1) {
            QPushButton *processBtn = new QPushButton("处理");
            processBtn->setObjectName("primaryBtn");
            processBtn->setFixedSize(60, 30);
            processBtn->setProperty("disputeId", disputeId);
            connect(processBtn, &QPushButton::clicked, [this, processBtn]() {
                bool ok;
                QString result = QInputDialog::getText(this, "处理纠纷",
                                                       "请输入处理结果:",
                                                       QLineEdit::Normal,
                                                       "", &ok);
                if (ok && !result.isEmpty()) {
                    onProcessDispute(processBtn->property("disputeId").toInt(), result);
                }
            });
            actionLayout->addWidget(processBtn);
        } else {
            QPushButton *viewBtn = new QPushButton("查看详情");
            viewBtn->setObjectName("secondaryBtn");
            viewBtn->setFixedSize(80, 30);
            connect(viewBtn, &QPushButton::clicked, [this, disputeId]() {
                onViewDisputeDetail(disputeId);
            });
            actionLayout->addWidget(viewBtn);
        }
        actionLayout->addStretch();
        disputeTable->setCellWidget(row, 8, actionWidget);
    }
}

AdminMainWindow::~AdminMainWindow() {}
