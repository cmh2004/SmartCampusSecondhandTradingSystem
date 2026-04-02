#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QHeaderView>
#include <QGraphicsDropShadowEffect>
#include "UserCenterPage.h"
#include "..\apiservice.h"

UserCenterPage::UserCenterPage(QWidget *parent) : QWidget(parent) {
    setupUI();
}

void UserCenterPage::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(20);

    // ========== 1. 顶部用户信息卡片 ==========
    QWidget *userCard = new QWidget();
    userCard->setFixedHeight(180);
    userCard->setStyleSheet(R"(
        background-color: #F0F8FB;
        border-radius: 16px;
        padding: 12px;
    )");
    QHBoxLayout *cardLayout = new QHBoxLayout(userCard);
    cardLayout->setContentsMargins(10, 0, 0, 5);
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
    infoLayout->setSpacing(4);
    infoLayout->setAlignment(Qt::AlignCenter);

    userNameLabel = new QLabel("张三同学");
    userNameLabel->setStyleSheet(R"(
        font-size: 20px;
        font-weight: 600;
        color: #2D3748;
    )");

    QWidget *subInfoWidget = new QWidget();
    QHBoxLayout *subInfoLayout = new QHBoxLayout(subInfoWidget);
    subInfoLayout->setSpacing(0);
    subInfoLayout->setContentsMargins(0, 0, 0, 0);

    userLevelLabel = new QLabel("信用分: 96");
    userLevelLabel->setStyleSheet("font-size: 14px; color: #4A5568;");
    userLevelLabel->setMinimumHeight(16);

    userJoinLabel = new QLabel("注册时间: 2024-03-01");
    userJoinLabel->setStyleSheet("font-size: 14px; color: #718096;");
    userJoinLabel->setMinimumHeight(16);

    subInfoLayout->addWidget(userLevelLabel);
    subInfoLayout->addWidget(userJoinLabel);
    subInfoLayout->addStretch();
    // 按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *editProfileBtn = new QPushButton("编辑资料");
    QPushButton *creditScoreBtn = new QPushButton("信用分详情");
    QPushButton *logoutBtn = new QPushButton("退出登录");

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

    logoutBtn->setFixedSize(120, 40);
    logoutBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #e74c3c;
            color: white;
            border-radius: 8px;
            font-weight: 600;
            font-size: 14px;
            border: none;
            padding: 6px 12px;
        }
        QPushButton:hover {
            background-color: #c0392b;
        }
        QPushButton:pressed {
            background-color: #a93226;
        }
    )");

    connect(editProfileBtn, &QPushButton::clicked, this, &UserCenterPage::onEditProfile);
    connect(creditScoreBtn, &QPushButton::clicked, this, &UserCenterPage::onShowCreditScore);
    connect(logoutBtn, &QPushButton::clicked, this, &UserCenterPage::onLogout);

    buttonLayout->addWidget(editProfileBtn);
    buttonLayout->addSpacing(20);
    buttonLayout->addWidget(creditScoreBtn);
    buttonLayout->addSpacing(20);
    buttonLayout->addWidget(logoutBtn);
    buttonLayout->addStretch();

    infoLayout->addWidget(userNameLabel);
    infoLayout->addWidget(subInfoWidget);
    infoLayout->addLayout(buttonLayout);

    cardLayout->addWidget(userAvatarLabel);
    cardLayout->addLayout(infoLayout);

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
    menuWidget->setFixedWidth(190);
    QVBoxLayout *menuLayout = new QVBoxLayout(menuWidget);
    menuLayout->setContentsMargins(0, 0, 0, 0);
    menuLayout->setSpacing(0);

    // 菜单项
    QStringList menuItems = {"我的发布", "我的收藏", "评价记录", "浏览历史"};
    QStringList menuIcons = {"📦", "❤️", "⭐", "👁️"};
    QList<QPushButton*> buttons;

    for (int i = 0; i < menuItems.size(); ++i) {
        QPushButton *menuBtn = new QPushButton(menuIcons[i] + "  " + menuItems[i]);
        menuBtn->setFixedHeight(48);
        menuBtn->setCheckable(true);
        menuBtn->setProperty("tabIndex", i);
        menuBtn->setCursor(Qt::PointingHandCursor);

        menuBtn->setStyleSheet(R"(
            QPushButton {
                border: none;
                text-align: left;
                padding-left: 24px;
                font-size: 14px;
                color: #4A5568;
                background-color: white;
                border-left: 4px solid transparent;
                border-radius: 0;
            }
            QPushButton:hover {
                background-color: #F7FAFC;
                color: #2B6CB0;
            }
            QPushButton:checked {
                background-color: #EBF8FF;
                color: #2B6CB0;
                border-left: 4px solid #4299E1;
                font-weight: 500;
            }
        )");

        if (i == 0) menuBtn->setChecked(true);
        buttons.push_back(menuBtn);

        connect(menuBtn, &QPushButton::clicked, this, [this, i]() {
            userSubTabs->setCurrentIndex(i);
        });

        menuLayout->addWidget(menuBtn);

        // 添加菜单项之间的分隔线
        if (i < menuItems.size() - 1) {
            QFrame *line = new QFrame();
            line->setFrameShape(QFrame::HLine);
            line->setFrameShadow(QFrame::Sunken);
            line->setStyleSheet("color: #EDF2FF; margin: 0 10px;");
            menuLayout->addWidget(line);
        }
    }

    for(int i = 0; i < buttons.size(); ++i){
        connect(buttons[i], &QPushButton::clicked, this, [=](){
            for(int j = 0; j < buttons.size(); ++j){
                if(i != j){
                    buttons[j]->setChecked(false);
                }
                else{
                    buttons[j]->setChecked(true);
                }
            }
        });
    }

    menuLayout->addStretch();

    menuWidget->setStyleSheet(R"(
        QWidget {
            background-color: white;
            border-radius: 12px;
            border: 1px solid #E2E8F0;
        }
    )");

    QGraphicsDropShadowEffect *menuShadow = new QGraphicsDropShadowEffect();
    menuShadow->setBlurRadius(12);
    menuShadow->setXOffset(0);
    menuShadow->setYOffset(2);
    menuShadow->setColor(QColor(0, 0, 0, 10));
    menuWidget->setGraphicsEffect(menuShadow);

    // 2.2 右侧内容区
    userSubTabs = new QTabWidget();
    userSubTabs->setTabPosition(QTabWidget::North);
    userSubTabs->tabBar()->hide();
    userSubTabs->setStyleSheet(R"(
        QTabWidget {
            background: transparent;
            border: none;
        }
        QTabWidget::pane {
            border: 1px solid #E2E8F0;
            border-radius: 12px;
            background-color: white;
            margin: 0;
            padding: 0;
        }
    )");

    // 创建子页面
    createMyPublishTab();
    createMyCollectionTab();
    createReviewTab();
    createHistoryTab();

    // 为每个子页面容器添加圆角样式
    QString tabStyle = R"(
        QWidget {
            background-color: white;
            border-radius: 12px;
        }
    )";
    myPublishWidget->setStyleSheet(tabStyle);
    myCollectionWidget->setStyleSheet(tabStyle);
    myReviewWidget->setStyleSheet(tabStyle);
    myHistoryWidget->setStyleSheet(tabStyle);

    // 添加标签页
    userSubTabs->addTab(myPublishWidget, "");
    userSubTabs->addTab(myCollectionWidget, "");
    userSubTabs->addTab(myReviewWidget, "");
    userSubTabs->addTab(myHistoryWidget, "");

    // 为右侧内容区添加阴影
    QGraphicsDropShadowEffect *contentShadow = new QGraphicsDropShadowEffect();
    contentShadow->setBlurRadius(16);
    contentShadow->setXOffset(0);
    contentShadow->setYOffset(4);
    contentShadow->setColor(QColor(0, 0, 0, 8));
    userSubTabs->setGraphicsEffect(contentShadow);

    funcContentLayout->addWidget(menuWidget);
    funcContentLayout->addWidget(userSubTabs, 1);

    // ========== 组装所有模块 ==========
    mainLayout->addWidget(userCard);
    mainLayout->addWidget(funcContentWidget, 1);

    // 页面背景色
    this->setStyleSheet("background-color: #FAFAFA;");
}

