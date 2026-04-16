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
#include <QApplication>
#include "../loginpage.h"
#include "../user/mainwindow.h"
#include "../apiservice.h"
#include "adminmainwindow.h"
#include "reportsmanagepage.h"

AdminMainWindow::AdminMainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("校园二手交易系统 - 管理员后台");
    setMinimumSize(1200, 800);

    setupUI();
    loadGoodsReviewData("", "全部", "", "");
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
    ReportsManagePage *reportsPage = new ReportsManagePage(this);

    mainTabWidget->addTab(goodsReviewPage, "商品审核");
    mainTabWidget->addTab(userManagementPage, "用户管理");
    mainTabWidget->addTab(disputeManagementPage, "纠纷处理");
    mainTabWidget->addTab(reportsPage, "举报管理");

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
    goodsStatusCombo->addItems({"全部", "待审核", "在售", "交易中", "已售出", "已拒绝", "已下架"});
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

    filterLayout->addStretch();

    connect(filterBtn, &QPushButton::clicked, this, &AdminMainWindow::onFilterGoods);

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

    // 分页控件
    QWidget *paginationWidget = new QWidget();
    QHBoxLayout *paginationLayout = new QHBoxLayout(paginationWidget);
    paginationLayout->setContentsMargins(0, 10, 0, 0);
    paginationLayout->setAlignment(Qt::AlignCenter);

    goodsPrevBtn = new QPushButton("上一页");
    goodsPrevBtn->setObjectName("secondaryBtn");
    goodsPrevBtn->setFixedSize(80, 32);
    goodsNextBtn = new QPushButton("下一页");
    goodsNextBtn->setObjectName("secondaryBtn");
    goodsNextBtn->setFixedSize(80, 32);
    goodsPageInfoLabel = new QLabel("第 1 页");
    goodsPageInfoLabel->setStyleSheet("font-size: 13px; color: #475569; margin: 0 15px;");

    paginationLayout->addWidget(goodsPrevBtn);
    paginationLayout->addWidget(goodsPageInfoLabel);
    paginationLayout->addWidget(goodsNextBtn);

    mainLayout->addWidget(paginationWidget);

    // 连接信号
    connect(goodsPrevBtn, &QPushButton::clicked, this, [this]() {
        if (m_goodsCurrentPage > 1) {
            loadGoodsReviewData(goodsSearchEdit->text().trimmed(),
                                goodsStatusCombo->currentText(),
                                goodsDateFromEdit->date().toString("yyyy-MM-dd"),
                                goodsDateToEdit->date().toString("yyyy-MM-dd"),
                                m_goodsCurrentPage - 1);
        }
    });
    connect(goodsNextBtn, &QPushButton::clicked, this, [this]() {
        loadGoodsReviewData(goodsSearchEdit->text().trimmed(),
                            goodsStatusCombo->currentText(),
                            goodsDateFromEdit->date().toString("yyyy-MM-dd"),
                            goodsDateToEdit->date().toString("yyyy-MM-dd"),
                            m_goodsCurrentPage + 1);
    });

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
    userSearchEdit->setPlaceholderText("请输入用户名");
    userSearchEdit->setFixedWidth(200);
    filterLayout->addWidget(userSearchEdit);

    filterLayout->addWidget(new QLabel("用户状态:"));
    userStatusCombo = new QComboBox();
    userStatusCombo->addItems({"全部", "正常", "已封禁"});
    filterLayout->addWidget(userStatusCombo);

    QPushButton *searchBtn = new QPushButton("搜索");
    searchBtn->setObjectName("primaryBtn");
    filterLayout->addWidget(searchBtn);

    filterLayout->addStretch();

    connect(searchBtn, &QPushButton::clicked, this, &AdminMainWindow::onSearchUsers);

    // 用户管理表格
    userTable = new QTableWidget(0, 7);
    QStringList headers = {"用户ID", "用户名", "手机号", "注册时间", "信用分", "状态", "操作"};
    userTable->setHorizontalHeaderLabels(headers);
    userTable->verticalHeader()->setVisible(false);
    userTable->setAlternatingRowColors(true);
    userTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    userTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 设置列宽
    userTable->setColumnWidth(0, 80);
    userTable->setColumnWidth(1, 150);
    userTable->setColumnWidth(2, 120);
    userTable->setColumnWidth(3, 150);
    userTable->setColumnWidth(4, 150);
    userTable->setColumnWidth(5, 100);
    userTable->horizontalHeader()->setStretchLastSection(true);
    userTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    mainLayout->addWidget(filterWidget);
    mainLayout->addWidget(userTable, 1);

    // 分页控件
    QWidget *paginationWidget = new QWidget();
    QHBoxLayout *paginationLayout = new QHBoxLayout(paginationWidget);
    paginationLayout->setContentsMargins(0, 10, 0, 0);
    paginationLayout->setAlignment(Qt::AlignCenter);

    userPrevBtn = new QPushButton("上一页");
    userPrevBtn->setObjectName("secondaryBtn");
    userPrevBtn->setFixedSize(80, 32);
    userNextBtn = new QPushButton("下一页");
    userNextBtn->setObjectName("secondaryBtn");
    userNextBtn->setFixedSize(80, 32);
    userPageInfoLabel = new QLabel("第 1 页");
    userPageInfoLabel->setStyleSheet("font-size: 13px; color: #475569; margin: 0 15px;");

    paginationLayout->addWidget(userPrevBtn);
    paginationLayout->addWidget(userPageInfoLabel);
    paginationLayout->addWidget(userNextBtn);

    mainLayout->addWidget(paginationWidget);

    // 连接信号
    connect(userPrevBtn, &QPushButton::clicked, this, [this]() {
        if (m_userCurrentPage > 1) {
            loadUserManagementData(userSearchEdit->text().trimmed(),
                                   userStatusCombo->currentText(),
                                   m_userCurrentPage - 1);
        }
    });
    connect(userNextBtn, &QPushButton::clicked, this, [this]() {
        loadUserManagementData(userSearchEdit->text().trimmed(),
                               userStatusCombo->currentText(),
                               m_userCurrentPage + 1);
    });

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

    filterLayout->addWidget(new QLabel("处理状态:"));
    disputeStatusCombo = new QComboBox();
    disputeStatusCombo->addItems({"全部", "待处理", "已解决"});
    filterLayout->addWidget(disputeStatusCombo);

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

    // 分页控件
    QWidget *paginationWidget = new QWidget();
    QHBoxLayout *paginationLayout = new QHBoxLayout(paginationWidget);
    paginationLayout->setContentsMargins(0, 10, 0, 0);
    paginationLayout->setAlignment(Qt::AlignCenter);

    disputePrevBtn = new QPushButton("上一页");
    disputePrevBtn->setObjectName("secondaryBtn");
    disputePrevBtn->setFixedSize(80, 32);
    disputeNextBtn = new QPushButton("下一页");
    disputeNextBtn->setObjectName("secondaryBtn");
    disputeNextBtn->setFixedSize(80, 32);
    disputePageInfoLabel = new QLabel("第 1 页");
    disputePageInfoLabel->setStyleSheet("font-size: 13px; color: #475569; margin: 0 15px;");

    paginationLayout->addWidget(disputePrevBtn);
    paginationLayout->addWidget(disputePageInfoLabel);
    paginationLayout->addWidget(disputeNextBtn);

    mainLayout->addWidget(paginationWidget);

    connect(disputePrevBtn, &QPushButton::clicked, this, [this]() {
        if (m_disputeCurrentPage > 1) {
            loadDisputeData(getDisputeStatusParam(), m_disputeCurrentPage - 1);
        }
    });
    connect(disputeNextBtn, &QPushButton::clicked, this, [this]() {
        loadDisputeData(getDisputeStatusParam(), m_disputeCurrentPage + 1);
    });

    return page;
}

