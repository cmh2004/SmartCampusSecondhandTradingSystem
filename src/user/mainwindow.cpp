#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QDate>
#include <QTime>
#include <QInputDialog>
#include <QScrollArea>
#include <QGraphicsDropShadowEffect>
#include "MainWindow.h"
#include "GoodsDetailDialog.h"
#include "ChatDialog.h"
#include "DisputeSubmitDialog.h"
#include "paymentdialog.h"
#include "reviewdialog.h"
#include "profileeditdialog.h"
#include "creditscoredialog.h"
#include "reportsubmitdialog.h"
#include "disputesubmitdialog.h"  // 售后纠纷对话框

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),isDragging(false) {
    // 设置无边框窗口
    setWindowFlags(Qt::FramelessWindowHint);
    setWindowTitle("校园二手商品智能交易系统");
    setMinimumSize(1200, 800);
    setWindowIcon(QIcon(":/icons/img/app.png"));

    setupUI();
    loadMockData();

    // 连接信号槽
    connect(categoryList, &QListWidget::itemClicked, this, &MainWindow::onCategoryClicked);
    connect(searchBtn, &QPushButton::clicked, this, &MainWindow::onSearchClicked);
    connect(goodsTable, &QTableWidget::cellDoubleClicked, this, &MainWindow::onShowGoodsDetail);
    connect(mainTabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
}

void MainWindow::setupUI() {
    // 创建主窗口部件
    mainWidget = new QWidget(this);
    mainWidget->setObjectName("mainContainer");
    mainWidget->setStyleSheet(R"(
        #mainContainer {
            background-color: white;
            border-radius: 12px;
            border: 1px solid #e2e8f0;
        }
    )");

    // 为整个容器添加阴影效果
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(20);
    shadowEffect->setColor(QColor(0, 0, 0, 30));
    shadowEffect->setOffset(0, 5);
    mainWidget->setGraphicsEffect(shadowEffect);

    // 创建主布局
    QVBoxLayout *windowLayout = new QVBoxLayout(mainWidget);
    windowLayout->setContentsMargins(0, 0, 0, 0);
    windowLayout->setSpacing(0);

    // 添加自定义标题栏
    setupCustomTitleBar();
    windowLayout->addWidget(customTitleBar);

    // 创建主标签页
    mainTabWidget = new QTabWidget(mainWidget);
    mainTabWidget->setTabPosition(QTabWidget::North);
    mainTabWidget->setTabShape(QTabWidget::Rounded);
    mainTabWidget->tabBar()->hide();

    // 创建各个页面
    homePage = createHomePage();
    publishPage = createPublishPage();
    messagesPage = createMessagesPage();
    ordersPage = createOrdersPage();
    userCenterPage = createUserCenterPage();

    mainTabWidget->addTab(homePage, "");
    mainTabWidget->addTab(publishPage, "");
    mainTabWidget->addTab(messagesPage, "");
    mainTabWidget->addTab(ordersPage, "");
    mainTabWidget->addTab(userCenterPage, "");

    windowLayout->addWidget(mainTabWidget, 1);

    // 设置主窗口的中心部件
    setCentralWidget(mainWidget);

    // 设置状态栏
    QStatusBar *statusBar = this->statusBar();
    QLabel *statusLabel = new QLabel("欢迎使用校园二手交易系统");
    statusBar->addWidget(statusLabel);

    // 设置样式
    setStyleSheet(R"(
        QMainWindow {
            background-color: #f5f7fa;
        }
        QStatusBar {
            background-color: #f8f9fa;
            border-top: 1px solid #e2e8f0;
            border-bottom-left-radius: 12px;
            border-bottom-right-radius: 12px;
            padding: 5px;
        }
        QStatusBar::item {
            border: none;
        }
        /* 窗口控制按钮样式 */
        QPushButton#minimizeBtn, QPushButton#maximizeBtn, QPushButton#closeBtn {
            background-color: rgba(255, 255, 255, 0.2);
            color: white;
            border: none;
            border-radius: 4px;
            font-size: 14px;
            font-weight: bold;
            padding: 0;
            margin: 0;
        }
        QPushButton#minimizeBtn:hover {
            background-color: rgba(255, 255, 255, 0.3);
        }
        QPushButton#maximizeBtn:hover {
            background-color: rgba(255, 255, 255, 0.3);
        }
        QPushButton#closeBtn:hover {
            background-color: #e81123;
            color: white;
        }
        QTabWidget::pane {
            border: none;
            background-color: white;
            border-radius: 0 0 12px 12px;
        }
        QTableWidget {
            background-color: white;
            border: 1px solid #ddd;
            border-radius: 6px;
            gridline-color: #eee;
            alternate-background-color: #f9f9f9;
        }
        QTableWidget::item {
            padding: 8px;
        }
        QTableWidget::item:selected {
            background-color: #e3f2fd;
        }
        QListWidget {
            background-color: white;
            border: 1px solid #ddd;
            border-radius: 6px;
        }
        QListWidget::item {
            padding: 10px;
            border-bottom: 1px solid #eee;
        }
        QListWidget::item:selected {
            background-color: #e3f2fd;
            color: #1976d2;
        }
        QLineEdit, QTextEdit, QComboBox {
            border: 1px solid #ddd;
            border-radius: 4px;
            padding: 6px;
            background-color: white;
        }
        QLineEdit:focus, QTextEdit:focus, QComboBox:focus {
            border-color: #3498db;
            outline: none;
        }
        QPushButton {
            padding: 8px 16px;
            border-radius: 4px;
            border: none;
            font-weight: bold;
        }
        QPushButton#primaryBtn {
            background-color: #3498db;
            color: white;
        }
        QPushButton#primaryBtn:hover {
            background-color: #2980b9;
        }
        QPushButton#secondaryBtn {
            background-color: #7f8c8d;
            color: white;
        }
        /* 首页样式 */
        #searchBar {
            background-color: white;
            border-radius: 10px;
            padding: 15px;
        }

        #searchEdit {
            border: 2px solid #e2e8f0;
            border-radius: 8px;
            padding: 0 15px;
            font-size: 14px;
            background-color: white;
        }

        #searchEdit:focus {
            border-color: #3b82f6;
            outline: none;
        }

        #searchEdit::placeholder {
            color: #94a3b8;
        }

        #sortCombo {
            border: 2px solid #e2e8f0;
            border-radius: 8px;
            padding: 0 10px;
            font-size: 14px;
            background-color: white;
        }

        #sortCombo:hover {
            border-color: #cbd5e1;
        }

        #sortCombo::drop-down {
            border: none;
            width: 20px;
        }

        #welcomeLabel {
            font-size: 22px;
            font-weight: 700;
            color: #1e293b;
            padding: 10px 5px;
            background: linear-gradient(135deg, #3b82f6 0%, #8b5cf6 100%);
        }

        /* 分类区域样式 */
        #categoryWidget {
            background-color: white;
            border-radius: 12px;
            border: 1px solid #e2e8f0;
            padding: 15px;
        }

        #categoryTitle {
            font-size: 18px;
            font-weight: 600;
            color: #1e293b;
            padding-bottom: 10px;
            border-bottom: 2px solid #f1f5f9;
        }

        #categoryList {
            border: none;
            background-color: transparent;
            font-size: 14px;
        }

        #categoryList::item {
            padding: 12px 10px;
            border-radius: 8px;
            margin: 2px 0;
            color: #475569;
            border-left: 3px solid transparent;
        }

        #categoryList::item:hover {
            background-color: #f8fafc;
            color: #3b82f6;
        }

        #categoryList::item:selected {
            background-color: #eff6ff;
            color: #1d4ed8;
            font-weight: 500;
            border-left: 3px solid #3b82f6;
        }

        /* 商品区域样式 */
        #goodsWidget {
            background-color: white;
            border-radius: 12px;
            border: 1px solid #e2e8f0;
            padding: 20px;
        }

        /* 商品表格样式 */
        #goodsTable {
            background-color: white;
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            gridline-color: transparent;
            alternate-background-color: #f8fafc;
            font-size: 13px;
        }

        #goodsTable::item {
            padding: 12px 8px;
            border-bottom: 1px solid #f1f5f9;
        }

        #goodsTable::item:selected {
            background-color: #eff6ff;
            color: #1e293b;
            border-radius: 4px;
        }

        QHeaderView::section {
            background-color: #f8fafc;
            padding: 14px 8px;
            border: none;
            border-bottom: 2px solid #e2e8f0;
            font-weight: 600;
            color: #475569;
            font-size: 13px;
        }

        QHeaderView::section:first {
            border-top-left-radius: 8px;
        }

        QHeaderView::section:last {
            border-top-right-radius: 8px;
        }

        /* 滚动条美化 */
        QScrollBar:vertical {
            border: none;
            background: #f1f5f9;
            width: 8px;
            border-radius: 4px;
        }

        QScrollBar::handle:vertical {
            background: #cbd5e1;
            border-radius: 4px;
            min-height: 20px;
        }

        QScrollBar::handle:vertical:hover {
            background: #94a3b8;
        }

        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            border: none;
            background: none;
            height: 0px;
        }
        QPushButton#warningBtn {
            background-color: #e74c3c;
            color: white;
            border-radius: 4px;
            padding: 6px 12px;
            font-size: 13px;
            border: none;
        }

        QPushButton#warningBtn:hover {
            background-color: #c0392b;
        }

        QPushButton#warningBtn:pressed {
            background-color: #a93226;
        }
        /* 订单页面样式 */
        QTableWidget QPushButton {
            padding: 5px 10px;
            border-radius: 3px;
            font-size: 12px;
            border: 1px solid transparent;
        }

        QTableWidget QPushButton:hover {
            opacity: 0.9;
        }

        QTableWidget QPushButton:pressed {
            opacity: 0.8;
        }

        QTableWidget {
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            background-color: white;
            gridline-color: #f1f5f9;
        }

        QTableWidget::item {
            padding: 12px 8px;
            border-bottom: 1px solid #f1f5f9;
        }

        QTableWidget::item:selected {
            background-color: #e3f2fd;
            color: #1976d2;
            border-radius: 4px;
        }

        QHeaderView::section {
            background-color: #f8fafc;
            padding: 14px 8px;
            border: none;
            border-bottom: 2px solid #e2e8f0;
            font-weight: 600;
            color: #475569;
            font-size: 13px;
        }

        /* 订单状态标签样式 */
        #orderStatusLabel {
            padding: 3px 8px;
            border-radius: 10px;
            font-size: 12px;
            font-weight: bold;
        }
    )");
}