void UserCenterPage::createMyPublishTab() {
    QVBoxLayout *layout = new QVBoxLayout(myPublishWidget);
    layout->setContentsMargins(10, 10, 10, 10);  // 增加内边距
    layout->setSpacing(5);

    myGoodsTable = new QTableWidget(0, 4);
    myGoodsTable->setHorizontalHeaderLabels({"商品", "价格", "状态", "操作"});
    myGoodsTable->verticalHeader()->setVisible(false);
    myGoodsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    myGoodsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    myGoodsTable->setAlternatingRowColors(true);

    myGoodsTable->setStyleSheet(R"(
        QTableWidget {
            border: 1px solid #E2E8F0;
            border-radius: 8px;
            background-color: white;
            outline: none;
        }
        QTableWidget::item {
            padding: 16px 12px;
            border-bottom: 1px solid #F7FAFC;
            color: #4A5568;
            font-size: 14px;
            outline: none;
        }
        QTableWidget::item:alternate {
            background-color: #F9FAFB;
        }
        QHeaderView::section {
            background-color: #F7FAFC;
            border: none;
            border-bottom: 1px solid #E2E8F0;
            padding: 16px 12px;
            color: #718096;
            font-size: 13px;
            font-weight: 600;
        }
        QTableWidget::item:hover {
            background-color: #F3F4F6;
        }
    )");

    myGoodsTable->setColumnWidth(0, 250);
    myGoodsTable->setColumnWidth(1, 120);
    myGoodsTable->setColumnWidth(2, 150);
    myGoodsTable->horizontalHeader()->setStretchLastSection(true);

    // 添加示例数据
    QStringList goodsData = {"二手iPhone 12", "¥2500", "待售", "编辑/下架"};
    int row = myGoodsTable->rowCount();
    myGoodsTable->insertRow(row);
    for (int i = 0; i < goodsData.size(); i++) {
        QTableWidgetItem *item = new QTableWidgetItem(goodsData[i]);
        if (i == 2) { // 状态列
            item->setForeground(QColor("#38A169")); // 绿色
            item->setFont(QFont("Microsoft YaHei", -1, QFont::Medium));
        }
        myGoodsTable->setItem(row, i, item);
    }

    layout->addWidget(myGoodsTable, 1);
}

