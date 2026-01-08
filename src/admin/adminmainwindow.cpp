// adminmainwindow.cpp - 由于代码过长，这里只展示核心部分
#include "adminmainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QDateTime>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QMenu>
#include <QAction>

AdminMainWindow::AdminMainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("校园二手交易系统 - 管理员后台");
    setMinimumSize(1400, 900);

    setupUI();
    // loadDashboardData();

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
    QAction *exitAction = systemMenu->addAction("退出系统");
    connect(logoutAction, &QAction::triggered, this, &AdminMainWindow::onLogoutClicked);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    // 工具菜单
    QMenu *toolsMenu = menuBar->addMenu("工具");
    toolsMenu->addAction("数据备份");
    toolsMenu->addAction("日志查看");
    toolsMenu->addAction("系统设置");

    // 帮助菜单
    QMenu *helpMenu = menuBar->addMenu("帮助");
    helpMenu->addAction("使用手册");
    helpMenu->addAction("关于系统");

    // 工具栏
    QToolBar *toolBar = addToolBar("主工具栏");
    toolBar->addAction("刷新");
    toolBar->addAction("导出");
    toolBar->addSeparator();
    toolBar->addAction("帮助");

    // 主标签页
    mainTabWidget = new QTabWidget(mainWidget);
    mainTabWidget->setTabPosition(QTabWidget::North);

    // 创建各个页面
    QWidget *dashboardPage = createDashboardPage();
    QWidget *goodsReviewPage = createGoodsReviewPage();
    QWidget *userManagementPage = createUserManagementPage();
    QWidget *disputeManagementPage = createDisputeManagementPage();
    QWidget *statisticsPage = createStatisticsPage();

    mainTabWidget->addTab(dashboardPage, QIcon(":/icons/dashboard.png"), "仪表盘");
    mainTabWidget->addTab(goodsReviewPage, QIcon(":/icons/review.png"), "商品审核");
    mainTabWidget->addTab(userManagementPage, QIcon(":/icons/users.png"), "用户管理");
    mainTabWidget->addTab(disputeManagementPage, QIcon(":/icons/dispute.png"), "纠纷处理");
    mainTabWidget->addTab(statisticsPage, QIcon(":/icons/statistics.png"), "数据统计");

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
            padding: 8px 16px;
        }
        #successBtn {
            background-color: #27ae60;
            color: white;
            border-radius: 4px;
            padding: 8px 16px;
        }
        #warningBtn {
            background-color: #e74c3c;
            color: white;
            border-radius: 4px;
            padding: 8px 16px;
        }
        #secondaryBtn {
            background-color: #95a5a6;
            color: white;
            border-radius: 4px;
            padding: 8px 16px;
        }
    )");
}