void MainWindow::setupCustomTitleBar() {
    // 创建自定义标题栏
    customTitleBar = new QWidget();
    customTitleBar->setFixedHeight(60);  // 标题栏高度
    customTitleBar->setObjectName("titleBar");
    customTitleBar->setStyleSheet(R"(
        #titleBar {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                        stop:0 #1e90ff, stop:1 #00bbcf);
            border-top-left-radius: 12px;
            border-top-right-radius: 12px;
            border-bottom: 1px solid #e2e8f0;
        }
    )");

    QHBoxLayout *titleLayout = new QHBoxLayout(customTitleBar);
    titleLayout->setContentsMargins(20, 0, 6, 0);
    titleLayout->setSpacing(15);
    setWindowFlags(windowFlags() | Qt::CustomizeWindowHint);

    // 左侧：图标和标题
    QLabel *iconLabel = new QLabel();
    iconLabel->setPixmap(QPixmap(":/icons/img/logo.png").scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    titleLabel = new QLabel("校园二手商品智能交易系统");
    titleLabel->setStyleSheet(R"(
        QLabel {
            color: white;
            font-size: 16px;
            font-weight: bold;
            padding-left: 10px;
        }
    )");

    // 中间：标签页按钮
    QWidget *tabButtonsWidget = new QWidget();
    QHBoxLayout *tabButtonsLayout = new QHBoxLayout(tabButtonsWidget);
    tabButtonsLayout->setContentsMargins(30, 0, 0, 0);
    tabButtonsLayout->setSpacing(5);

    // 创建标签按钮
    QStringList tabNames = {"首页", "发布商品", "消息", "我的订单", "个人中心"};
    QStringList tabIcons = {":/icons/img/home.png", ":/icons/img/publish.png",
                            ":/icons/img/message.png", ":/icons/img/order.png",
                            ":/icons/img/person.png"};

    QList<QPushButton*> tabButtons;
    for (int i = 0; i < tabNames.size(); ++i) {
        QPushButton *tabBtn = new QPushButton(tabNames[i]);
        tabBtn->setObjectName("tabButton");
        tabBtn->setCheckable(true);
        tabBtn->setIcon(QIcon(tabIcons[i]));
        tabBtn->setIconSize(QSize(20, 20));
        tabBtn->setFixedSize(110, 36);

        tabButtons.append(tabBtn);
        tabButtonsLayout->addWidget(tabBtn);
    }
    // 设置第一个按钮为选中状态
    if (!tabButtons.isEmpty()) {
        tabButtons[0]->setChecked(true);
    }

    // 连接标签页切换信号，更新按钮状态
    for (int i = 0; i < tabButtons.size(); ++i) {
        connect(tabButtons[i], &QPushButton::clicked, this, [this, i, tabButtons]() {
            // 先更新所有按钮状态
            for (int j = 0; j < tabButtons.size(); ++j) {
                tabButtons[j]->setChecked(j == i);
            }
            // 然后切换标签页
            mainTabWidget->setCurrentIndex(i);
        });
    }

    tabButtonsLayout->addStretch();

    // 右侧：窗口控制按钮
    QWidget *windowControls = new QWidget();
    QHBoxLayout *controlsLayout = new QHBoxLayout(windowControls);
    controlsLayout->setContentsMargins(0, 0, 0, 0);
    controlsLayout->setSpacing(5);

    minimizeBtn = new QPushButton("－");
    minimizeBtn->setObjectName("minimizeBtn");
    minimizeBtn->setFixedSize(40, 40);
    minimizeBtn->setToolTip("最小化");

    maximizeBtn = new QPushButton("□");
    maximizeBtn->setObjectName("maximizeBtn");
    maximizeBtn->setFixedSize(40, 40);
    maximizeBtn->setToolTip("最大化");

    closeBtn = new QPushButton("×");
    closeBtn->setObjectName("closeBtn");
    closeBtn->setFixedSize(40, 40);
    closeBtn->setToolTip("关闭");

    controlsLayout->addWidget(minimizeBtn);
    controlsLayout->addWidget(maximizeBtn);
    controlsLayout->addWidget(closeBtn);

    // 连接窗口控制按钮信号
    connect(minimizeBtn, &QPushButton::clicked, this, &QMainWindow::showMinimized);
    connect(maximizeBtn, &QPushButton::clicked, this, [this]() {
        if (isMaximized()) {
            showNormal();
            maximizeBtn->setText("□");
        } else {
            showMaximized();
            maximizeBtn->setText("❐");
        }
    });
    connect(closeBtn, &QPushButton::clicked, this, &QMainWindow::close);

    // 添加所有部件到标题栏布局
    titleLayout->addWidget(iconLabel);
    titleLayout->addWidget(titleLabel);
    titleLayout->addWidget(tabButtonsWidget, 1);  // 标签按钮占用剩余空间
    titleLayout->addWidget(windowControls);

    // 设置标签按钮样式
    QString tabButtonStyle = R"(
        QPushButton#tabButton {
            background-color: rgba(255, 255, 255, 0.2);
            color: white;
            border: none;
            border-radius: 6px;
            padding: 8px 12px;
            font-size: 14px;
            font-weight: 500;
        }
        QPushButton#tabButton:hover {
            background-color: rgba(255, 255, 255, 0.3);
        }
        QPushButton#tabButton:checked {
            background-color: white;
            color: #1e90ff;
        }
        QPushButton#tabButton:checked:hover {
            background-color: #f0f8ff;
        }
    )";

    QString windowButtonStyle = R"(
        QPushButton#minimizeBtn, QPushButton#maximizeBtn, QPushButton#closeBtn {
            background-color: transparent;
            border: none;
            border-radius: 0;
            padding: 0;
            margin: 0;
            min-width: 32px;
            min-height: 32px;
            color: white;
            font-size: 16px;
            font-weight: bold;
        }

        QPushButton#minimizeBtn:hover {
            background-color: rgba(255, 255, 255, 0.15);
            border-radius: 4px;
        }

        QPushButton#maximizeBtn:hover {
            background-color: rgba(255, 255, 255, 0.15);
            border-radius: 4px;
        }

        QPushButton#closeBtn:hover {
            background-color: #ff3b30;
            color: white;
            border-radius: 4px;
        }

        QPushButton#closeBtn:pressed {
            background-color: #d70015;
        }

        QPushButton#minimizeBtn:pressed, QPushButton#maximizeBtn:pressed {
            background-color: rgba(255, 255, 255, 0.25);
        }
    )";

    customTitleBar->setStyleSheet(customTitleBar->styleSheet() + tabButtonStyle + windowButtonStyle);
}

// 鼠标按下事件 - 用于窗口拖拽
void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        // 将鼠标位置转换为标题栏的局部坐标
        QPoint titleBarPos = customTitleBar->mapFromParent(event->pos());

        // 检查是否在标题栏区域内点击
        if (customTitleBar->rect().contains(titleBarPos)) {
            isDragging = true;
            dragStartPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
            event->accept();
        }
    }
}

// 鼠标移动事件 - 实现窗口拖拽
void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    if (isDragging && (event->buttons() & Qt::LeftButton)) {
        QPoint targetPos = event->globalPosition().toPoint() - dragStartPosition;

        // 获取当前屏幕
        QScreen *screen = QApplication::screenAt(targetPos);
        if (!screen) {
            screen = QApplication::primaryScreen();
        }

        QRect screenRect = screen->availableGeometry();
        QSize windowSize = size();

        // 限制窗口在屏幕内
        targetPos.setX(qMax(screenRect.left(),
                            qMin(targetPos.x(),
                                 screenRect.right() - windowSize.width())));
        targetPos.setY(qMax(screenRect.top(),
                            qMin(targetPos.y(),
                                 screenRect.bottom() - windowSize.height())));

        move(targetPos);
        event->accept();
        return;
    }
    QMainWindow::mouseMoveEvent(event);
}

