#include "UserCenterPage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QListWidget>
#include <QTextEdit>
#include <QHeaderView>
#include <QGraphicsDropShadowEffect>

UserCenterPage::UserCenterPage(QWidget *parent) : QWidget(parent) {
    setupUI();
}

void UserCenterPage::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(20);

    // ========== 1. 顶部用户信息卡片 ==========
    QWidget *userCard = new QWidget();
    userCard->setFixedHeight(200);
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
        border-radius: 60px;
        border: 1px solid white;
        background-color: white;
    )");
    QPixmap avatar(":/icons/img/user.png");
    userAvatarLabel->setPixmap(avatar.scaled(116, 116, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    userAvatarLabel->setScaledContents(true);

    // 1.2 用户信息文本区
    QVBoxLayout *infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(8);
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
    subInfoLayout->setContentsMargins(0, 0, 0, 0);

    userLevelLabel = new QLabel("信用分: 96");
    userLevelLabel->setStyleSheet("font-size: 14px; color: #4A5568;");
    userLevelLabel->setMinimumHeight(20);

    userJoinLabel = new QLabel("注册时间: 2024-03-01");
    userJoinLabel->setStyleSheet("font-size: 14px; color: #718096;");
    userJoinLabel->setMinimumHeight(20);

    subInfoLayout->addWidget(userLevelLabel);
    subInfoLayout->addWidget(userJoinLabel);

    // 按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
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
            border: none;
            padding: 6px 12px;
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
            border: none;
            padding: 6px 12px;
        }
        QPushButton:hover {
            background-color: #8E44AD;
        }
        QPushButton:pressed {
            background-color: #7D3C98;
        }
    )");

    connect(editProfileBtn, &QPushButton::clicked, this, &UserCenterPage::onEditProfile);
    connect(creditScoreBtn, &QPushButton::clicked, this, &UserCenterPage::onShowCreditScore);

    buttonLayout->addWidget(editProfileBtn);
    buttonLayout->addSpacing(15);
    buttonLayout->addWidget(creditScoreBtn);
    buttonLayout->addStretch();

    infoLayout->addWidget(userNameLabel);
    infoLayout->addWidget(subInfoWidget);
    infoLayout->addLayout(buttonLayout);

    cardLayout->addWidget(userAvatarLabel);
    cardLayout->addLayout(infoLayout);
    cardLayout->addStretch();

    // 添加阴影效果
    QGraphicsDropShadowEffect *cardShadow = new QGraphicsDropShadowEffect(this);
    cardShadow->setBlurRadius(12);
    cardShadow->setXOffset(0);
    cardShadow->setYOffset(4);
    cardShadow->setColor(QColor(0, 0, 0, 15));
    userCard->setGraphicsEffect(cardShadow);

    // ========== 2. 功能标签区 ==========
    QWidget *funcContentWidget = new QWidget();
    QHBoxLayout *funcContentLayout = new QHBoxLayout(funcContentWidget);
    funcContentLayout->setContentsMargins(0, 0, 0, 0);
    funcContentLayout->setSpacing(20);

    // 2.1 左侧菜单
    setupMenuTabs();

    QWidget *menuWidget = new QWidget();
    menuWidget->setFixedWidth(200);
    QVBoxLayout *menuLayout = new QVBoxLayout(menuWidget);
    menuLayout->setContentsMargins(0, 0, 0, 0);
    menuLayout->setSpacing(0);

    // 菜单标题
    QLabel *menuTitle = new QLabel("功能菜单");
    menuTitle->setStyleSheet(R"(
        font-size: 16px;
        font-weight: 600;
        color: #2D3748;
        padding: 10px 15px;
        border-bottom: 1px solid #E2E8F0;
    )");
    menuLayout->addWidget(menuTitle);

    // 菜单项
    QStringList menuItems = {"我的发布", "我的收藏", "评价记录", "浏览历史"};
    QStringList menuIcons = {"📦", "❤️", "⭐", "👁️"};

    for (int i = 0; i < menuItems.size(); ++i) {
        QPushButton *menuBtn = new QPushButton(menuIcons[i] + "  " + menuItems[i]);
        menuBtn->setFixedHeight(45);
        menuBtn->setCheckable(true);
        menuBtn->setProperty("tabIndex", i);

        menuBtn->setStyleSheet(R"(
            QPushButton {
                border: none;
                text-align: left;
                padding-left: 20px;
                font-size: 14px;
                color: #4A5568;
                background-color: transparent;
                border-left: 3px solid transparent;
            }
            QPushButton:hover {
                background-color: #F7FAFC;
                color: #2B6CB0;
            }
            QPushButton:checked {
                background-color: #EBF8FF;
                color: #2B6CB0;
                border-left: 3px solid #4299E1;
                font-weight: 500;
            }
        )");

        if (i == 0) menuBtn->setChecked(true);

        connect(menuBtn, &QPushButton::clicked, this, [this, i]() {
            userSubTabs->setCurrentIndex(i);
            emit menuTabChanged(i);
        });

        menuLayout->addWidget(menuBtn);
    }

    menuLayout->addStretch();

    // 2.2 右侧内容区
    userSubTabs = new QTabWidget();
    userSubTabs->setTabPosition(QTabWidget::North);
    userSubTabs->tabBar()->hide();
    userSubTabs->setStyleSheet(R"(
        QTabWidget::pane {
            border: 1px solid #F3F4F6;
            border-radius: 8px;
            background-color: white;
        }
    )");

    // 创建子页面
    createMyPublishTab();
    createMyCollectionTab();
    createReviewTab();
    createHistoryTab();

    // 添加标签页
    userSubTabs->addTab(myPublishWidget, "");
    userSubTabs->addTab(myCollectionWidget, "");
    userSubTabs->addTab(myReviewWidget, "");
    userSubTabs->addTab(myHistoryWidget, "");

    funcContentLayout->addWidget(menuWidget);
    funcContentLayout->addWidget(userSubTabs, 1);

    // ========== 组装所有模块 ==========
    mainLayout->addWidget(userCard);
    mainLayout->addWidget(funcContentWidget, 1);

    // 页面背景色
    this->setStyleSheet("background-color: #FAFAFA;");
}