void AdminMainWindow::onLogoutClicked() {
    QMessageBox::StandardButton reply = QMessageBox::question(this, "退出登录", "确定要退出管理员账号吗？");
    if (reply == QMessageBox::Yes) {
        // 1. 调用退出 API（可选，服务端会清除 token）
        ApiService::instance()->logout();

        // 2. 清除本地认证信息
        ApiService::instance()->clearAuthToken();
        ApiService::instance()->setCurrentUserId(-1);

        // 3. 关闭当前窗口
        this->close();

        // 4. 重新显示登录页
        LoginPage loginPage;
        if (loginPage.exec() == QDialog::Accepted) {
            QString selectedRole = loginPage.getSelectedRole();
            if (selectedRole == "admin") {
                AdminMainWindow *adminWindow = new AdminMainWindow();
                adminWindow->show();
            } else {
                MainWindow *userWindow = new MainWindow();
                userWindow->show();
            }
        } else {
            // 用户取消登录，退出整个应用程序
            QApplication::quit();
        }
    }
}

void AdminMainWindow::onTabChanged(int index) {
    // 根据标签页切换加载数据
    switch(index) {
    case 0: loadGoodsReviewData("", "全部", "", ""); break;
    case 1: loadUserManagementData(); break;
    case 2: loadDisputeData(); break;
    }
}