// 鼠标释放事件
void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
        event->accept();
    }
}

QWidget* MainWindow::createHomePage() {
    QWidget *page = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // 顶部搜索栏
    QWidget *searchBar = new QWidget();
    searchBar->setObjectName("searchBar");
    QHBoxLayout *searchLayout = new QHBoxLayout(searchBar);
    searchLayout->setContentsMargins(0, 0, 0, 0);
    searchLayout->setSpacing(12);

    searchEdit = new QLineEdit();
    searchEdit->setPlaceholderText("搜索商品名称、描述...");
    searchEdit->setMinimumHeight(38);
    searchEdit->setObjectName("searchEdit");

    searchBtn = new QPushButton("搜索");
    searchBtn->setObjectName("primaryBtn");
    searchBtn->setFixedWidth(90);
    searchBtn->setMinimumHeight(38);

    sortCombo = new QComboBox();
    sortCombo->addItems({"最新发布", "价格最低", "价格最高", "最热商品"});
    sortCombo->setFixedWidth(130);
    sortCombo->setMinimumHeight(38);
    sortCombo->setObjectName("sortCombo");

    searchLayout->addWidget(searchEdit, 1);
    searchLayout->addWidget(searchBtn);
    searchLayout->addWidget(new QLabel("排序:"));
    searchLayout->addWidget(sortCombo);

    // 欢迎标签
    welcomeLabel = new QLabel("热门推荐商品");
    welcomeLabel->setObjectName("welcomeLabel");

    // 主体内容区
    QWidget *contentArea = new QWidget();
    contentArea->setObjectName("contentArea");
    QHBoxLayout *contentLayout = new QHBoxLayout(contentArea);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(20);

    // 左侧分类列表
    QWidget *categoryWidget = new QWidget();
    categoryWidget->setFixedWidth(200);
    categoryWidget->setObjectName("categoryWidget");
    QVBoxLayout *categoryLayout = new QVBoxLayout(categoryWidget);
    categoryLayout->setContentsMargins(10, 10, 0, 0);

    QLabel *categoryTitle = new QLabel("商品分类");
    categoryTitle->setObjectName("categoryTitle");
    categoryTitle->setContentsMargins(0, 0, 0, 10);

    categoryList = new QListWidget();
    categoryList->setObjectName("categoryList");
    categoryList->addItems({"📦 全部商品", "📚 书籍教材", "💻 电子产品", "👕 服饰鞋包",
                            "🏠 生活用品", "⚽ 体育器材", "✏️ 学习工具", "💄 美妆个护", "🔍 其他"});

    categoryLayout->addWidget(categoryTitle);
    categoryLayout->addWidget(categoryList, 1);

    // 右侧商品表格区域
    QWidget *goodsWidget = new QWidget();
    goodsWidget->setObjectName("goodsWidget");
    QVBoxLayout *goodsLayout = new QVBoxLayout(goodsWidget);
    goodsLayout->setContentsMargins(0, 0, 0, 0);

    goodsTable = new QTableWidget(0, 5);
    goodsTable->setObjectName("goodsTable");
    QStringList headers = {"商品图片", "名称", "价格", "发布时间", "状态"};
    goodsTable->setHorizontalHeaderLabels(headers);
    goodsTable->verticalHeader()->setVisible(false);
    goodsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    goodsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    goodsTable->setAlternatingRowColors(true);
    goodsTable->setShowGrid(false);

    // 设置列宽
    goodsTable->setColumnWidth(0, 90);
    goodsTable->setColumnWidth(1, 250);
    goodsTable->setColumnWidth(2, 110);
    goodsTable->setColumnWidth(3, 150);
    goodsTable->horizontalHeader()->setStretchLastSection(true);

    goodsLayout->addWidget(goodsTable, 1);

    // 添加到内容区
    contentLayout->addWidget(categoryWidget);
    contentLayout->addWidget(goodsWidget, 1);

    // 添加到主布局
    mainLayout->addWidget(searchBar);
    mainLayout->addWidget(welcomeLabel);
    mainLayout->addWidget(contentArea, 1);

    return page;
}

QWidget* MainWindow::createPublishPage() {
    QWidget *page = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    QLabel *titleLabel = new QLabel("发布新商品");
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #2c3e50;");

    // 表单容器
    QWidget *formContainer = new QWidget();
    QVBoxLayout *formLayout = new QVBoxLayout(formContainer);
    formLayout->setSpacing(15);

    // 商品名称
    QWidget *nameRow = new QWidget();
    QHBoxLayout *nameLayout = new QHBoxLayout(nameRow);
    nameLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *nameLabel = new QLabel("商品名称:");
    nameLabel->setFixedWidth(100);
    goodsNameEdit = new QLineEdit();
    goodsNameEdit->setPlaceholderText("请输入商品名称（最多50字）");

    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(goodsNameEdit, 1);

    // 商品分类
    QWidget *categoryRow = new QWidget();
    QHBoxLayout *categoryLayout = new QHBoxLayout(categoryRow);
    categoryLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *categoryLabel = new QLabel("商品分类:");
    categoryLabel->setFixedWidth(100);
    goodsCategoryCombo = new QComboBox();
    goodsCategoryCombo->addItems({"书籍教材", "电子产品", "服饰鞋包", "生活用品",
                                  "体育器材", "学习工具", "美妆个护", "其他"});

    categoryLayout->addWidget(categoryLabel);
    categoryLayout->addWidget(goodsCategoryCombo, 1);

    // 商品价格
    QWidget *priceRow = new QWidget();
    QHBoxLayout *priceLayout = new QHBoxLayout(priceRow);
    priceLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *priceLabel = new QLabel("期望价格:");
    priceLabel->setFixedWidth(100);
    goodsPriceEdit = new QLineEdit();
    goodsPriceEdit->setPlaceholderText("单位：元");

    QPushButton *aiPriceBtn = new QPushButton("AI估价");
    aiPriceBtn->setObjectName("secondaryBtn");

    priceLayout->addWidget(priceLabel);
    priceLayout->addWidget(goodsPriceEdit, 1);
    priceLayout->addWidget(aiPriceBtn);

    // 商品描述
    QWidget *descRow = new QWidget();
    QVBoxLayout *descLayout = new QVBoxLayout(descRow);
    descLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *descLabel = new QLabel("商品描述:");
    goodsDescEdit = new QTextEdit();
    goodsDescEdit->setPlaceholderText("请详细描述商品信息、使用状况、包含附件等");
    goodsDescEdit->setMaximumHeight(150);

    descLayout->addWidget(descLabel);
    descLayout->addWidget(goodsDescEdit);

    // 图片上传
    QWidget *imageRow = new QWidget();
    QVBoxLayout *imageLayout = new QVBoxLayout(imageRow);
    imageLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *imageLabel = new QLabel("商品图片:");
    uploadImageBtn = new QPushButton("+ 上传图片");
    uploadImageBtn->setObjectName("secondaryBtn");
    uploadImageBtn->setFixedSize(150, 150);

    imagePreview = new QLabel();
    imagePreview->setFixedSize(150, 150);
    imagePreview->setStyleSheet("border: 2px dashed #ccc; border-radius: 8px;");
    imagePreview->setAlignment(Qt::AlignCenter);
    imagePreview->setText("暂无图片");

    connect(uploadImageBtn, &QPushButton::clicked, [this]() {
        QString fileName = QFileDialog::getOpenFileName(this, "选择商品图片",
                                                        "", "Images (*.png *.jpg *.jpeg)");
        if (!fileName.isEmpty()) {
            QPixmap pixmap(fileName);
            imagePreview->setPixmap(pixmap.scaled(150, 150, Qt::KeepAspectRatio));
        }
    });

    QHBoxLayout *imageBtnLayout = new QHBoxLayout();
    imageBtnLayout->addWidget(uploadImageBtn);
    imageBtnLayout->addWidget(imagePreview);
    imageBtnLayout->addStretch();

    imageLayout->addWidget(imageLabel);
    imageLayout->addLayout(imageBtnLayout);

    // 提交按钮
    QPushButton *submitBtn = new QPushButton("发布商品");
    submitBtn->setObjectName("primaryBtn");
    submitBtn->setFixedHeight(45);
    connect(submitBtn, &QPushButton::clicked, this, &MainWindow::onPublishGoods);

    // 添加到表单
    formLayout->addWidget(nameRow);
    formLayout->addWidget(categoryRow);
    formLayout->addWidget(priceRow);
    formLayout->addWidget(descRow);
    formLayout->addWidget(imageRow);
    formLayout->addWidget(submitBtn);

    // 添加到主布局
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(formContainer);
    mainLayout->addStretch();

    return page;
}