void UserCenterPage::setupMenuTabs() {
    // 创建子页面
    myPublishWidget = new QWidget();
    myCollectionWidget = new QWidget();
    myReviewWidget = new QWidget();
    myHistoryWidget = new QWidget();
}

void UserCenterPage::createMyPublishTab() {
    QVBoxLayout *layout = new QVBoxLayout(myPublishWidget);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);

    QLabel *title = new QLabel("我的发布商品");
    title->setStyleSheet(R"(
        font-size: 18px;
        font-weight: 600;
        color: #2D3748;
        margin-bottom: 10px;
    )");

    myGoodsTable = new QTableWidget(0, 4);
    myGoodsTable->setHorizontalHeaderLabels({"商品", "价格", "状态", "操作"});
    myGoodsTable->verticalHeader()->setVisible(false);
    myGoodsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    myGoodsTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    myGoodsTable->setStyleSheet(R"(
        QTableWidget {
            border: 1px solid #F3F4F6;
            border-radius: 8px;
            background-color: white;
        }
        QTableWidget::item {
            padding: 12px 8px;
            border-bottom: 1px solid #F7FAFC;
            color: #4A5568;
        }
        QHeaderView::section {
            background-color: #F7FAFC;
            border: none;
            border-bottom: 1px solid #E2E8F0;
            padding: 12px 8px;
            color: #718096;
            font-size: 13px;
            font-weight: 600;
        }
    )");

    myGoodsTable->setColumnWidth(0, 200);
    myGoodsTable->setColumnWidth(1, 100);
    myGoodsTable->setColumnWidth(2, 100);
    myGoodsTable->horizontalHeader()->setStretchLastSection(true);

    // 添加示例数据
    QStringList goodsData = {"二手iPhone 12", "¥2500", "待售", "编辑/下架"};
    int row = myGoodsTable->rowCount();
    myGoodsTable->insertRow(row);
    for (int i = 0; i < goodsData.size(); i++) {
        QTableWidgetItem *item = new QTableWidgetItem(goodsData[i]);
        myGoodsTable->setItem(row, i, item);
    }

    layout->addWidget(title);
    layout->addWidget(myGoodsTable, 1);
}