void UserCenterPage::createMyCollectionTab() {
    QVBoxLayout *layout = new QVBoxLayout(myCollectionWidget);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(5);

    collectionList = new QListWidget();
    collectionList->setStyleSheet(R"(
        QListWidget {
            border: 1px solid #E2E8F0;
            border-radius: 8px;
            background-color: white;
            outline: none;
        }
        QListWidget::item {
            padding: 16px 20px;
            border-bottom: 1px solid #F7FAFC;
            color: #4A5568;
            font-size: 14px;
            outline: none;
        }
        QListWidget::item:hover {
            background-color: #F9FAFB;
            outline: none;
        }
        QListWidget::item:selected {
            background-color: #EBF8FF;
            color: #2B6CB0;
            border: none;
            outline: none;
        }
    )");

    loadFavorites();

    layout->addWidget(collectionList, 1);
}

void UserCenterPage::createReviewTab() {
    QVBoxLayout *layout = new QVBoxLayout(myReviewWidget);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(5);

    // 创建评价列表
    reviewList = new QListWidget();
    reviewList->setStyleSheet(R"(
        QListWidget {
            border: 1px solid #E2E8F0;
            border-radius: 8px;
            background-color: white;
            outline: none;
        }
        QListWidget::item {
            border: none;
            padding: 2px;
            margin: 2px;
        }
        QListWidget::item:selected {
            background-color: #BEE3F8 ;
            border:2px solid #E8F4FF ;
        }
        QScrollBar:vertical {
            border: none;
            background-color: #F7FAFC;
            width: 8px;
            border-radius: 4px;
            margin: 2px;
        }
        QScrollBar::handle:vertical {
            background-color: #CBD5E0;
            border-radius: 4px;
            min-height: 30px;
        }
        QScrollBar::handle:vertical:hover {
            background-color: #A0AEC0;
        }
    )");

    // 添加评价数据
    addReviewItem("2024-03-18", "订单 #1002", "大学物理教材", 5,
                  "书本保存得很好，卖家很细心，交易很愉快！包装也很用心。");
    addReviewItem("2024-03-15", "订单 #1004", "笔记本电脑", 4,
                  "电脑性能不错，就是电池续航稍弱，总体满意。性价比很高。");
    addReviewItem("2024-03-10", "订单 #1006", "吉他", 3,
                  "琴弦有点旧了，需要自己更换。音色还可以，价格适中。");
    addReviewItem("2024-02-28", "订单 #1008", "篮球鞋", 5,
                  "鞋子很新，穿起来很舒服。卖家发货很快，非常满意！");
    addReviewItem("2024-02-20", "订单 #1010", "耳机", 4,
                  "音质不错，续航能力强。包装有点简单，但整体很好。");

    layout->addWidget(reviewList, 1);
}