QWidget* AdminMainWindow::createDashboardPage() {
    QWidget *page = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // 顶部统计卡片
    QWidget *statsWidget = new QWidget();
    QHBoxLayout *statsLayout = new QHBoxLayout(statsWidget);
    statsLayout->setContentsMargins(0, 0, 0, 0);
    statsLayout->setSpacing(15);

    struct StatCard {
        QString title;
        QString value;
        QString icon;
        QColor color;
    };

    QList<StatCard> statCards = {
        {"总用户数", "1,234", "👥", QColor(52, 152, 219)},
        {"商品总数", "5,678", "📦", QColor(46, 204, 113)},
        {"交易总额", "¥89,012", "💰", QColor(155, 89, 182)},
        {"待审核商品", "23", "⏳", QColor(241, 196, 15)},
        {"待处理纠纷", "12", "⚖️", QColor(230, 126, 34)},
        {"平台增长率", "15.6%", "📈", QColor(231, 76, 60)}
    };

    for (const auto &card : statCards) {
        QWidget *cardWidget = new QWidget();
        cardWidget->setFixedSize(200, 120);
        cardWidget->setStyleSheet(QString(R"(
            background-color: white;
            border-radius: 12px;
            border-left: 5px solid %1;
            box-shadow: 0 2px 8px rgba(0,0,0,0.1);
        )").arg(card.color.name()));

        QVBoxLayout *cardLayout = new QVBoxLayout(cardWidget);
        cardLayout->setContentsMargins(15, 15, 15, 15);

        QLabel *iconLabel = new QLabel(card.icon);
        iconLabel->setStyleSheet("font-size: 24px;");

        QLabel *titleLabel = new QLabel(card.title);
        titleLabel->setStyleSheet("color: #666; font-size: 14px;");

        QLabel *valueLabel = new QLabel(card.value);
        valueLabel->setStyleSheet(QString("color: %1; font-size: 28px; font-weight: bold;").arg(card.color.name()));

        if (card.title == "平台增长率") {
            platformGrowthBar = new QProgressBar();
            platformGrowthBar->setRange(0, 100);
            platformGrowthBar->setValue(15);
            platformGrowthBar->setTextVisible(true);
            platformGrowthBar->setFormat("%p%");
            platformGrowthBar->setStyleSheet(R"(
                QProgressBar {
                    height: 8px;
                    border-radius: 4px;
                    background-color: #ecf0f1;
                }
                QProgressBar::chunk {
                    border-radius: 4px;
                    background-color: #e74c3c;
                }
            )");
            cardLayout->addWidget(platformGrowthBar);
        }

        cardLayout->addWidget(iconLabel);
        cardLayout->addWidget(valueLabel);
        cardLayout->addWidget(titleLabel);
        statsLayout->addWidget(cardWidget);
    }

    statsLayout->addStretch();
    mainLayout->addWidget(statsWidget);

    // 图表区域
    QWidget *chartWidget = new QWidget();
    QHBoxLayout *chartLayout = new QHBoxLayout(chartWidget);
    chartLayout->setContentsMargins(0, 0, 0, 0);
    chartLayout->setSpacing(20);

    // 收入趋势图
    QGroupBox *revenueGroup = new QGroupBox("平台收入趋势");
    QVBoxLayout *revenueLayout = new QVBoxLayout(revenueGroup);
    revenueChartView = new QChartView();
    revenueChartView->setMinimumHeight(300);
    revenueLayout->addWidget(revenueChartView);

    // 商品分类分布图
    QGroupBox *categoryGroup = new QGroupBox("商品分类分布");
    QVBoxLayout *categoryLayout = new QVBoxLayout(categoryGroup);
    categoryChartView = new QChartView();
    categoryChartView->setMinimumHeight(300);
    categoryLayout->addWidget(categoryChartView);

    chartLayout->addWidget(revenueGroup, 1);
    chartLayout->addWidget(categoryGroup, 1);
    mainLayout->addWidget(chartWidget, 1);

    return page;
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

QWidget* AdminMainWindow::createStatisticsPage() {
    QWidget *page = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // 时间筛选工具栏
    QWidget *dateWidget = new QWidget();
    QHBoxLayout *dateLayout = new QHBoxLayout(dateWidget);
    dateLayout->setContentsMargins(0, 0, 0, 0);
    dateLayout->setSpacing(10);

    dateLayout->addWidget(new QLabel("统计时间:"));
    statDateFromEdit = new QDateEdit();
    statDateFromEdit->setDate(QDate::currentDate().addDays(-30));
    statDateFromEdit->setDisplayFormat("yyyy-MM-dd");
    statDateFromEdit->setFixedWidth(120);
    dateLayout->addWidget(statDateFromEdit);

    dateLayout->addWidget(new QLabel("至"));
    statDateToEdit = new QDateEdit();
    statDateToEdit->setDate(QDate::currentDate());
    statDateToEdit->setDisplayFormat("yyyy-MM-dd");
    statDateToEdit->setFixedWidth(120);
    dateLayout->addWidget(statDateToEdit);

    dateLayout->addWidget(new QLabel("统计类型:"));
    statTypeCombo = new QComboBox();
    statTypeCombo->addItems({"交易统计", "用户统计", "商品统计", "纠纷统计"});
    dateLayout->addWidget(statTypeCombo);

    QPushButton *updateBtn = new QPushButton("更新统计");
    updateBtn->setObjectName("primaryBtn");
    dateLayout->addWidget(updateBtn);

    QPushButton *exportBtn = new QPushButton("导出报表");
    exportBtn->setObjectName("secondaryBtn");
    dateLayout->addWidget(exportBtn);

    dateLayout->addStretch();

    connect(updateBtn, &QPushButton::clicked, this, &AdminMainWindow::onDateRangeChanged);
    connect(exportBtn, &QPushButton::clicked, this, &AdminMainWindow::onExportStatistics);

    // 图表区域
    QWidget *chartWidget = new QWidget();
    QHBoxLayout *chartLayout = new QHBoxLayout(chartWidget);
    chartLayout->setContentsMargins(0, 0, 0, 0);
    chartLayout->setSpacing(20);

    // 日度统计图
    QGroupBox *dailyGroup = new QGroupBox("日度交易趋势");
    QVBoxLayout *dailyLayout = new QVBoxLayout(dailyGroup);
    dailyChartView = new QChartView();
    dailyChartView->setMinimumHeight(300);
    dailyLayout->addWidget(dailyChartView);

    // 周度统计图
    QGroupBox *weeklyGroup = new QGroupBox("周度分布");
    QVBoxLayout *weeklyLayout = new QVBoxLayout(weeklyGroup);
    weeklyChartView = new QChartView();
    weeklyChartView->setMinimumHeight(300);
    weeklyLayout->addWidget(weeklyChartView);

    chartLayout->addWidget(dailyGroup, 1);
    chartLayout->addWidget(weeklyGroup, 1);

    // 排行榜区域
    QWidget *rankWidget = new QWidget();
    QHBoxLayout *rankLayout = new QHBoxLayout(rankWidget);
    rankLayout->setContentsMargins(0, 0, 0, 0);
    rankLayout->setSpacing(20);

    // 活跃用户榜
    QGroupBox *topUsersGroup = new QGroupBox("活跃用户Top10");
    QVBoxLayout *topUsersLayout = new QVBoxLayout(topUsersGroup);
    topUsersTable = new QTableWidget(0, 4);
    topUsersTable->setHorizontalHeaderLabels({"排名", "用户名", "交易次数", "总金额"});
    topUsersTable->verticalHeader()->setVisible(false);
    topUsersTable->setMaximumHeight(300);
    topUsersLayout->addWidget(topUsersTable);

    // 热门商品榜
    QGroupBox *topGoodsGroup = new QGroupBox("热门商品Top10");
    QVBoxLayout *topGoodsLayout = new QVBoxLayout(topGoodsGroup);
    topGoodsTable = new QTableWidget(0, 4);
    topGoodsTable->setHorizontalHeaderLabels({"排名", "商品名称", "销量", "销售额"});
    topGoodsTable->verticalHeader()->setVisible(false);
    topGoodsTable->setMaximumHeight(300);
    topGoodsLayout->addWidget(topGoodsTable);

    rankLayout->addWidget(topUsersGroup, 1);
    rankLayout->addWidget(topGoodsGroup, 1);

    mainLayout->addWidget(dateWidget);
    mainLayout->addWidget(chartWidget, 1);
    mainLayout->addWidget(rankWidget);

    return page;
}

// 其他槽函数实现...
void AdminMainWindow::onLogoutClicked() {
    QMessageBox::StandardButton reply = QMessageBox::question(this, "退出登录", "确定要退出管理员账号吗？");
    if (reply == QMessageBox::Yes) {
        close();
    }
}

void AdminMainWindow::onTabChanged(int index) {
    // 根据标签页切换加载数据
    switch(index) {
    case 0: loadDashboardData(); break;
    case 1: loadGoodsReviewData(); break;
    case 2: loadUserManagementData(); break;
    case 3: loadDisputeData(); break;
    case 4: loadStatisticsData(); break;
    }
}

// 数据加载函数...
void AdminMainWindow::loadGoodsReviewData() {
    // 清空表格
    goodsReviewTable->setRowCount(0);

    // 模拟数据
    QStringList goodsData = {
        "1001,二手iPhone 12 128GB,张三同学,¥2500,2024-03-20 10:30,待审核",
        "1002,大学物理教材,李四同学,¥35,2024-03-19 14:20,已通过",
        "1003,篮球鞋 Nike Air,王五同学,¥280,2024-03-18 09:15,待审核",
        "1004,笔记本电脑戴尔,赵六同学,¥3200,2024-03-17 16:45,已拒绝",
        "1005,小米手环6,钱七同学,¥150,2024-03-16 11:20,待审核"
    };

    for (const QString &data : goodsData) {
        QStringList items = data.split(',');
        int row = goodsReviewTable->rowCount();
        goodsReviewTable->insertRow(row);

        for (int col = 0; col < items.size(); col++) {
            QTableWidgetItem *item = new QTableWidgetItem(items[col]);

            // 设置状态颜色
            if (col == 5) { // 状态列
                if (items[col] == "待审核") {
                    item->setForeground(QColor(230, 126, 34)); // 橙色
                    item->setFont(QFont("", -1, QFont::Bold));
                } else if (items[col] == "已通过") {
                    item->setForeground(QColor(46, 204, 113)); // 绿色
                } else if (items[col] == "已拒绝") {
                    item->setForeground(QColor(231, 76, 60)); // 红色
                }
            }

            goodsReviewTable->setItem(row, col, item);
        }

        // 添加操作按钮
        QWidget *actionWidget = new QWidget();
        QHBoxLayout *actionLayout = new QHBoxLayout(actionWidget);
        actionLayout->setContentsMargins(5, 2, 5, 2);
        actionLayout->setSpacing(5);

        if (items[5] == "待审核") {
            QPushButton *approveBtn = new QPushButton("通过");
            approveBtn->setObjectName("successBtn");
            approveBtn->setFixedSize(60, 25);
            approveBtn->setProperty("goodsId", items[0].toInt());
            connect(approveBtn, &QPushButton::clicked, [this, approveBtn]() {
                onReviewGoods(approveBtn->property("goodsId").toInt(), true);
            });

            QPushButton *rejectBtn = new QPushButton("拒绝");
            rejectBtn->setObjectName("warningBtn");
            rejectBtn->setFixedSize(60, 25);
            rejectBtn->setProperty("goodsId", items[0].toInt());
            connect(rejectBtn, &QPushButton::clicked, [this, rejectBtn]() {
                onReviewGoods(rejectBtn->property("goodsId").toInt(), false);
            });

            QPushButton *viewBtn = new QPushButton("查看");
            viewBtn->setObjectName("secondaryBtn");
            viewBtn->setFixedSize(60, 25);

            actionLayout->addWidget(approveBtn);
            actionLayout->addWidget(rejectBtn);
            actionLayout->addWidget(viewBtn);
        } else {
            QPushButton *viewBtn = new QPushButton("查看详情");
            viewBtn->setObjectName("secondaryBtn");
            viewBtn->setFixedSize(80, 25);
            actionLayout->addWidget(viewBtn);
        }

        actionLayout->addStretch();
        goodsReviewTable->setCellWidget(row, 6, actionWidget);
    }
}

// 商品审核相关
void AdminMainWindow::onReviewGoods(int goodsId, bool approve) {
    QString status = approve ? "已通过" : "已拒绝";
    QMessageBox::information(this, "审核结果",
                             QString("商品ID %1 已%2").arg(goodsId).arg(status));

    // 重新加载数据
    loadGoodsReviewData();
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
    QString action = block ? "封禁" : "解封";
    QMessageBox::information(this, "用户状态变更",
                             QString("用户ID %1 已%2").arg(userId).arg(action));
}

void AdminMainWindow::onAdjustCreditScore(const QString &userId, int score) {
    QMessageBox::information(this, "信用分调整",
                             QString("用户ID %1 的信用分已调整为 %2").arg(userId).arg(score));
}

void AdminMainWindow::onSearchUsers() {
    QString keyword = userSearchEdit->text();
    QString status = userStatusCombo->currentText();
    QString creditLevel = userCreditLevelCombo->currentText();

    QMessageBox::information(this, "搜索条件",
                             QString("关键词: %1\n状态: %2\n信用等级: %3")
                                 .arg(keyword).arg(status).arg(creditLevel));

    // 这里应该实现具体的搜索逻辑
    // loadUserManagementData();
}

// 纠纷处理相关
void AdminMainWindow::onViewDisputeDetail(int disputeId) {
    QMessageBox::information(this, "纠纷详情",
                             QString("查看纠纷ID %1 的详细信息").arg(disputeId));
}

void AdminMainWindow::onProcessDispute(int disputeId, QString decision) {
    QMessageBox::information(this, "纠纷处理",
                             QString("纠纷ID %1 已处理\n处理结果: %2").arg(disputeId).arg(decision));
}

void AdminMainWindow::onFilterDisputes() {
    QString type = disputeTypeCombo->currentText();
    QString status = disputeStatusCombo->currentText();
    QString date = disputeDateEdit->date().toString("yyyy-MM-dd");

    QMessageBox::information(this, "筛选条件",
                             QString("纠纷类型: %1\n处理状态: %2\n提交日期: %3之后")
                                 .arg(type).arg(status).arg(date));

    // loadDisputeData();
}

// 数据统计相关
void AdminMainWindow::onDateRangeChanged() {
    QString dateFrom = statDateFromEdit->date().toString("yyyy-MM-dd");
    QString dateTo = statDateToEdit->date().toString("yyyy-MM-dd");
    QString type = statTypeCombo->currentText();

    QMessageBox::information(this, "更新统计",
                             QString("统计时间: %1 至 %2\n统计类型: %3")
                                 .arg(dateFrom).arg(dateTo).arg(type));

    loadStatisticsData();
}

void AdminMainWindow::onExportStatistics() {
    QString fileName = QFileDialog::getSaveFileName(this, "导出报表",
                                                    "统计报表.xlsx", "Excel文件 (*.xlsx)");
    if (!fileName.isEmpty()) {
        QMessageBox::information(this, "导出成功",
                                 QString("报表已导出到: %1").arg(fileName));
    }
}

// 数据加载函数
void AdminMainWindow::loadDashboardData() {
    // 更新统计卡片
    totalUsersLabel->setText("1,234");
    totalGoodsLabel->setText("5,678");
    // 其他标签更新...

    // 这里应该实现图表数据的加载
    // 暂时用空实现
}

void AdminMainWindow::loadUserManagementData() {
    // 清空表格
    userTable->setRowCount(0);

    // 模拟数据
    QStringList userData = {
        "1001,张三,20210001,13800138001,2023-09-01 10:30,95,正常",
        "1002,李四,20210002,13800138002,2023-09-02 14:20,88,正常",
        "1003,王五,20210003,13800138003,2023-09-03 09:15,76,已封禁",
        "1004,赵六,20210004,13800138004,2023-09-04 16:45,92,限制交易",
        "1005,钱七,20210005,13800138005,2023-09-05 11:20,81,正常"
    };

    for (const QString &data : userData) {
        QStringList items = data.split(',');
        int row = userTable->rowCount();
        userTable->insertRow(row);

        for (int col = 0; col < items.size(); col++) {
            QTableWidgetItem *item = new QTableWidgetItem(items[col]);
            userTable->setItem(row, col, item);
        }
    }
}

void AdminMainWindow::loadDisputeData() {
    // 类似loadGoodsReviewData的实现
    // 暂时用空实现
}

void AdminMainWindow::loadStatisticsData() {
    // 这里应该实现统计数据的加载和图表更新
    // 暂时用空实现
}

AdminMainWindow::~AdminMainWindow() {}