QWidget* MainWindow::createUserCenterPage() {
    QWidget *page = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(20);

    // ========== 1. 顶部用户信息卡片 ==========
    QWidget *userCard = new QWidget();
    userCard->setFixedHeight(200); // 固定高度，保证布局稳定
    userCard->setStyleSheet(R"(
        background-color: #F0F8FB;
        border-radius: 16px;
        padding: 20px;
    )");
    QHBoxLayout *cardLayout = new QHBoxLayout(userCard);
    cardLayout->setContentsMargins(0, 0, 0, 0);
    cardLayout->setSpacing(30);

    // 1.1 头像区域
    userAvatarLabel = new QLabel();
    userAvatarLabel->setFixedSize(120, 120);
    userAvatarLabel->setStyleSheet(R"(
        border-radius: 60px; /* 圆形头像 */
        border: 1px solid white;
        padding: 10px;
    )");
    userAvatarLabel->setPixmap(QPixmap(":/icons/img/user.png").scaled(116,116, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    userAvatarLabel->setScaledContents(true);

    // 1.2 用户信息文本区
    QVBoxLayout *infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(0);
    infoLayout->setAlignment(Qt::AlignCenter);

    userNameLabel = new QLabel("张三同学");
    userNameLabel->setStyleSheet(R"(
        font-size: 20px;
        font-weight: 600;
        color: #2D3748;
    )");

    QWidget *subInfoWidget = new QWidget();
    QHBoxLayout *subInfoLayout = new QHBoxLayout(subInfoWidget);
    subInfoLayout->setSpacing(10);
    subInfoLayout->setContentsMargins(5, 6, 5, 6);

    QLabel *userLevelLabel = new QLabel("信用分: 96");
    userLevelLabel->setStyleSheet("font-size: 14px; color: #4A5568;");
    userLevelLabel->setMinimumHeight(20);

    QLabel *userJoinLabel = new QLabel("注册时间: 2024-03-01");
    userJoinLabel->setStyleSheet("font-size: 14px; color: #718096;");
    userLevelLabel->setMinimumHeight(20);

    subInfoLayout->addWidget(userLevelLabel);
    subInfoLayout->addWidget(userJoinLabel);

    QHBoxLayout *profileButtonLayout = new QHBoxLayout();
    QPushButton *editProfileBtn = new QPushButton("编辑资料");
    QPushButton *creditScoreBtn = new QPushButton("信用分详情");
    editProfileBtn->setFixedSize(120, 40);
    editProfileBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #4299E1;
            color: white;
            border-radius: 8px;
            font-weight: 600;
            font-size: 14px;
            padding: 10px 16px;
            border: none;
        }
        QPushButton:hover {
            background-color: #3182CE;
        }
        QPushButton:pressed {
            background-color: #2B6CB0;
        }
    )");
    creditScoreBtn->setFixedSize(120, 40);
    creditScoreBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #9B59B6;
            color: white;
            border-radius: 8px;
            font-weight: 600;
            font-size: 14px;
            padding: 10px 16px;
            border: none;
        }
        QPushButton:hover {
            background-color: #8E44AD;
        }
        QPushButton:pressed {
            background-color: #7D3C98;
        }
    )");
    connect(editProfileBtn, &QPushButton::clicked, this, &MainWindow::onShowProfileEdit);
    connect(creditScoreBtn, &QPushButton::clicked, this, &MainWindow::onShowCreditScore);

    profileButtonLayout->addWidget(editProfileBtn);
    profileButtonLayout->addStretch(1);
    profileButtonLayout->addWidget(creditScoreBtn);
    profileButtonLayout->addStretch(10);

    infoLayout->addWidget(userNameLabel);
    infoLayout->addWidget(subInfoWidget);
    infoLayout->addLayout(profileButtonLayout);

    // 1.3 卡片右侧留白+阴影
    cardLayout->addWidget(userAvatarLabel);
    cardLayout->addLayout(infoLayout);
    cardLayout->addStretch(); // 右侧留白，信息居中

    QGraphicsDropShadowEffect *cardShadow = new QGraphicsDropShadowEffect(this);
    cardShadow->setBlurRadius(12);
    cardShadow->setXOffset(0);
    cardShadow->setYOffset(4);
    cardShadow->setColor(QColor(0, 0, 0, 15));
    userCard->setGraphicsEffect(cardShadow);

    // ========== 2. 功能标签区（左侧菜单+右侧内容） ==========
    QWidget *funcContentWidget = new QWidget();
    QHBoxLayout *funcContentLayout = new QHBoxLayout(funcContentWidget);
    funcContentLayout->setContentsMargins(0, 0, 0, 0);
    funcContentLayout->setSpacing(30);

    // 2.1 左侧功能菜单（垂直排列）
    QWidget *menuWidget = new QWidget();
    menuWidget->setFixedWidth(200);
    QVBoxLayout *menuLayout = new QVBoxLayout(menuWidget);
    menuLayout->setContentsMargins(0, 0, 0, 0);
    menuLayout->setSpacing(0);

    // 菜单选项
    QStringList menuTexts = {"我的发布", "我的收藏", "评价记录", "浏览历史"};
    QStringList menuIcons = {"📦", "❤️", "⭐", "👁️"};

    for (int i = 0; i < menuTexts.size(); ++i) {
        QPushButton *menuBtn = new QPushButton(menuIcons[i] + "  " + menuTexts[i]);
        menuBtn->setFixedHeight(50);
        menuBtn->setStyleSheet(R"(
            QPushButton {
                border: none;
                text-align: left;
                padding-left: 25px;
                font-size: 15px;
                color: #4A5568;
                background-color: transparent;
            }
            QPushButton:hover {
                background-color: #E8F4F8;
                color: #2B6CB0;
            }
            QPushButton:checked {
                background-color: #E8F4F8;
                color: #2B6CB0;
                border-left: 3px solid #4299E1;
                font-weight: 500;
            }
        )");
        menuBtn->setCheckable(true);
        if (i == 0) menuBtn->setChecked(true); // 默认选中第一个

        // 绑定菜单切换信号
        connect(menuBtn, &QPushButton::clicked, this, [this, i]() {
            userSubTabs->setCurrentIndex(i);
        });

        menuLayout->addWidget(menuBtn);
    }
    menuLayout->addStretch();

    // 2.2 右侧内容区
    userSubTabs = new QTabWidget();
    userSubTabs->setTabPosition(QTabWidget::West);
    userSubTabs->setStyleSheet(R"(
        QTabWidget::pane {
            border: 1px solid #F3F4F6;
            border-radius: 12px;
            background-color: white;
            padding: 20px;
        }
        QTabBar::tab {
            width: 0;
            height: 0;
            margin: 0;
            padding: 0;
            border: none;
        }
    )");

    // 2.2.1 我的发布
    QWidget *myPublishWidget = new QWidget();
    QVBoxLayout *publishLayout = new QVBoxLayout(myPublishWidget);
    publishLayout->setContentsMargins(0, 0, 0, 0);
    publishLayout->setSpacing(15);

    QLabel *publishTitle = new QLabel("我的发布商品");
    publishTitle->setStyleSheet(R"(
        font-size: 18px;
        font-weight: 600;
        color: #2D3748;
    )");

    QTableWidget *myGoodsTable = new QTableWidget(0, 4);
    myGoodsTable->setHorizontalHeaderLabels({"商品", "价格", "状态", "操作"});
    myGoodsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    myGoodsTable->setStyleSheet(R"(
        QTableWidget {
            border: 1px solid #F3F4F6;
            border-radius: 8px;
            gridline-color: #F7FAFC;
            alternate-background-color: #F9FAFB;
        }
        QTableWidget::item {
            padding: 12px;
            color: #4A5568;
        }
        QTableWidget::item:selected {
            background-color: #E8F4F8;
            color: #2B6CB0;
            border: none;
        }
        QHeaderView::section {
            background-color: #F7FAFC;
            border: none;
            border-bottom: 1px solid #E2E8F0;
            padding: 10px;
            color: #718096;
            font-size: 13px;
        }
    )");
    myGoodsTable->setColumnWidth(0,200);
    myGoodsTable->setColumnWidth(1,100);
    myGoodsTable->setColumnWidth(2,100);
    myGoodsTable->setColumnWidth(3,200);
    myGoodsTable->horizontalHeader()->setStretchLastSection(true);
    myGoodsTable->verticalHeader()->setVisible(false);

    // 模拟数据
    QStringList publishData = {"二手iPhone 12", "¥2500", "待售", "编辑/下架"};
    int row = myGoodsTable->rowCount();
    myGoodsTable->insertRow(row);
    for (int j = 0; j < publishData.size(); ++j) {
        myGoodsTable->setItem(row, j, new QTableWidgetItem(publishData[j]));
    }

    publishLayout->addWidget(publishTitle);
    publishLayout->addWidget(myGoodsTable);

    // 2.2.2 我的收藏
    QWidget *myCollectionWidget = new QWidget();
    QVBoxLayout *collectionLayout = new QVBoxLayout(myCollectionWidget);
    collectionLayout->setContentsMargins(6, 0, 0, 0);
    collectionLayout->setSpacing(15);

    QLabel *collectionTitle = new QLabel("我的收藏");
    collectionTitle->setStyleSheet(publishTitle->styleSheet());

    QListWidget *collectionList = new QListWidget();
    collectionList->setStyleSheet(R"(
        QListWidget {
            border: 1px solid #F3F4F6;
            border-radius: 8px;
            background-color: white;
        }
        QListWidget::item {
            padding: 15px 20px;
            border-bottom: 1px solid #F7FAFC;
            color: #4A5568;
            font-size: 14px;
        }
        QListWidget::item:hover {
            background-color: #F9FAFB;
        }
        QListWidget::item:selected {
            background-color: #E8F4F8;
            color: #2B6CB0;
            border: none;
        }
    )");
    collectionList->addItem("大学物理教材 - ¥35");
    collectionList->addItem("篮球鞋 Nike Air - ¥280");

    collectionLayout->addWidget(collectionTitle);
    collectionLayout->addWidget(collectionList);

    // 2.2.3 评价记录
    QWidget *reviewWidget = new QWidget();
    QVBoxLayout *reviewLayout = new QVBoxLayout(reviewWidget);
    reviewLayout->setContentsMargins(0, 0, 0, 0);
    reviewLayout->setSpacing(15);

    QLabel *reviewTitle = new QLabel("我的评价");
    reviewTitle->setStyleSheet(publishTitle->styleSheet());

    QTextEdit *reviewEdit = new QTextEdit();
    reviewEdit->setReadOnly(true);
    reviewEdit->setStyleSheet(R"(
        QTextEdit {
            border: 1px solid #F3F4F6;
            border-radius: 8px;
            padding: 15px;
            background-color: white;
            color: #4A5568;
            font-size: 14px;
        }
    )");
    reviewEdit->setText("评价记录：\n1. 商品质量很好，卖家很靠谱！（★★★★★）\n2. 物流很快，东西和描述一致～（★★★★☆）");

    reviewLayout->addWidget(reviewTitle);
    reviewLayout->addWidget(reviewEdit);

    // 2.2.4 浏览历史
    QWidget *historyWidget = new QWidget();
    QVBoxLayout *historyLayout = new QVBoxLayout(historyWidget);
    historyLayout->setAlignment(Qt::AlignCenter);

    QLabel *historyIcon = new QLabel("👁️");
    historyIcon->setStyleSheet("font-size: 48px; color: #CBD5E0;");

    QLabel *historyText = new QLabel("暂无浏览历史");
    historyText->setStyleSheet("font-size: 16px; color: #718096; margin-top: 15px;");

    historyLayout->addWidget(historyIcon);
    historyLayout->addWidget(historyText);

    // 给userSubTabs添加页面
    userSubTabs->addTab(myPublishWidget, "");
    userSubTabs->addTab(myCollectionWidget, "");
    userSubTabs->addTab(reviewWidget, "");
    userSubTabs->addTab(historyWidget, "");

    // 组装功能区
    funcContentLayout->addWidget(menuWidget);
    funcContentLayout->addWidget(userSubTabs, 1); // 内容区占满剩余空间

    // ========== 组装所有模块到主布局 ==========
    mainLayout->addWidget(userCard);
    mainLayout->addWidget(funcContentWidget, 1); // 功能区占满剩余高度

    // 页面背景色
    page->setStyleSheet("background-color: #FAFAFA;");

    return page;
}