// 辅助函数：添加评价项
void UserCenterPage::addReviewItem(const QString &date, const QString &orderId,
                                   const QString &item, int rating, const QString &comment) {
    QWidget *reviewWidget = new QWidget();
    reviewWidget->setObjectName("reviewWidget");
    reviewWidget->setStyleSheet(R"(
        QWidget {
            background-color: white;
            border-bottom: 1px solid #F7FAFC;
        }
        #reviewWidget {
            border:1px solid #F0F0F0;
        }
    )");

    QVBoxLayout *itemLayout = new QVBoxLayout(reviewWidget);
    itemLayout->setContentsMargins(16, 16, 16, 20);
    itemLayout->setSpacing(8);

    // 头部：日期和订单号
    QWidget *headerWidget = new QWidget();
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(8);

    QLabel *dateLabel = new QLabel(date);
    dateLabel->setStyleSheet(R"(
        font-size: 13px;
        color: #718096;
        font-weight: 500;
    )");

    QLabel *orderLabel = new QLabel(orderId);
    orderLabel->setStyleSheet(R"(
        font-size: 13px;
        color: #4299E1;
        font-weight: 500;
    )");

    headerLayout->addWidget(dateLabel);
    headerLayout->addWidget(orderLabel);
    headerLayout->addStretch();

    // 商品信息
    QLabel *itemLabel = new QLabel("商品：" + item);
    itemLabel->setStyleSheet(R"(
        font-size: 14px;
        font-weight: 600;
        color: #2D3748;
    )");

    // 评分区域
    QWidget *ratingWidget = new QWidget();
    QHBoxLayout *ratingLayout = new QHBoxLayout(ratingWidget);
    ratingLayout->setContentsMargins(0, 0, 0, 0);
    ratingLayout->setSpacing(8);

    QLabel *ratingText = new QLabel("评分：");
    ratingText->setStyleSheet("font-size: 13px; color: #718096;");

    // 星级显示
    QWidget *starsWidget = new QWidget();
    QHBoxLayout *starsLayout = new QHBoxLayout(starsWidget);
    starsLayout->setContentsMargins(0, 0, 0, 0);
    starsLayout->setSpacing(2);

    for (int i = 0; i < 5; ++i) {
        QLabel *starLabel = new QLabel();
        if (i < rating) {
            starLabel->setText("★");
            starLabel->setStyleSheet(R"(
                color: #F6AD55;
                font-size: 16px;
            )");
        } else {
            starLabel->setText("☆");
            starLabel->setStyleSheet(R"(
                color: #E2E8F0;
                font-size: 16px;
            )");
        }
        starsLayout->addWidget(starLabel);
    }

    // 评分数字
    QLabel *ratingNumLabel = new QLabel(QString("(%1星)").arg(rating));
    ratingNumLabel->setStyleSheet(R"(
        font-size: 13px;
        color: #718096;
        margin-left: 4px;
    )");

    ratingLayout->addWidget(ratingText);
    ratingLayout->addWidget(starsWidget);
    ratingLayout->addWidget(ratingNumLabel);
    ratingLayout->addStretch();

    // 评价内容
    QLabel *commentLabel = new QLabel(comment);
    commentLabel->setStyleSheet(R"(
        font-size: 13px;
        color: #4A5568;
        line-height: 1.5;
        padding-top: 4px;
    )");
    commentLabel->setWordWrap(true);

    // 底部操作按钮
    QWidget *actionWidget = new QWidget();
    QHBoxLayout *actionLayout = new QHBoxLayout(actionWidget);
    actionLayout->setContentsMargins(0, 0, 0, 0);
    actionLayout->setSpacing(12);

    QPushButton *editBtn = new QPushButton("编辑");
    editBtn->setFixedSize(60, 28);
    editBtn->setStyleSheet(R"(
        QPushButton {
            background-color: transparent;
            border: 1px solid #E2E8F0;
            border-radius: 4px;
            color: #718096;
            font-size: 12px;
            padding: 4px 8px;
        }
        QPushButton:hover {
            background-color: #F7FAFC;
            color: #4299E1;
        }
    )");
    editBtn->setCursor(Qt::PointingHandCursor);

    actionLayout->addStretch();
    actionLayout->addWidget(editBtn);

    // 组装所有组件
    itemLayout->addWidget(headerWidget);
    itemLayout->addWidget(itemLabel);
    itemLayout->addWidget(ratingWidget);
    itemLayout->addWidget(commentLabel);
    itemLayout->addWidget(actionWidget);

    // 创建列表项
    QListWidgetItem *itemWidget = new QListWidgetItem();
    itemWidget->setSizeHint(QSize(0, 190)); // 设置固定高度
    reviewList->addItem(itemWidget);
    reviewList->setItemWidget(itemWidget, reviewWidget);
}

