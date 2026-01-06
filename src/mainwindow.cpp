#include "MainWindow.h"
#include "LoginPage.h"
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
#include "GoodsDetailDialog.h"
#include "ChatDialog.h"
#include "DisputeSubmitDialog.h"
#include "paymentdialog.h"
#include "reviewdialog.h"
#include "profileeditdialog.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("校园二手商品智能交易系统");
    setMinimumSize(1200, 800);

    setupUI();
    loadMockData();

    // 连接信号槽
    connect(categoryList, &QListWidget::itemClicked, this, &MainWindow::onCategoryClicked);
    connect(searchBtn, &QPushButton::clicked, this, &MainWindow::onSearchClicked);
    connect(goodsTable, &QTableWidget::cellDoubleClicked, this, &MainWindow::onShowGoodsDetail);
    connect(mainTabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
}

void MainWindow::setupUI() {
    // 1. 创建主窗口部件
    mainWidget = new QWidget(this);
    setCentralWidget(mainWidget);

    // 4. 创建主标签页
    mainTabWidget = new QTabWidget(mainWidget);
    mainTabWidget->setTabPosition(QTabWidget::North);
    mainTabWidget->setTabShape(QTabWidget::Rounded);

    // 创建各个页面 - 直接调用函数
    homePage = createHomePage();
    publishPage = createPublishPage();
    messagesPage = createMessagesPage();
    ordersPage = createOrdersPage();
    userCenterPage = createUserCenterPage();

    mainTabWidget->addTab(homePage, QIcon(":/icons/home.png"), "首页");
    mainTabWidget->addTab(publishPage, QIcon(":/icons/publish.png"), "发布商品");
    mainTabWidget->addTab(messagesPage, QIcon(":/icons/message.png"), "消息");
    mainTabWidget->addTab(ordersPage, QIcon(":/icons/order.png"), "我的订单");
    mainTabWidget->addTab(userCenterPage, QIcon(":/icons/user.png"), "个人中心");

    // 5. 设置主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->addWidget(mainTabWidget);

    // 6. 设置状态栏 - 修正构造函数调用
    QStatusBar *statusBar = this->statusBar();
    QLabel *statusLabel = new QLabel("欢迎使用校园二手交易系统");
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
            padding: 10px 20px;
            background-color: #ecf0f1;
            border-radius: 6px 6px 0 0;
            margin-right: 3px;
            font-size: 14px;
        }
        QTabBar::tab:selected {
            background-color: white;
            border-bottom: 3px solid #3498db;
            font-weight: bold;
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
        border: 1px solid #e2e8f0;
        box-shadow: 0 1px 3px rgba(0,0,0,0.05);
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
        box-shadow: 0 0 0 3px rgba(59, 130, 246, 0.1);
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
        -webkit-background-clip: text;
        -webkit-text-fill-color: transparent;
        text-shadow: 0 1px 2px rgba(0,0,0,0.05);
    }

    /* 分类区域样式 */
    #categoryWidget {
        background-color: white;
        border-radius: 12px;
        border: 1px solid #e2e8f0;
        padding: 15px;
        box-shadow: 0 1px 3px rgba(0,0,0,0.05);
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
        box-shadow: 0 1px 3px rgba(0,0,0,0.05);
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
    )");
}