// 数据加载函数
void AdminMainWindow::loadGoodsReviewData(const QString& keyword, const QString& status,
                                          const QString& startDate, const QString& endDate,
                                          int page, int pageSize) {
    m_goodsCurrentPage = page;
    goodsPageInfoLabel->setText(QString("第 %1 页").arg(page));

    goodsReviewTable->setRowCount(0);

    QJsonArray goodsList = ApiService::instance()->getGoodsForReview(keyword, status, startDate, endDate, page, pageSize);
    for (const QJsonValue &val : goodsList) {
        QJsonObject goods = val.toObject();
        int goodsId = goods.value("id").toInt();
        QString name = goods.value("name").toString();
        QString seller = goods.value("seller_name").toString(); // 需确保服务端返回 seller_name
        double price = goods.value("price").toDouble();
        QString publishTime = goods.value("publish_time").toString();
        int statusCode = goods.value("status").toString().toInt();
        QString statusText;
        switch (statusCode) {
        case 0: statusText = "待审核"; break;
        case 1: statusText = "已上架"; break;
        case 2: statusText = "交易中"; break;
        case 3: statusText = "已售出"; break;
        case 4: statusText = "已拒绝"; break;
        case 5: statusText = "已下架"; break;
        default: statusText = "未知"; break;
        }

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
        
        // 通用的查看详情按钮
        QPushButton *viewBtn = new QPushButton("查看");
        viewBtn->setObjectName("secondaryBtn");
        viewBtn->setFixedSize(60, 30);
        connect(viewBtn, &QPushButton::clicked, [this, goodsId, name, seller, price, publishTime, statusCode]() {
            // 调用商品详情 API 获取完整信息（包括描述、分类、图片等）
            QJsonObject detail = ApiService::instance()->getGoodsDetail(goodsId);
            QString detailText;
            if (detail.value("success").toBool()) {
                QJsonObject data = detail.value("data").toObject();
                QString categoryName = data.value("category_name").toString();
                if (categoryName.isEmpty()) {
                    int catId = data.value("category_id").toInt();
                    categoryName = getCategoryName(catId); // 需要实现一个转换函数
                }
                QString description = data.value("description").toString();
                QString images = data.value("images").toArray().size() > 0 ? "有" : "无";
                QString statusText;
                switch (statusCode) {
                case 0: statusText = "待审核"; break;
                case 1: statusText = "已上架"; break;
                case 2: statusText = "交易中"; break;
                case 3: statusText = "已售出"; break;
                case 4: statusText = "已拒绝"; break;
                case 5: statusText = "已下架"; break;
                default: statusText = "未知";
                }
                detailText = QString(
                                 "📦 商品详情\n"
                                 "━━━━━━━━━━━━━━━━━━━━\n"
                                 "商品ID: %1\n"
                                 "商品名称: %2\n"
                                 "卖家: %3\n"
                                 "分类: %4\n"
                                 "价格: ¥%5\n"
                                 "发布时间: %6\n"
                                 "当前状态: %7\n"
                                 "商品描述:\n%8\n"
                                 "图片数量: %9"
                                 ).arg(goodsId).arg(name).arg(seller).arg(categoryName)
                                 .arg(price).arg(publishTime).arg(statusText)
                                 .arg(description.isEmpty() ? "无" : description).arg(images);
            } else {
                detailText = QString("商品ID: %1\n无法获取详细信息").arg(goodsId);
            }
            QMessageBox::information(this, "商品详情", detailText);
        });

        switch (statusCode) {
        case 0: // 待审核
        {
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

            actionLayout->addWidget(approveBtn);
            actionLayout->addWidget(rejectBtn);
            actionLayout->addWidget(viewBtn);
            break;
        }
        case 1: // 已上架
        {
            QPushButton *offShelfBtn = new QPushButton("下架");
            offShelfBtn->setObjectName("warningBtn");
            offShelfBtn->setFixedSize(60, 30);
            offShelfBtn->setProperty("goodsId", goodsId);
            connect(offShelfBtn, &QPushButton::clicked, [this, offShelfBtn]() {
                onOffShelfGoods(offShelfBtn->property("goodsId").toInt());
            });
            actionLayout->addWidget(offShelfBtn);
            actionLayout->addWidget(viewBtn);
            break;
        }
        case 4: // 已拒绝
        {
            QPushButton *reapplyBtn = new QPushButton("重新审核");
            reapplyBtn->setObjectName("primaryBtn");
            reapplyBtn->setFixedSize(80, 30);
            reapplyBtn->setProperty("goodsId", goodsId);
            connect(reapplyBtn, &QPushButton::clicked, [this, reapplyBtn]() {
                onReapplyGoods(reapplyBtn->property("goodsId").toInt());
            });
            actionLayout->addWidget(reapplyBtn);
            actionLayout->addWidget(viewBtn);
            break;
        }
        case 5: // 已下架
        {
            QPushButton *applyShelfBtn = new QPushButton("申请上架");
            applyShelfBtn->setObjectName("primaryBtn");
            applyShelfBtn->setFixedSize(80, 30);
            applyShelfBtn->setProperty("goodsId", goodsId);
            connect(applyShelfBtn, &QPushButton::clicked, [this, applyShelfBtn]() {
                onApplyShelfGoods(applyShelfBtn->property("goodsId").toInt());
            });
            actionLayout->addWidget(applyShelfBtn);
            actionLayout->addWidget(viewBtn);
            break;
        }
        case 2: // 交易中
        case 3: // 已售出
        default:
        {
            // 仅查看
            actionLayout->addWidget(viewBtn);
            break;
        }
        }
        actionLayout->addStretch();
        goodsReviewTable->setCellWidget(row, 6, actionWidget);
    }

    // 根据返回数量判断是否有下一页（如果返回数量小于 pageSize，则禁用下一页）
    bool hasMore = (goodsList.size() == pageSize);
    goodsNextBtn->setEnabled(hasMore);
    goodsPrevBtn->setEnabled(page > 1);
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
        QString keyword = goodsSearchEdit->text().trimmed();
        QString status = goodsStatusCombo->currentText();
        QString startDate = goodsDateFromEdit->date().toString("yyyy-MM-dd");
        QString endDate = goodsDateToEdit->date().toString("yyyy-MM-dd");
        loadGoodsReviewData(keyword, status, startDate, endDate);
    } else {
        QMessageBox::warning(this, "审核失败", result.value("error").toString());
    }
}