void UserCenterPage::createHistoryTab() {
    QVBoxLayout *layout = new QVBoxLayout(myHistoryWidget);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setAlignment(Qt::AlignCenter);

    // 空状态容器
    QWidget *emptyWidget = new QWidget();
    emptyWidget->setStyleSheet("background-color: white; border-radius: 12px;");

    QVBoxLayout *emptyLayout = new QVBoxLayout(emptyWidget);
    emptyLayout->setAlignment(Qt::AlignCenter);
    emptyLayout->setSpacing(16);

    // 图标容器
    QWidget *iconContainer = new QWidget();
    iconContainer->setFixedSize(80, 80);
    iconContainer->setStyleSheet(R"(
        background-color: #F7FAFC;
        border-radius: 40px;
    )");

    QVBoxLayout *iconLayout = new QVBoxLayout(iconContainer);
    QLabel *iconLabel = new QLabel("👁️");
    iconLabel->setStyleSheet("font-size: 32px;");
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLayout->addWidget(iconLabel);

    QLabel *textLabel = new QLabel("暂无浏览历史");
    textLabel->setStyleSheet("font-size: 16px; color: #718096;");

    emptyLayout->addWidget(iconContainer);
    emptyLayout->addWidget(textLabel);

    layout->addWidget(emptyWidget);
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

void UserCenterPage::setupMenuTabs() {
    // 创建子页面
    myPublishWidget = new QWidget();
    myCollectionWidget = new QWidget();
    myReviewWidget = new QWidget();
    myHistoryWidget = new QWidget();
}

void UserCenterPage::onLogout() {
    // 显示确认对话框
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "退出登录",
        "确定要退出登录吗？\n退出后需要重新登录才能使用。",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {

        // 可以在这里清除用户信息（可选）
        userNameLabel->setText("未登录");
        userLevelLabel->setText("信用分: --");
        userJoinLabel->setText("注册时间: --");

        // 可以添加一个简单的退出提示
        QMessageBox::information(this, "退出成功", "您已成功退出登录！");
    }
}

void UserCenterPage::loadUserInfo() {
    QJsonObject result = ApiService::instance()->getUserProfile();
    if (result.value("success").toBool()) {
        QJsonObject data = result.value("data").toObject();
        userNameLabel->setText(data.value("nickname").toString());
        int creditScore = data.value("credit_score").toInt();
        userLevelLabel->setText(QString("信用分: %1").arg(creditScore));
        userJoinLabel->setText(QString("注册时间: %1").arg(data.value("register_time").toString()));
        // 头像加载
        QString avatarUrl = data.value("avatar_url").toString();
        if (!avatarUrl.isEmpty()) {
            // 如果 avatarUrl 是相对路径，补全 base URL
            if (!avatarUrl.startsWith("http")) {
                avatarUrl = "http://127.0.0.1:8080" + avatarUrl;
            }
            // 异步加载图片
            QNetworkAccessManager *nam = new QNetworkAccessManager(this);
            connect(nam, &QNetworkAccessManager::finished, [this, nam](QNetworkReply *reply) {
                if (reply->error() == QNetworkReply::NoError) {
                    QPixmap pixmap;
                    pixmap.loadFromData(reply->readAll());
                    if (!pixmap.isNull()) {
                        userAvatarLabel->setPixmap(pixmap.scaled(116, 116, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    }
                }
                reply->deleteLater();
                nam->deleteLater();
            });
            nam->get(QNetworkRequest(QUrl(avatarUrl)));
        }
    } else {
        QMessageBox::warning(this, "提示", "加载用户信息失败");
    }
}

void UserCenterPage::loadFavorites() {
    QJsonArray favorites = ApiService::instance()->getFavorites(1, 20); // 页码、每页数量
    collectionList->clear();
    for (const QJsonValue &val : favorites) {
        QJsonObject goods = val.toObject();
        QString title = goods.value("title").toString();
        double price = goods.value("price").toDouble();
        collectionList->addItem(QString("%1 - ¥%2").arg(title).arg(price));
    }
}