QWidget* MainWindow::createHomePage() {
    QWidget *page = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // 顶部搜索栏 - 样式优化
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

    // 欢迎标签 - 样式优化
    welcomeLabel = new QLabel("热门推荐商品");
    welcomeLabel->setObjectName("welcomeLabel");

    // 主体内容区
    QWidget *contentArea = new QWidget();
    contentArea->setObjectName("contentArea");
    QHBoxLayout *contentLayout = new QHBoxLayout(contentArea);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(20);

    // 左侧分类列表 - 样式优化
    QWidget *categoryWidget = new QWidget();
    categoryWidget->setFixedWidth(200);
    categoryWidget->setObjectName("categoryWidget");
    QVBoxLayout *categoryLayout = new QVBoxLayout(categoryWidget);
    categoryLayout->setContentsMargins(0, 0, 0, 0);

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
    goodsPriceEdit->setPlaceholderText("元");

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
    mainLayout->setContentsMargins(30, 30, 30, 30); // 全局边距，避免贴边
    mainLayout->setSpacing(30); // 模块间间距，提升呼吸感

    // ========== 1. 顶部用户信息卡片（核心视觉区） ==========
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

    // 1.1 头像区域（圆形美化）
    userAvatarLabel = new QLabel();
    userAvatarLabel->setFixedSize(120, 120);
    userAvatarLabel->setStyleSheet(R"(
        border-radius: 60px; /* 圆形头像 */
        border: 4px solid white;
        background-color: #4299E1;
    )");
    userAvatarLabel->setPixmap(QPixmap(":/icons/default_avatar.png").scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    userAvatarLabel->setAlignment(Qt::AlignCenter);

    // 1.2 用户信息文本区（层次化排版）
    QVBoxLayout *infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(12);
    infoLayout->setAlignment(Qt::AlignCenter);

    userNameLabel = new QLabel("张三同学");
    userNameLabel->setStyleSheet(R"(
        font-size: 24px;
        font-weight: 600;
        color: #2D3748;
    )");

    QWidget *subInfoWidget = new QWidget();
    QHBoxLayout *subInfoLayout = new QHBoxLayout(subInfoWidget);
    subInfoLayout->setSpacing(20);
    subInfoLayout->setContentsMargins(0, 6, 0, 6);

    QLabel *userLevelLabel = new QLabel("信用等级: ★★★★☆");
    userLevelLabel->setStyleSheet("font-size: 14px; color: #4A5568;");
    userLevelLabel->setMinimumHeight(24);

    QLabel *userJoinLabel = new QLabel("注册时间: 2024-03-01");
    userJoinLabel->setStyleSheet("font-size: 14px; color: #718096;");
    userLevelLabel->setMinimumHeight(24);

    subInfoLayout->addWidget(userLevelLabel);
    subInfoLayout->addWidget(userJoinLabel);

    QPushButton *editProfileBtn = new QPushButton("编辑资料");
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
        box-shadow: 0 2px 4px rgba(66, 153, 225, 0.3);
    }
    QPushButton:pressed {
        background-color: #2B6CB0;
    }
)");
    connect(editProfileBtn, &QPushButton::clicked, this, &MainWindow::onShowProfileEdit);

    infoLayout->addWidget(userNameLabel);
    infoLayout->addWidget(subInfoWidget);
    infoLayout->addWidget(editProfileBtn);

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

    // ========== 2. 数据统计卡片（核心数据可视化） ==========
    QWidget *statsWidget = new QWidget();
    QHBoxLayout *statsLayout = new QHBoxLayout(statsWidget);
    statsLayout->setContentsMargins(0, 0, 0, 0);
    statsLayout->setSpacing(25);

    // 统计项：已发布/已购买/收藏/待收货
    struct StatsItem {
        QString text;
        QString num;
        QString color;
    };
    QList<StatsItem> statsItems = {
        {"已发布", "3", "#48BB78"},
        {"已购买", "8", "#4299E1"},
        {"我的收藏", "5", "#F6AD55"},
        {"待收货", "2", "#ED64A6"}
    };

    for (const auto &item : statsItems) {
        QWidget *statCard = new QWidget();
        statCard->setFixedSize(150, 100);
        statCard->setStyleSheet(R"(
            background-color: white;
            border-radius: 12px;
            border: 1px solid #F3F4F6;
        )");
        QVBoxLayout *statLayout = new QVBoxLayout(statCard);
        statLayout->setContentsMargins(15, 15, 15, 15);
        statLayout->setSpacing(8);
        statLayout->setAlignment(Qt::AlignCenter);

        QLabel *numLabel = new QLabel(item.num);
        numLabel->setStyleSheet(QString(R"(
            font-size: 28px;
            font-weight: 700;
            color: %1;
        )").arg(item.color));

        QLabel *textLabel = new QLabel(item.text);
        textLabel->setStyleSheet(R"(
            font-size: 14px;
            color: #718096;
        )");

        statLayout->addWidget(numLabel);
        statLayout->addWidget(textLabel);

        // hover效果
        statCard->setStyleSheet(statCard->styleSheet() + R"(
            QWidget:hover {
                background-color: #F9FAFB;
                border-color: #E2E8F0;
            }
        )");

        statsLayout->addWidget(statCard);
    }
    statsLayout->addStretch(); // 自适应窗口宽度

    // ========== 3. 功能标签区（左侧菜单+右侧内容） ==========
    QWidget *funcContentWidget = new QWidget();
    QHBoxLayout *funcContentLayout = new QHBoxLayout(funcContentWidget);
    funcContentLayout->setContentsMargins(0, 0, 0, 0);
    funcContentLayout->setSpacing(30);

    // 3.1 左侧功能菜单（垂直排列）
    QWidget *menuWidget = new QWidget();
    menuWidget->setFixedWidth(200);
    QVBoxLayout *menuLayout = new QVBoxLayout(menuWidget);
    menuLayout->setContentsMargins(0, 0, 0, 0);
    menuLayout->setSpacing(0);

    // 菜单选项（与原userSubTabs对应）
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

        // 绑定菜单切换信号（与原userSubTabs切换同步）
        connect(menuBtn, &QPushButton::clicked, this, [this, i]() {
            userSubTabs->setCurrentIndex(i);
        });

        menuLayout->addWidget(menuBtn);
    }
    menuLayout->addStretch();

    // 3.2 右侧内容区（复用原userSubTabs，美化样式）
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
    )"); // 隐藏原TabBar，用左侧菜单控制

    // 3.2.1 我的发布（复用原QTableWidget，美化）
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
    myGoodsTable->horizontalHeader()->setStretchLastSection(true);
    myGoodsTable->verticalHeader()->setVisible(false);

    // 模拟数据（与原逻辑一致）
    QStringList publishData = {"二手iPhone 12", "¥2500", "待售", "编辑/下架"};
    int row = myGoodsTable->rowCount();
    myGoodsTable->insertRow(row);
    for (int j = 0; j < publishData.size(); ++j) {
        myGoodsTable->setItem(row, j, new QTableWidgetItem(publishData[j]));
    }

    publishLayout->addWidget(publishTitle);
    publishLayout->addWidget(myGoodsTable);

    // 3.2.2 我的收藏（复用原QListWidget，美化）
    QWidget *myCollectionWidget = new QWidget();
    QVBoxLayout *collectionLayout = new QVBoxLayout(myCollectionWidget);
    collectionLayout->setContentsMargins(0, 0, 0, 0);
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

    // 3.2.3 评价记录（复用原QTextEdit，美化）
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

    // 3.2.4 浏览历史（新增默认页面）
    QWidget *historyWidget = new QWidget();
    QVBoxLayout *historyLayout = new QVBoxLayout(historyWidget);
    historyLayout->setAlignment(Qt::AlignCenter);

    QLabel *historyIcon = new QLabel("👁️");
    historyIcon->setStyleSheet("font-size: 48px; color: #CBD5E0;");

    QLabel *historyText = new QLabel("暂无浏览历史");
    historyText->setStyleSheet("font-size: 16px; color: #718096; margin-top: 15px;");

    historyLayout->addWidget(historyIcon);
    historyLayout->addWidget(historyText);

    // 给userSubTabs添加页面（与原逻辑一致）
    userSubTabs->addTab(myPublishWidget, "");
    userSubTabs->addTab(myCollectionWidget, "");
    userSubTabs->addTab(reviewWidget, "");
    userSubTabs->addTab(historyWidget, "");

    // 组装功能区
    funcContentLayout->addWidget(menuWidget);
    funcContentLayout->addWidget(userSubTabs, 1); // 内容区占满剩余空间

    // ========== 组装所有模块到主布局 ==========
    mainLayout->addWidget(userCard);
    mainLayout->addWidget(statsWidget);
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
    chatWithLabel->setStyleSheet("font-weight: bold;");

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
    QPushButton *emojiBtn = new QPushButton("表情");
    QPushButton *fileBtn = new QPushButton("文件");

    sendBtn->setObjectName("primaryBtn");

    btnLayout->addStretch();
    btnLayout->addWidget(emojiBtn);
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

    QLabel *titleLabel = new QLabel("我的订单");
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; margin-bottom: 20px;");

    // 订单筛选
    QWidget *filterWidget = new QWidget();
    QHBoxLayout *filterLayout = new QHBoxLayout(filterWidget);
    filterLayout->setContentsMargins(0, 0, 0, 0);

    QComboBox *statusCombo = new QComboBox();
    statusCombo->addItems({"全部订单", "待付款", "待发货", "待收货", "已完成", "已取消"});

    QLineEdit *orderSearchEdit = new QLineEdit();
    orderSearchEdit->setPlaceholderText("搜索订单号、商品名称...");

    QPushButton *filterBtn = new QPushButton("筛选");

    filterLayout->addWidget(new QLabel("订单状态:"));
    filterLayout->addWidget(statusCombo);
    filterLayout->addWidget(orderSearchEdit, 1);
    filterLayout->addWidget(filterBtn);

    // 订单表格 - 增加列数以容纳操作按钮
    ordersTable = new QTableWidget(3, 7);
    ordersTable->setHorizontalHeaderLabels({"订单号", "商品", "价格", "状态", "下单时间", "支付", "评价"});
    ordersTable->horizontalHeader()->setStretchLastSection(true);
    ordersTable->verticalHeader()->setVisible(false);

    // 示例订单1 - 待付款
    ordersTable->setItem(0, 0, new QTableWidgetItem("1001"));
    ordersTable->setItem(0, 1, new QTableWidgetItem("二手iPhone 12 128GB"));
    ordersTable->setItem(0, 2, new QTableWidgetItem("¥2500"));
    ordersTable->setItem(0, 3, new QTableWidgetItem("待付款"));
    ordersTable->setItem(0, 4, new QTableWidgetItem("2024-03-20 10:30"));

    // 支付按钮
    paymentBtn = new QPushButton("去支付");
    paymentBtn->setProperty("orderId", 1001);
    paymentBtn->setProperty("amount", 2500.00);
    connect(paymentBtn, &QPushButton::clicked, this, &MainWindow::onShowPayment);
    ordersTable->setCellWidget(0, 5, paymentBtn);

    // 示例订单2 - 已完成
    ordersTable->setItem(1, 0, new QTableWidgetItem("1002"));
    ordersTable->setItem(1, 1, new QTableWidgetItem("大学物理教材"));
    ordersTable->setItem(1, 2, new QTableWidgetItem("¥35"));
    ordersTable->setItem(1, 3, new QTableWidgetItem("已完成"));
    ordersTable->setItem(1, 4, new QTableWidgetItem("2024-03-18 14:20"));

    // 评价按钮
    reviewBtn = new QPushButton("评价");
    reviewBtn->setProperty("orderId", 1002);
    reviewBtn->setProperty("sellerName", "李四同学");
    connect(reviewBtn, &QPushButton::clicked, this, &MainWindow::onShowReview);
    ordersTable->setCellWidget(1, 6, reviewBtn);

    // 示例订单3 - 待收货
    ordersTable->setItem(2, 0, new QTableWidgetItem("1003"));
    ordersTable->setItem(2, 1, new QTableWidgetItem("篮球鞋 Nike Air"));
    ordersTable->setItem(2, 2, new QTableWidgetItem("¥280"));
    ordersTable->setItem(2, 3, new QTableWidgetItem("待收货"));
    ordersTable->setItem(2, 4, new QTableWidgetItem("2024-03-19 16:45"));

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(filterWidget);
    mainLayout->addWidget(ordersTable, 1);

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

        // 商品图片（模拟）
        QLabel *imageLabel = new QLabel();
        imageLabel->setPixmap(QPixmap(":/icons/goods_default.png").scaled(60, 60));
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
    // 这里应该根据分类筛选商品
    QMessageBox::information(this, "提示", QString("筛选分类: %1").arg(category));
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
    if (column == 0) return; // 点击图片列不处理

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

MainWindow::~MainWindow() {}