void AdminMainWindow::onFilterGoods()
{
    QString keyword = goodsSearchEdit->text().trimmed();
    QString status = goodsStatusCombo->currentText();
    QString startDate = goodsDateFromEdit->date().toString("yyyy-MM-dd");
    QString endDate = goodsDateToEdit->date().toString("yyyy-MM-dd");

    loadGoodsReviewData(keyword, status, startDate, endDate,1);
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

void AdminMainWindow::onAdjustCreditScore(int userId, int newScore) {
    bool ok;
    QString reason = QInputDialog::getText(this, "调整原因",
                                           "请输入调整原因:",
                                           QLineEdit::Normal,
                                           "管理员手动调整", &ok);
    if (!ok) return;
    QJsonObject result = ApiService::instance()->updateUserCreditScore(userId, newScore, reason);
    if (result.value("success").toBool()) {
        QMessageBox::information(this, "调整成功", "信用分已更新");
        loadUserManagementData();  // 刷新表格
    } else {
        QMessageBox::warning(this, "调整失败", result.value("error").toString());
    }
}

void AdminMainWindow::onSearchUsers() {
    QString keyword = userSearchEdit->text();
    QString status = userStatusCombo->currentText();

    loadUserManagementData(keyword, status, 1, 20);
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
    loadDisputeData(getDisputeStatusParam(), 1, 20);
}

void AdminMainWindow::loadUserManagementData(const QString& keyword, const QString& status, int page, int pageSize)  {
    m_userCurrentPage = page;
    userPageInfoLabel->setText(QString("第 %1 页").arg(page));

    userTable->setRowCount(0);

    QString apiStatus;
    if (status == "正常") apiStatus = "1";
    else if (status == "已封禁") apiStatus = "0";
    else apiStatus = ""; // 全部

    QJsonArray users = ApiService::instance()->getUserList(apiStatus, keyword, "0", page, pageSize);
    for (const QJsonValue &val : users) {
        QJsonObject user = val.toObject();
        int userId = user.value("id").toInt();
        QString username = user.value("account").toString();
        QString phone = user.value("phone").toString();
        QString regTime = user.value("register_time").toString();
        int creditScore = user.value("credit_score").toInt();
        int status = user.value("status").toString().toInt(); // 1=正常, 0=禁用

        int row = userTable->rowCount();
        userTable->insertRow(row);
        userTable->setRowHeight(row, 42);
        userTable->setItem(row, 0, new QTableWidgetItem(QString::number(userId)));
        userTable->setItem(row, 1, new QTableWidgetItem(username));
        userTable->setItem(row, 2, new QTableWidgetItem(phone));
        userTable->setItem(row, 3, new QTableWidgetItem(regTime));
        userTable->setItem(row, 4, new QTableWidgetItem(QString::number(creditScore)));

        QString statusText = (status == 1) ? "正常" : "已封禁";
        QTableWidgetItem *statusItem = new QTableWidgetItem(statusText);
        if (status == 0) statusItem->setForeground(QColor(231, 76, 60));
        userTable->setItem(row, 5, statusItem);

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
                onAdjustCreditScore(adjustBtn->property("userId").toString().toInt(), newScore);
            }
        });
        actionLayout->addWidget(adjustBtn);
        actionLayout->addStretch();
        userTable->setCellWidget(row, 6, actionWidget);
    }
    // 更新按钮状态
    bool hasMore = (users.size() == pageSize);
    userNextBtn->setEnabled(hasMore);
    userPrevBtn->setEnabled(page > 1);
}