QWidget* MainWindow::createMessagesPage() {
    QWidget *page = new QWidget();
    QHBoxLayout *mainLayout = new QHBoxLayout(page);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 左侧聊天列表
    QWidget *chatListWidget = new QWidget();
    chatListWidget->setFixedWidth(250);
    QVBoxLayout *listLayout = new QVBoxLayout(chatListWidget);

    QLabel *listTitle = new QLabel("聊天列表");
    listTitle->setStyleSheet("font-weight: bold; font-size: 16px; padding: 10px;");

    chatList = new QListWidget();

    listLayout->addWidget(listTitle);
    listLayout->addWidget(chatList);

    // 右侧聊天区域
    QWidget *chatAreaWidget = new QWidget();
    QVBoxLayout *chatLayout = new QVBoxLayout(chatAreaWidget);

    // 聊天头部
    QWidget *chatHeader = new QWidget();
    chatHeader->setFixedHeight(50);
    chatHeader->setStyleSheet("background-color: #f8f9fa; border-bottom: 1px solid #eee;");

    QHBoxLayout *headerLayout = new QHBoxLayout(chatHeader);
    QLabel *chatWithLabel = new QLabel("与 李四 的对话");
    chatWithLabel->setStyleSheet("font-weight: bold; font-size: 15px; ");

    headerLayout->addWidget(chatWithLabel);
    headerLayout->addStretch();

    // 聊天消息区域
    chatArea = new QTextEdit();
    chatArea->setReadOnly(true);

    // 消息输入区域
    QWidget *inputWidget = new QWidget();
    inputWidget->setFixedHeight(100);
    QVBoxLayout *inputLayout = new QVBoxLayout(inputWidget);

    messageEdit = new QLineEdit();
    messageEdit->setPlaceholderText("输入消息...");

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *sendBtn = new QPushButton("发送");
    QPushButton *fileBtn = new QPushButton("文件");

    sendBtn->setObjectName("primaryBtn");

    btnLayout->addStretch();
    btnLayout->addWidget(fileBtn);
    btnLayout->addWidget(sendBtn);

    inputLayout->addWidget(messageEdit);
    inputLayout->addLayout(btnLayout);

    chatLayout->addWidget(chatHeader);
    chatLayout->addWidget(chatArea, 1);
    chatLayout->addWidget(inputWidget);

    mainLayout->addWidget(chatListWidget);
    mainLayout->addWidget(chatAreaWidget, 1);

    return page;
}

QWidget* MainWindow::createOrdersPage() {
    QWidget *page = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // 1. 页面标题
    QLabel *titleLabel = new QLabel("我的订单");
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; margin-bottom: 20px;");
    mainLayout->addWidget(titleLabel);

    // 2. 订单筛选区域
    QWidget *filterWidget = new QWidget();
    QHBoxLayout *filterLayout = new QHBoxLayout(filterWidget);
    filterLayout->setContentsMargins(0, 0, 0, 0);
    filterLayout->setSpacing(10);

    QComboBox *statusCombo = new QComboBox();
    statusCombo->addItems({"全部订单", "待付款", "待发货", "待收货", "已完成", "已取消", "纠纷处理中"});
    statusCombo->setFixedWidth(120);

    QLineEdit *orderSearchEdit = new QLineEdit();
    orderSearchEdit->setPlaceholderText("搜索订单号、商品名称...");
    orderSearchEdit->setMinimumHeight(36);

    QPushButton *filterBtn = new QPushButton("筛选");
    filterBtn->setObjectName("primaryBtn");
    filterBtn->setFixedWidth(80);

    QPushButton *refreshBtn = new QPushButton("刷新");
    refreshBtn->setObjectName("secondaryBtn");
    refreshBtn->setFixedWidth(80);

    filterLayout->addWidget(new QLabel("订单状态:"));
    filterLayout->addWidget(statusCombo);
    filterLayout->addSpacing(20);
    filterLayout->addWidget(orderSearchEdit, 1);
    filterLayout->addWidget(filterBtn);
    filterLayout->addWidget(refreshBtn);

    mainLayout->addWidget(filterWidget);

    // 3. 订单表格
    ordersTable = new QTableWidget(0, 7);
    ordersTable->setHorizontalHeaderLabels({"订单号", "商品", "价格", "状态", "下单时间", "卖家", "操作"});

    // 设置列宽
    ordersTable->setColumnWidth(0, 100);  // 订单号
    ordersTable->setColumnWidth(1, 250);  // 商品
    ordersTable->setColumnWidth(2, 100);  // 价格
    ordersTable->setColumnWidth(3, 100);  // 状态
    ordersTable->setColumnWidth(4, 150);  // 下单时间
    ordersTable->setColumnWidth(5, 120);  // 卖家
    ordersTable->horizontalHeader()->setStretchLastSection(true);  // 操作列自适应

    ordersTable->verticalHeader()->setVisible(false);
    ordersTable->setAlternatingRowColors(true);
    ordersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ordersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 4. 添加示例订单数据
    loadOrderExamples();

    mainLayout->addWidget(ordersTable, 1);

    // 5. 底部统计信息
    QWidget *statsWidget = new QWidget();
    QHBoxLayout *statsLayout = new QHBoxLayout(statsWidget);
    statsLayout->setContentsMargins(0, 5, 0, 0);

    QLabel *totalLabel = new QLabel("共 3 个订单");
    totalLabel->setStyleSheet("color: #666;");

    statsLayout->addWidget(totalLabel);
    statsLayout->addStretch();

    QPushButton *exportBtn = new QPushButton("导出订单");
    exportBtn->setObjectName("primaryBtn");
    statsLayout->addWidget(exportBtn);

    mainLayout->addWidget(statsWidget);

    // 6. 连接信号槽
    connect(filterBtn, &QPushButton::clicked, [this, statusCombo, orderSearchEdit]() {
        QString status = statusCombo->currentText();
        QString keyword = orderSearchEdit->text().trimmed();
        filterOrders(status, keyword);
    });

    connect(refreshBtn, &QPushButton::clicked, [this]() {
        loadOrderExamples();  // 重新加载示例数据
    });

    connect(exportBtn, &QPushButton::clicked, [this]() {
        QMessageBox::information(this, "导出订单", "订单导出功能开发中...");
    });

    connect(statusCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this, orderSearchEdit, statusCombo](int index) {
                filterOrders(statusCombo->currentText(), orderSearchEdit->text().trimmed());
            });

    connect(orderSearchEdit, &QLineEdit::returnPressed, [this, statusCombo, orderSearchEdit]() {
        filterOrders(statusCombo->currentText(), orderSearchEdit->text().trimmed());
    });

    return page;
}