void UserCenterPage::createMyCollectionTab() {
    QVBoxLayout *layout = new QVBoxLayout(myCollectionWidget);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);

    QLabel *title = new QLabel("我的收藏");
    title->setStyleSheet(R"(
        font-size: 18px;
        font-weight: 600;
        color: #2D3748;
        margin-bottom: 10px;
    )");

    collectionList = new QListWidget();
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
            background-color: #EBF8FF;
            color: #2B6CB0;
            border: none;
        }
    )");

    collectionList->addItems({
        "📚 大学物理教材 - ¥35",
        "👟 篮球鞋 Nike Air - ¥280",
        "💻 笔记本电脑戴尔 - ¥3200",
        "📱 二手iPhone 12 128GB - ¥2500"
    });

    layout->addWidget(title);
    layout->addWidget(collectionList, 1);
}

void UserCenterPage::createReviewTab() {
    QVBoxLayout *layout = new QVBoxLayout(myReviewWidget);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);

    QLabel *title = new QLabel("评价记录");
    title->setStyleSheet(R"(
        font-size: 18px;
        font-weight: 600;
        color: #2D3748;
        margin-bottom: 10px;
    )");

    reviewEdit = new QTextEdit();
    reviewEdit->setReadOnly(true);
    reviewEdit->setStyleSheet(R"(
        QTextEdit {
            border: 1px solid #F3F4F6;
            border-radius: 8px;
            padding: 20px;
            background-color: white;
            color: #4A5568;
            font-size: 14px;
            line-height: 1.6;
        }
    )");

    reviewEdit->setText(
        "📅 2024-03-18\n"
        "订单#1002 - 大学物理教材\n"
        "评分: ★★★★★ (5星)\n"
        "评价: 书本保存得很好，卖家很细心，交易很愉快！\n\n"
        "📅 2024-03-15\n"
        "订单#1004 - 笔记本电脑\n"
        "评分: ★★★★☆ (4星)\n"
        "评价: 电脑性能不错，就是电池续航稍弱，总体满意。\n\n"
        "📅 2024-03-10\n"
        "订单#1006 - 吉他\n"
        "评分: ★★★☆☆ (3星)\n"
        "评价: 琴弦有点旧了，需要自己更换。"
        );

    layout->addWidget(title);
    layout->addWidget(reviewEdit, 1);
}

void UserCenterPage::createHistoryTab() {
    QVBoxLayout *layout = new QVBoxLayout(myHistoryWidget);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setAlignment(Qt::AlignCenter);

    QLabel *iconLabel = new QLabel("👁️");
    iconLabel->setStyleSheet("font-size: 48px; color: #CBD5E0;");
    iconLabel->setAlignment(Qt::AlignCenter);

    QLabel *textLabel = new QLabel("暂无浏览历史");
    textLabel->setStyleSheet("font-size: 16px; color: #718096; margin-top: 15px;");
    textLabel->setAlignment(Qt::AlignCenter);

    layout->addStretch();
    layout->addWidget(iconLabel);
    layout->addWidget(textLabel);
    layout->addStretch();
}

void UserCenterPage::updateUserInfo(const QString &name, int creditScore, const QString &joinDate) {
    userNameLabel->setText(name);
    userLevelLabel->setText(QString("信用分: %1").arg(creditScore));
    userJoinLabel->setText(QString("注册时间: %1").arg(joinDate));
}

void UserCenterPage::onEditProfile() {
    emit editProfileRequested();
}

void UserCenterPage::onShowCreditScore() {
    emit creditScoreRequested();
}