void AdminMainWindow::loadDisputeData(const QString& status, int page, int pageSize)  {
    m_disputeCurrentPage = page;
    disputePageInfoLabel->setText(QString("第 %1 页").arg(page));

    disputeTable->setRowCount(0);
    QJsonArray disputes = ApiService::instance()->getDisputeList(status, page, pageSize);
    for (const QJsonValue &val : disputes) {
        QJsonObject dispute = val.toObject();
        int disputeId = dispute.value("id").toInt();
        int orderId = dispute.value("order_id").toInt();
        QString complainant = dispute.value("complainant_name").toString(); // 需服务端返回
        QString defendant = dispute.value("defendant_name").toString();
        QString type = dispute.value("type").toString();
        QString createTime = dispute.value("create_time").toString();
        int status = dispute.value("status").toString().toInt(); // 0待处理,1处理中,2已解决,3已关闭
        QString progress = dispute.value("handle_result").toString();

        int row = disputeTable->rowCount();
        disputeTable->insertRow(row);
        disputeTable->setRowHeight(row, 42);
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
            connect(viewBtn, &QPushButton::clicked, [this, disputeId, orderId, complainant, defendant, type, createTime, status, progress]() {
                QJsonObject result = ApiService::instance()->getDisputeDetail(disputeId);
                QString detailText;
                if (result.value("success").toBool()) {
                    QJsonObject data = result.value("data").toObject();
                    QString description = data.value("description").toString();
                    QString evidence = data.value("evidence_urls").toString();
                    QString handleResult = data.value("handle_result").toString();
                    QString statusText;
                    int st = data.value("status").toString().toInt();
                    if (st == 0) statusText = "待处理";
                    else if (st == 1) statusText = "处理中";
                    else if (st == 2) statusText = "已解决";
                    else statusText = "已关闭";

                    detailText = QString(
                                     "⚖️ 纠纷详情\n"
                                     "━━━━━━━━━━━━━━━━━━━━\n"
                                     "纠纷ID: %1\n"
                                     "订单号: %2\n"
                                     "投诉方: %3\n"
                                     "被投诉方: %4\n"
                                     "纠纷类型: %5\n"
                                     "提交时间: %6\n"
                                     "当前状态: %7\n"
                                     "处理进度: %8\n"
                                     "详细描述:\n%9\n"
                                     "证据材料:\n%10"
                                     ).arg(disputeId).arg(orderId).arg(complainant).arg(defendant)
                                     .arg(type).arg(createTime).arg(statusText).arg(handleResult.isEmpty() ? "无" : handleResult)
                                     .arg(description.isEmpty() ? "无" : description)
                                     .arg(evidence.isEmpty() ? "无" : evidence);
                } else {
                    detailText = QString("纠纷ID: %1\n无法获取详细信息").arg(disputeId);
                }
                QMessageBox::information(this, "纠纷详情", detailText);
            });
            actionLayout->addWidget(viewBtn);
        }
        actionLayout->addStretch();
        disputeTable->setCellWidget(row, 8, actionWidget);
    }

    bool hasMore = (disputes.size() == pageSize);
    disputeNextBtn->setEnabled(hasMore);
    disputePrevBtn->setEnabled(page > 1);
}