void MainWindow::loadMockData() {
    // 清空表格
    goodsTable->setRowCount(0);

    // 添加模拟数据
    QStringList goodsNames = {
        "二手iPhone 12 128GB",
        "大学物理教材",
        "篮球鞋 Nike Air",
        "笔记本电脑戴尔",
        "英语四级词汇书",
        "小米手环6",
        "吉他雅马哈",
        "考研数学复习全书"
    };

    QStringList categories = {"电子产品", "书籍教材", "服饰鞋包", "电子产品",
                              "书籍教材", "电子产品", "其他", "书籍教材"};

    QStringList prices = {"2500", "35", "280", "3200", "15", "150", "800", "40"};

    for (int i = 0; i < goodsNames.size(); i++) {
        int row = goodsTable->rowCount();
        goodsTable->insertRow(row);
        goodsTable->setRowHeight(row, 90);

        // 商品图片（模拟）
        QLabel *imageLabel = new QLabel();
        imageLabel->setPixmap(QPixmap(":/icons/img/buy.png").scaled(80, 80,Qt::KeepAspectRatio,Qt::SmoothTransformation));
        imageLabel->setAlignment(Qt::AlignCenter);
        goodsTable->setCellWidget(row, 0, imageLabel);

        // 商品名称
        QTableWidgetItem *nameItem = new QTableWidgetItem(goodsNames[i]);
        nameItem->setData(Qt::UserRole, i); // 存储商品ID
        goodsTable->setItem(row, 1, nameItem);

        // 价格
        goodsTable->setItem(row, 2, new QTableWidgetItem(QString("¥%1").arg(prices[i])));

        // 发布时间
        goodsTable->setItem(row, 3, new QTableWidgetItem(QDateTime::currentDateTime()
                                                             .addDays(-i).toString("yyyy-MM-dd hh:mm")));

        // 状态
        QString status = (i % 3 == 0) ? "待售" : (i % 3 == 1) ? "交易中" : "已售出";
        QTableWidgetItem *statusItem = new QTableWidgetItem(status);
        statusItem->setForeground(status == "待售" ? QColor(46, 204, 113) :
                                      status == "交易中" ? QColor(241, 196, 15) :
                                      QColor(149, 165, 166));
        goodsTable->setItem(row, 4, statusItem);
    }
}

void MainWindow::onCategoryClicked(QListWidgetItem* item) {
    QString category = item->text();
    welcomeLabel->setText(QString("当前分类: %1").arg(category));
}

void MainWindow::onSearchClicked() {
    QString keyword = searchEdit->text().trimmed();
    if (keyword.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入搜索关键词");
        return;
    }
    // 这里应该执行搜索逻辑
    QMessageBox::information(this, "提示", QString("搜索: %1").arg(keyword));
}

void MainWindow::onPublishGoods() {
    QString name = goodsNameEdit->text().trimmed();
    QString price = goodsPriceEdit->text().trimmed();
    QString desc = goodsDescEdit->toPlainText().trimmed();

    if (name.isEmpty() || price.isEmpty()) {
        QMessageBox::warning(this, "提示", "请填写商品名称和价格");
        return;
    }

    // 这里应该提交到服务器
    QMessageBox::information(this, "成功", "商品发布成功，等待审核");

    // 清空表单
    goodsNameEdit->clear();
    goodsPriceEdit->clear();
    goodsDescEdit->clear();
    imagePreview->clear();
    imagePreview->setText("暂无图片");
}

void MainWindow::onShowGoodsDetail(int row, int column) {
    if (row < 0 || column < 0) return;

    // 获取商品ID和名称
    QTableWidgetItem *nameItem = goodsTable->item(row, 1);
    QTableWidgetItem *priceItem = goodsTable->item(row, 2);

    if (!nameItem || !priceItem) return;

    int goodsId = nameItem->data(Qt::UserRole).toInt();
    QString goodsName = nameItem->text();
    QString goodsPrice = priceItem->text();

    // 创建商品详情对话框
    GoodsDetailDialog *detailDialog = new GoodsDetailDialog(this, goodsId);
    detailDialog->setWindowTitle(goodsName);
    detailDialog->setAttribute(Qt::WA_DeleteOnClose);

    // 连接信号
    // 联系卖家信号
    connect(detailDialog, &GoodsDetailDialog::contactSellerRequested,
            this, [this](int goodsId) {
                // 这里应该根据商品ID获取卖家ID，暂时用模拟数据
                QString sellerId = QString("seller_%1").arg(goodsId);
                ChatDialog *chatDialog = new ChatDialog(this, goodsId, sellerId);
                chatDialog->setWindowTitle(QString("与卖家聊天 - 商品ID: %1").arg(goodsId));
                chatDialog->setAttribute(Qt::WA_DeleteOnClose);
                chatDialog->show();
            });

    // 购买信号
    connect(detailDialog, &GoodsDetailDialog::buyNowRequested,
            this, [this, goodsName, goodsPrice](int goodsId) {
                QString message = QString("确认购买商品？\n\n"
                                          "商品: %1\n"
                                          "价格: %2\n"
                                          "商品ID: %3\n\n"
                                          "购买后请及时联系卖家完成交易。")
                                      .arg(goodsName)
                                      .arg(goodsPrice)
                                      .arg(goodsId);

                QMessageBox::StandardButton reply = QMessageBox::question(
                    this, "确认购买", message,
                    QMessageBox::Yes | QMessageBox::No);

                if (reply == QMessageBox::Yes) {
                    QMessageBox::information(this, "购买成功",
                                             "订单已创建！\n\n"
                                             "订单信息:\n"
                                             "• 商品: " + goodsName + "\n"
                                                               "• 价格: " + goodsPrice + "\n"
                                                                "• 状态: 待支付\n\n"
                                                                "请及时联系卖家完成交易。");

                    // 跳转到订单页面
                    mainTabWidget->setCurrentIndex(3); // 订单页索引
                }
            });

    // 议价信号
    connect(detailDialog, &GoodsDetailDialog::makeOfferRequested,
            this, [this, goodsPrice](int goodsId) {
                bool ok;
                // 从价格字符串中提取数字
                QString priceNum = goodsPrice;
                priceNum = priceNum.remove("¥").trimmed();

                QString offerPrice = QInputDialog::getText(this, "议价",
                                                           QString("当前价格: %1\n请输入您的出价:").arg(goodsPrice),
                                                           QLineEdit::Normal, priceNum, &ok);

                if (ok && !offerPrice.isEmpty()) {
                    // 验证价格是否为数字
                    bool isNumber;
                    double price = offerPrice.toDouble(&isNumber);

                    if (isNumber && price > 0) {
                        // 打开聊天窗口
                        ChatDialog *chatDialog = new ChatDialog(this, goodsId,
                                                                QString("seller_%1").arg(goodsId));
                        chatDialog->setAttribute(Qt::WA_DeleteOnClose);
                        chatDialog->show();

                        QMessageBox::information(this, "议价成功",
                                                 QString("已向卖家发送议价: ¥%1").arg(offerPrice));
                    } else {
                        QMessageBox::warning(this, "错误", "请输入有效的价格数字");
                    }
                }
            });
    connect(detailDialog, &GoodsDetailDialog::reportGoodsRequested,
            this, [this, goodsName](int goodsId) {
                onReportGoods(goodsId);
            });
    // 显示对话框
    detailDialog->show();
}

void MainWindow::onTabChanged(int index) {
    if (index == 0) { // 首页
        loadMockData();
    }
}

// 支付对话框显示
void MainWindow::onShowPayment() {
    // 从按钮属性获取订单信息
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (btn) {
        int orderId = btn->property("orderId").toInt();
        double amount = btn->property("amount").toDouble();

        PaymentDialog *dialog = new PaymentDialog(this, orderId, amount);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        dialog->show();
    } else {
        // 如果没有按钮触发，使用默认值
        PaymentDialog *dialog = new PaymentDialog(this, 1001, 2500.00);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        dialog->show();
    }
}

// 评价对话框显示
void MainWindow::onShowReview() {
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (btn) {
        int orderId = btn->property("orderId").toInt();
        QString sellerName = btn->property("sellerName").toString();

        ReviewDialog *dialog = new ReviewDialog(this, orderId, sellerName);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        connect(dialog, &ReviewDialog::reviewSubmitted, this, &MainWindow::onReviewSubmitted);
        dialog->show();
    } else {
        ReviewDialog *dialog = new ReviewDialog(this, 1002, "李四同学");
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        connect(dialog, &ReviewDialog::reviewSubmitted, this, &MainWindow::onReviewSubmitted);
        dialog->show();
    }
}

// 个人资料编辑对话框显示
void MainWindow::onShowProfileEdit() {
    ProfileEditDialog *dialog = new ProfileEditDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(dialog, &ProfileEditDialog::profileUpdated, this, &MainWindow::onProfileUpdated);
    dialog->show();
}

// 评价提交后的处理
void MainWindow::onReviewSubmitted(int orderId, int rating, QString comment) {
    qDebug() << "评价已提交 - 订单ID:" << orderId << "评分:" << rating << "评价内容:" << comment;

    // 在实际应用中，这里应该更新数据库中的订单状态
    QMessageBox::information(this, "评价成功",
                             QString("感谢您的评价！\n订单: %1\n评分: %2星").arg(orderId).arg(rating));

    // 可以在这里刷新订单列表，显示评价已完成
}

// 个人资料更新后的处理
void MainWindow::onProfileUpdated() {
    qDebug() << "个人资料已更新";

    // 在实际应用中，这里应该刷新界面上的用户信息
    QMessageBox::information(this, "保存成功", "个人资料已更新");

    // 可以在这里更新主窗口中的用户信息显示
    if (userNameLabel) {
        userNameLabel->setText("用户资料已更新");
    }
}

void MainWindow::onShowCreditScore() {
    // 如果已经存在对话框，先关闭
    if (creditScoreDialog) {
        creditScoreDialog->close();
        creditScoreDialog->deleteLater();
    }

    // 创建并显示信用分对话框
    // 这里使用当前用户的ID，假设从登录信息中获取
    QString currentUserId = "user_001";  // 实际应该从登录信息获取

    creditScoreDialog = new CreditScoreDialog(this, currentUserId);
    creditScoreDialog->setAttribute(Qt::WA_DeleteOnClose);

    // 连接对话框关闭信号
    connect(creditScoreDialog, &CreditScoreDialog::finished, [this]() {
        creditScoreDialog = nullptr;
    });

    creditScoreDialog->show();
    creditScoreDialog->raise();
    creditScoreDialog->activateWindow();
}

void MainWindow::onReportGoods(int goodsId) {
    if (reportDialog) {
        reportDialog->close();
        reportDialog->deleteLater();
    }

    // 获取商品名称 - 实际应从数据库获取
    QString goodsName = "未知商品";
    for (int i = 0; i < goodsTable->rowCount(); i++) {
        QTableWidgetItem *item = goodsTable->item(i, 1);
        if (item && item->data(Qt::UserRole).toInt() == goodsId) {
            goodsName = item->text();
            break;
        }
    }

    reportDialog = new ReportSubmitDialog(this, goodsId, "goods", goodsName);
    reportDialog->setAttribute(Qt::WA_DeleteOnClose);
    reportDialog->setWindowTitle(QString("举报商品 - %1").arg(goodsName));

    connect(reportDialog, &ReportSubmitDialog::reportSubmitted,
            this, &MainWindow::onReportSubmitted);
    connect(reportDialog, &ReportSubmitDialog::finished, [this]() {
        reportDialog = nullptr;
    });

    reportDialog->show();
    reportDialog->raise();
    reportDialog->activateWindow();
}

void MainWindow::onReportUser(QString userId) {
    if (reportDialog) {
        reportDialog->close();
        reportDialog->deleteLater();
    }

    // 获取用户名 - 实际应从数据库获取
    QString userName = "未知用户";
    // TODO: 从数据库查询用户名

    reportDialog = new ReportSubmitDialog(this, userId.toInt(), "user", userName);
    reportDialog->setAttribute(Qt::WA_DeleteOnClose);
    reportDialog->setWindowTitle(QString("举报用户 - %1").arg(userName));

    connect(reportDialog, &ReportSubmitDialog::reportSubmitted,
            this, &MainWindow::onReportSubmitted);
    connect(reportDialog, &ReportSubmitDialog::finished, [this]() {
        reportDialog = nullptr;
    });

    reportDialog->show();
    reportDialog->raise();
    reportDialog->activateWindow();
}

void MainWindow::onReportOrder(int orderId) {
    if (reportDialog) {
        reportDialog->close();
        reportDialog->deleteLater();
    }

    // 获取订单信息 - 实际应从数据库获取
    QString orderInfo = QString("订单 %1").arg(orderId);

    reportDialog = new ReportSubmitDialog(this, orderId, "order", orderInfo);
    reportDialog->setAttribute(Qt::WA_DeleteOnClose);
    reportDialog->setWindowTitle(QString("举报订单 - #%1").arg(orderId));

    connect(reportDialog, &ReportSubmitDialog::reportSubmitted,
            this, &MainWindow::onReportSubmitted);
    connect(reportDialog, &ReportSubmitDialog::finished, [this]() {
        reportDialog = nullptr;
    });

    reportDialog->show();
    reportDialog->raise();
    reportDialog->activateWindow();
}

void MainWindow::onReportSubmitted(int targetId, QString targetType) {
    qDebug() << "举报已提交 - 目标ID:" << targetId << "类型:" << targetType;

    // 在实际项目中，这里可以：
    // 1. 更新数据库中的举报记录
    // 2. 发送通知给管理员
    // 3. 记录举报日志

    // 显示成功消息
    QString typeName;
    if (targetType == "goods") typeName = "商品";
    else if (targetType == "user") typeName = "用户";
    else if (targetType == "order") typeName = "订单";

    QMessageBox::information(this, "举报成功",
                             QString("您的举报已提交成功！\n\n"
                                     "举报对象：%1 #%2\n"
                                     "管理员将在24小时内处理。\n"
                                     "处理结果将通过系统消息通知您。")
                                 .arg(typeName).arg(targetId));
}

void MainWindow::loadOrderExamples() {
    if (!ordersTable) return;

    ordersTable->setRowCount(0);

    // 订单数据：订单号, 商品名称, 价格, 状态, 下单时间, 卖家, 操作
    QList<QStringList> orderData = {
        {"1001", "二手iPhone 12 128GB", "¥2500", "待付款", "2024-03-20 10:30", "张三同学", ""},
        {"1002", "大学物理教材", "¥35", "已完成", "2024-03-18 14:20", "李四同学", ""},
        {"1003", "篮球鞋 Nike Air", "¥280", "待收货", "2024-03-19 16:45", "王五同学", ""},
        {"1004", "笔记本电脑戴尔", "¥3200", "已完成", "2024-03-15 09:15", "赵六同学", ""},
        {"1005", "小米手环6", "¥150", "已取消", "2024-03-12 11:20", "钱七同学", ""},
        {"1006", "吉他雅马哈", "¥800", "纠纷处理中", "2024-03-10 08:45", "孙八同学", ""}
    };

    for (int i = 0; i < orderData.size(); i++) {
        int row = ordersTable->rowCount();
        ordersTable->insertRow(row);

        // 填充基础数据
        for (int col = 0; col < orderData[i].size() - 1; col++) { // 最后一个列是操作，单独处理
            QTableWidgetItem *item = new QTableWidgetItem(orderData[i][col]);

            // 设置状态列的颜色
            if (col == 3) { // 状态列
                QString status = orderData[i][col];
                if (status == "待付款") {
                    item->setForeground(QColor(231, 76, 60));  // 红色
                    item->setBackground(QColor(255, 243, 205));  // 浅黄色背景
                } else if (status == "已完成") {
                    item->setForeground(QColor(46, 204, 113));  // 绿色
                } else if (status == "待收货") {
                    item->setForeground(QColor(241, 196, 15));  // 黄色
                } else if (status == "已取消") {
                    item->setForeground(QColor(149, 165, 166)); // 灰色
                } else if (status == "纠纷处理中") {
                    item->setForeground(QColor(230, 126, 34));  // 橙色
                    item->setBackground(QColor(253, 237, 236)); // 浅红色背景
                }
                item->setFont(QFont("Microsoft YaHei", -1, QFont::Bold)); // 加粗
            }

            // 价格列右对齐
            if (col == 2) {
                item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
                item->setFont(QFont("Microsoft YaHei", -1, QFont::Bold));
            }

            ordersTable->setItem(row, col, item);
        }

        // 第6列：操作按钮
        QString status = orderData[i][3]; // 获取状态
        int orderId = orderData[i][0].toInt();

        QWidget *actionWidget = new QWidget();
        QHBoxLayout *actionLayout = new QHBoxLayout(actionWidget);
        actionLayout->setContentsMargins(5, 2, 5, 2);
        actionLayout->setSpacing(5);

        // 根据状态显示不同的操作按钮
        if (status == "待付款") {
            QPushButton *payBtn = new QPushButton("去支付");
            payBtn->setProperty("orderId", orderId);
            payBtn->setProperty("amount", 2500.00);
            payBtn->setObjectName("primaryBtn");
            payBtn->setFixedSize(70, 25);
            connect(payBtn, &QPushButton::clicked, this, &MainWindow::onShowPayment);

            QPushButton *cancelBtn = new QPushButton("取消订单");
            cancelBtn->setObjectName("secondaryBtn");
            cancelBtn->setFixedSize(80, 25);
            connect(cancelBtn, &QPushButton::clicked, [this, orderId]() {
                onCancelOrder(orderId);
            });

            actionLayout->addWidget(payBtn);
            actionLayout->addWidget(cancelBtn);

        } else if (status == "待收货") {
            QPushButton *confirmBtn = new QPushButton("确认收货");
            confirmBtn->setObjectName("primaryBtn");
            confirmBtn->setFixedSize(80, 25);
            connect(confirmBtn, &QPushButton::clicked, [this, orderId]() {
                onConfirmReceipt(orderId);
            });

            // 售后纠纷按钮（待收货状态也可以申请）
            QPushButton *disputeBtn = new QPushButton("售后");
            disputeBtn->setObjectName("warningBtn");
            disputeBtn->setFixedSize(60, 25);
            disputeBtn->setProperty("orderId", orderId);
            connect(disputeBtn, &QPushButton::clicked, [this, orderId]() {
                onShowDisputeSubmit(orderId);
            });

            actionLayout->addWidget(confirmBtn);
            actionLayout->addWidget(disputeBtn);

        } else if (status == "已完成") {
            QPushButton *reviewBtn = new QPushButton("评价");
            reviewBtn->setProperty("orderId", orderId);
            reviewBtn->setProperty("sellerName", orderData[i][5]); // 卖家名称
            reviewBtn->setObjectName("secondaryBtn");
            reviewBtn->setFixedSize(60, 25);
            connect(reviewBtn, &QPushButton::clicked, this, &MainWindow::onShowReview);

            // 售后纠纷按钮（已完成状态可以申请）
            QPushButton *disputeBtn = new QPushButton("售后");
            disputeBtn->setObjectName("warningBtn");
            disputeBtn->setFixedSize(60, 25);
            disputeBtn->setProperty("orderId", orderId);
            connect(disputeBtn, &QPushButton::clicked, [this, orderId]() {
                onShowDisputeSubmit(orderId);
            });

            actionLayout->addWidget(reviewBtn);
            actionLayout->addWidget(disputeBtn);

        } else if (status == "纠纷处理中") {
            // 纠纷处理中的订单显示查看纠纷详情
            QPushButton *viewDisputeBtn = new QPushButton("查看纠纷");
            viewDisputeBtn->setObjectName("warningBtn");
            viewDisputeBtn->setFixedSize(80, 25);
            connect(viewDisputeBtn, &QPushButton::clicked, [this, orderId]() {
                onViewDisputeDetail(orderId);
            });

            actionLayout->addWidget(viewDisputeBtn);

        } else if (status == "已取消") {
            // 已取消订单显示重新购买或删除
            QPushButton *reorderBtn = new QPushButton("重新购买");
            reorderBtn->setObjectName("secondaryBtn");
            reorderBtn->setFixedSize(80, 25);

            QPushButton *deleteBtn = new QPushButton("删除");
            deleteBtn->setObjectName("secondaryBtn");
            deleteBtn->setFixedSize(60, 25);

            actionLayout->addWidget(reorderBtn);
            actionLayout->addWidget(deleteBtn);
        }

        actionLayout->addStretch();
        ordersTable->setCellWidget(row, 6, actionWidget);
    }

    // 设置表格行高
    for (int i = 0; i < ordersTable->rowCount(); i++) {
        ordersTable->setRowHeight(i, 50);
    }
}