void AdminMainWindow::onOffShelfGoods(int goodsId)
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "确认下架", "确定要将该商品下架吗？下架后买家将无法购买。");
    if (reply == QMessageBox::Yes) {
        QJsonObject result = ApiService::instance()->updateGoodsStatus(goodsId, 5); // 5 = 已下架
        if (result.value("success").toBool()) {
            QMessageBox::information(this, "成功", "商品已下架");
            // 刷新当前列表
            onFilterGoods();
        } else {
            QMessageBox::warning(this, "失败", result.value("error").toString());
        }
    }
}

void AdminMainWindow::onReapplyGoods(int goodsId)
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "重新审核", "确定要将该商品重新提交审核吗？");
    if (reply == QMessageBox::Yes) {
        QJsonObject result = ApiService::instance()->updateGoodsStatus(goodsId, 0); // 0 = 待审核
        if (result.value("success").toBool()) {
            QMessageBox::information(this, "成功", "已重新提交审核");
            onFilterGoods();
        } else {
            QMessageBox::warning(this, "失败", result.value("error").toString());
        }
    }
}

void AdminMainWindow::onApplyShelfGoods(int goodsId)
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "上架", "确定要上架该商品吗？申请后将进入待审核状态。");
    if (reply == QMessageBox::Yes) {
        QJsonObject result = ApiService::instance()->updateGoodsStatus(goodsId, 0); // 0 = 待审核
        if (result.value("success").toBool()) {
            QMessageBox::information(this, "成功", "已提交上架申请，等待审核");
            onFilterGoods();
        } else {
            QMessageBox::warning(this, "失败", result.value("error").toString());
        }
    }
}

QString AdminMainWindow::getDisputeStatusParam() {
    QString status = disputeStatusCombo->currentText();
    if (status == "待处理") return "0";
    else if (status == "已解决") return "2";
    else return "";
}

QString AdminMainWindow::getCategoryName(int categoryId) {
    switch (categoryId) {
    case 1: return "书籍教材";
    case 2: return "电子产品";
    case 3: return "服饰鞋包";
    case 4: return "生活用品";
    case 5: return "体育器材";
    case 6: return "学习工具";
    case 7: return "美妆个护";
    default: return "其他";
    }
}

AdminMainWindow::~AdminMainWindow() {}