// 订单筛选函数
void MainWindow::filterOrders(const QString &status, const QString &keyword) {
    if (!ordersTable) return;

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
    }
}

// 取消订单函数
void MainWindow::onCancelOrder(int orderId) {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "确认取消",
        QString("确定要取消订单 #%1 吗？").arg(orderId),
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        // 这里应该更新数据库中的订单状态
        QMessageBox::information(this, "取消成功", "订单已取消");
        loadOrderExamples(); // 重新加载订单列表
    }
}

// 确认收货函数
void MainWindow::onConfirmReceipt(int orderId) {
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

                    // 更新操作按钮
                    QWidget *oldWidget = ordersTable->cellWidget(row, 6);
                    if (oldWidget) oldWidget->deleteLater();

                    // 重新创建操作按钮
                    QWidget *actionWidget = new QWidget();
                    QHBoxLayout *actionLayout = new QHBoxLayout(actionWidget);
                    actionLayout->setContentsMargins(5, 2, 5, 2);
                    actionLayout->setSpacing(5);

                    QPushButton *reviewBtn = new QPushButton("评价");
                    reviewBtn->setProperty("orderId", orderId);
                    reviewBtn->setObjectName("secondaryBtn");
                    reviewBtn->setFixedSize(60, 25);
                    connect(reviewBtn, &QPushButton::clicked, this, &MainWindow::onShowReview);

                    QPushButton *disputeBtn = new QPushButton("售后");
                    disputeBtn->setObjectName("warningBtn");
                    disputeBtn->setFixedSize(60, 25);
                    disputeBtn->setProperty("orderId", orderId);
                    connect(disputeBtn, &QPushButton::clicked, [this, orderId]() {
                        onShowDisputeSubmit(orderId);
                    });

                    actionLayout->addWidget(reviewBtn);
                    actionLayout->addWidget(disputeBtn);
                    actionLayout->addStretch();

                    ordersTable->setCellWidget(row, 6, actionWidget);
                }
                break;
            }
        }

        QMessageBox::information(this, "确认成功", "订单状态已更新为【已完成】");
    }
}

// 查看纠纷详情函数
void MainWindow::onViewDisputeDetail(int orderId) {
    QMessageBox::information(this, "纠纷详情",
                             QString("订单 #%1 纠纷处理中\n\n"
                                     "纠纷类型：商品质量问题\n"
                                     "提交时间：2024-03-20 14:30\n"
                                     "当前状态：管理员审核中\n"
                                     "预计处理时间：1-3个工作日").arg(orderId));
}

void MainWindow::onShowDisputeSubmit(int orderId) {
    qDebug() << "onShowDisputeSubmit called with orderId:" << orderId; // 添加调试输出

    if (disputeDialog) {
        qDebug() << "Closing existing dispute dialog";
        disputeDialog->close();
        disputeDialog->deleteLater();
    }

    // 检查订单是否已经有纠纷
    bool hasExistingDispute = false;
    for (int row = 0; row < ordersTable->rowCount(); row++) {
        QTableWidgetItem *orderIdItem = ordersTable->item(row, 0);
        if (orderIdItem && orderIdItem->text().toInt() == orderId) {
            QTableWidgetItem *statusItem = ordersTable->item(row, 3);
            if (statusItem && statusItem->text() == "纠纷处理中") {
                hasExistingDispute = true;
                break;
            }
        }
    }

    if (hasExistingDispute) {
        QMessageBox::information(this, "提示", "该订单已有正在处理的纠纷，请等待处理结果");
        return;
    }

    disputeDialog = new DisputeSubmitDialog(this, orderId);
    disputeDialog->setAttribute(Qt::WA_DeleteOnClose);
    disputeDialog->setWindowTitle(QString("提交售后纠纷 - 订单#%1").arg(orderId));

    // 连接提交成功的信号
    connect(disputeDialog, &DisputeSubmitDialog::finished, [this, orderId](int result) {
        if (result == QDialog::Accepted) {
            // 提交成功，更新订单状态
            onDisputeSubmitted(orderId);
        }
        disputeDialog = nullptr;
    });

    disputeDialog->show();
    disputeDialog->raise();
    disputeDialog->activateWindow();
}

void MainWindow::onDisputeSubmitted(int orderId) {
    // 更新订单状态为"纠纷处理中"
    for (int row = 0; row < ordersTable->rowCount(); row++) {
        QTableWidgetItem *orderIdItem = ordersTable->item(row, 0);
        if (orderIdItem && orderIdItem->text().toInt() == orderId) {
            QTableWidgetItem *statusItem = ordersTable->item(row, 3);
            if (statusItem) {
                statusItem->setText("纠纷处理中");
                statusItem->setForeground(QColor(230, 126, 34));  // 橙色
                statusItem->setBackground(QColor(253, 237, 236)); // 浅红色背景

                // 更新操作按钮为"查看纠纷"
                QWidget *oldWidget = ordersTable->cellWidget(row, 6);
                if (oldWidget) oldWidget->deleteLater();

                QWidget *actionWidget = new QWidget();
                QHBoxLayout *actionLayout = new QHBoxLayout(actionWidget);
                actionLayout->setContentsMargins(5, 2, 5, 2);
                actionLayout->setSpacing(5);

                QPushButton *viewDisputeBtn = new QPushButton("查看纠纷");
                viewDisputeBtn->setObjectName("warningBtn");
                viewDisputeBtn->setFixedSize(80, 25);
                connect(viewDisputeBtn, &QPushButton::clicked, [this, orderId]() {
                    onViewDisputeDetail(orderId);
                });

                actionLayout->addWidget(viewDisputeBtn);
                actionLayout->addStretch();

                ordersTable->setCellWidget(row, 6, actionWidget);
            }
            break;
        }
    }

    // 显示成功消息
    QMessageBox::information(this, "提交成功",
                             QString("售后纠纷申请已提交！\n\n"
                                     "订单号: #%1\n"
                                     "管理员将在24小时内处理。\n"
                                     "处理结果将通过系统消息通知您。").arg(orderId));
}

MainWindow::~MainWindow() {}
