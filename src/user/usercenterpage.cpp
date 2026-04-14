#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QHeaderView>
#include <QScrollArea>
#include <QGraphicsDropShadowEffect>
#include "UserCenterPage.h"
#include "..\apiservice.h"
#include "GoodsEditDialog.h"

UserCenterPage::UserCenterPage(QWidget *parent) : QWidget(parent) {
    setupUI();
    loadUserInfo();
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
            // 如果切换到浏览历史标签页（索引3），重新加载数据
            if (i == 3) {
                loadBrowseHistory();
            }
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

    connect(collectionList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        int goodsId = item->data(Qt::UserRole).toInt();
        if (goodsId > 0) {
            emit goodsDetailRequested(goodsId);   // 发射信号
        }
    });
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

    layout->addWidget(myGoodsTable, 1);

    // 分页控件
    QWidget *paginationWidget = new QWidget();
    QHBoxLayout *paginationLayout = new QHBoxLayout(paginationWidget);
    paginationLayout->setContentsMargins(0, 10, 0, 0);
    paginationLayout->setAlignment(Qt::AlignCenter);

    m_publishPrevBtn = new QPushButton("上一页");
    m_publishPrevBtn->setStyleSheet(
        "background-color: #3498db;"
        "color: white;"
        "border: none;"
        "border-radius: 4px;"
        "font-size: 13px;"
        );
    m_publishPrevBtn->setFixedSize(80, 32);
    m_publishNextBtn = new QPushButton("下一页");
    m_publishNextBtn->setStyleSheet(
        "background-color: #3498db;"
        "color: white;"
        "border: none;"
        "border-radius: 4px;"
        "font-size: 13px;"
        );
    m_publishNextBtn->setFixedSize(80, 32);
    m_publishPageLabel = new QLabel("第 1 页");
    m_publishPageLabel->setStyleSheet("font-size: 13px; color: #475569; margin: 0 15px;");

    paginationLayout->addWidget(m_publishPrevBtn);
    paginationLayout->addWidget(m_publishPageLabel);
    paginationLayout->addWidget(m_publishNextBtn);
    layout->addWidget(paginationWidget);

    connect(m_publishPrevBtn, &QPushButton::clicked, this, [this]() {
        if (m_publishCurrentPage > 1) {
            m_publishCurrentPage--;
            loadMyGoods(m_publishCurrentPage, m_publishPageSize);
        }
    });
    connect(m_publishNextBtn, &QPushButton::clicked, this, [this]() {
        m_publishCurrentPage++;
        loadMyGoods(m_publishCurrentPage, m_publishPageSize);
    });

    loadMyGoods(1, m_publishPageSize);
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

    layout->addWidget(collectionList, 1);

    // 分页控件
    QWidget *paginationWidget = new QWidget();
    QHBoxLayout *paginationLayout = new QHBoxLayout(paginationWidget);
    paginationLayout->setContentsMargins(0, 10, 0, 0);
    paginationLayout->setAlignment(Qt::AlignCenter);

    m_favPrevBtn = new QPushButton("上一页");
    m_favPrevBtn->setStyleSheet(
        "background-color: #3498db;"
        "color: white;"
        "border: none;"
        "border-radius: 4px;"
        "font-size: 13px;"
        );
    m_favPrevBtn->setFixedSize(80, 32);
    m_favNextBtn = new QPushButton("下一页");
    m_favNextBtn->setStyleSheet(
        "background-color: #3498db;"
        "color: white;"
        "border: none;"
        "border-radius: 4px;"
        "font-size: 13px;"
        );
    m_favNextBtn->setFixedSize(80, 32);
    m_favPageLabel = new QLabel("第 1 页");
    m_favPageLabel->setStyleSheet("font-size: 13px; color: #475569; margin: 0 15px;");

    paginationLayout->addWidget(m_favPrevBtn);
    paginationLayout->addWidget(m_favPageLabel);
    paginationLayout->addWidget(m_favNextBtn);
    layout->addWidget(paginationWidget);

    connect(m_favPrevBtn, &QPushButton::clicked, this, [this]() {
        if (m_favCurrentPage > 1) {
            loadFavorites(m_favCurrentPage - 1, m_favPageSize);
        }
    });
    connect(m_favNextBtn, &QPushButton::clicked, this, [this]() {
        loadFavorites(m_favCurrentPage + 1, m_favPageSize);
    });

    loadFavorites(1, m_favPageSize);
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

    layout->addWidget(reviewList, 1);

    // 分页控件
    QWidget *paginationWidget = new QWidget();
    QHBoxLayout *paginationLayout = new QHBoxLayout(paginationWidget);
    paginationLayout->setContentsMargins(0, 10, 0, 0);
    paginationLayout->setAlignment(Qt::AlignCenter);

    m_reviewPrevBtn = new QPushButton("上一页");
    m_reviewPrevBtn->setStyleSheet(
        "background-color: #3498db;"
        "color: white;"
        "border: none;"
        "border-radius: 4px;"
        "font-size: 13px;"
        );
    m_reviewPrevBtn->setFixedSize(80, 32);
    m_reviewNextBtn = new QPushButton("下一页");
    m_reviewNextBtn->setStyleSheet(
        "background-color: #3498db;"
        "color: white;"
        "border: none;"
        "border-radius: 4px;"
        "font-size: 13px;"
        );
    m_reviewNextBtn->setFixedSize(80, 32);
    m_reviewPageLabel = new QLabel("第 1 页");
    m_reviewPageLabel->setStyleSheet("font-size: 13px; color: #475569; margin: 0 15px;");

    paginationLayout->addWidget(m_reviewPrevBtn);
    paginationLayout->addWidget(m_reviewPageLabel);
    paginationLayout->addWidget(m_reviewNextBtn);
    layout->addWidget(paginationWidget);

    connect(m_reviewPrevBtn, &QPushButton::clicked, this, [this]() {
        if (m_reviewCurrentPage > 1) {
            loadMyReviews(m_reviewCurrentPage - 1, m_reviewPageSize);
        }
    });
    connect(m_reviewNextBtn, &QPushButton::clicked, this, [this]() {
        loadMyReviews(m_reviewCurrentPage + 1, m_reviewPageSize);
    });

    loadMyReviews(1, m_reviewPageSize);
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
    itemLayout->setContentsMargins(16, 12, 16, 16);
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

    // 组装所有组件
    itemLayout->addWidget(headerWidget);
    itemLayout->addWidget(itemLabel);
    itemLayout->addWidget(ratingWidget);
    itemLayout->addWidget(commentLabel);

    // 创建列表项
    QListWidgetItem *itemWidget = new QListWidgetItem();
    itemWidget->setSizeHint(QSize(0, 190)); // 设置固定高度
    reviewList->addItem(itemWidget);
    reviewList->setItemWidget(itemWidget, reviewWidget);
}

void UserCenterPage::createHistoryTab() {
    myHistoryWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(myHistoryWidget);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(10);

    // 历史记录容器（滚动区域内部）
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("border: none; background: transparent;");
    m_historyContainer = new QWidget();
    m_historyContainerLayout = new QVBoxLayout(m_historyContainer);
    m_historyContainerLayout->setContentsMargins(0, 0, 0, 0);
    m_historyContainerLayout->setSpacing(10);
    scrollArea->setWidget(m_historyContainer);
    layout->addWidget(scrollArea, 1);

    // 分页控件
    QWidget *paginationWidget = new QWidget();
    QHBoxLayout *paginationLayout = new QHBoxLayout(paginationWidget);
    paginationLayout->setContentsMargins(0, 10, 0, 0);
    paginationLayout->setAlignment(Qt::AlignCenter);

    m_historyPrevBtn = new QPushButton("上一页");
    m_historyPrevBtn->setStyleSheet(
        "background-color: #3498db;"
        "color: white;"
        "border: none;"
        "border-radius: 4px;"
        "font-size: 13px;"
        );
    m_historyPrevBtn->setFixedSize(80, 32);
    m_historyNextBtn = new QPushButton("下一页");
    m_historyNextBtn->setStyleSheet(
        "background-color: #3498db;"
        "color: white;"
        "border: none;"
        "border-radius: 4px;"
        "font-size: 13px;"
        );
    m_historyNextBtn->setFixedSize(80, 32);
    m_historyPageLabel = new QLabel("第 1 页");
    m_historyPageLabel->setStyleSheet("font-size: 13px; color: #475569; margin: 0 15px;");

    paginationLayout->addWidget(m_historyPrevBtn);
    paginationLayout->addWidget(m_historyPageLabel);
    paginationLayout->addWidget(m_historyNextBtn);
    layout->addWidget(paginationWidget);

    connect(m_historyPrevBtn, &QPushButton::clicked, this, [this]() {
        if (m_historyCurrentPage > 1) {
            loadBrowseHistory(m_historyCurrentPage - 1, m_historyPageSize);
        }
    });
    connect(m_historyNextBtn, &QPushButton::clicked, this, [this]() {
        loadBrowseHistory(m_historyCurrentPage + 1, m_historyPageSize);
    });

    loadBrowseHistory(1, m_historyPageSize);
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
        emit logoutRequested();   // 发射退出信号
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

void UserCenterPage::loadFavorites(int page, int pageSize) {
    m_favCurrentPage = page;
    m_favPageSize = pageSize;
    m_favPageLabel->setText(QString("第 %1 页").arg(page));

    collectionList->clear();

    QJsonArray favorites = ApiService::instance()->getFavorites(page, pageSize); // 页码、每页数量
    for (const QJsonValue &val : favorites) {
        QJsonObject goods = val.toObject();
        int goodsId = goods.value("id").toInt();
        QString title = goods.value("name").toString();
        double price = goods.value("price").toDouble();

        QListWidgetItem *item = new QListWidgetItem(QString("%1 %2 - ¥%3:%4").arg(goodsId).arg(title).arg(price).arg(goods.value("description").toString()));
        item->setData(Qt::UserRole, goodsId);             // 存储商品ID
        collectionList->addItem(item);
    }

    bool hasMore = (favorites.size() == pageSize);
    m_favNextBtn->setEnabled(hasMore);
    m_favPrevBtn->setEnabled(page > 1);
}

void UserCenterPage::refreshFavorites() {
    loadFavorites(); // 重新加载收藏列表
}

void UserCenterPage::loadMyGoods(int page, int pageSize) {
    m_publishCurrentPage = page;
    m_publishPageLabel->setText(QString("第 %1 页").arg(page));

    myGoodsTable->setRowCount(0);
    QJsonArray goodsList = ApiService::instance()->getMyGoods(page, pageSize);
    for (const QJsonValue &val : goodsList) {
        QJsonObject goods = val.toObject();
        int goodsId = goods.value("id").toInt();
        int row = myGoodsTable->rowCount();
        myGoodsTable->insertRow(row);
        myGoodsTable->setRowHeight(row, 65);

        myGoodsTable->setItem(row, 0, new QTableWidgetItem(goods.value("name").toString()));
        myGoodsTable->setItem(row, 1, new QTableWidgetItem(QString("¥%1").arg(goods.value("price").toDouble())));

        int status = goods.value("status").toString().toInt();
        QString statusText;
        if (status == 0) statusText = "待审核";
        else if (status == 1) statusText = "在售";
        else if (status == 2) statusText = "交易中";
        else if (status == 3) statusText = "已售出";
        else if (status == 4) statusText = "已拒绝";
        else if (status == 5) statusText = "已下架";
        else statusText = "未知";
        QTableWidgetItem *statusItem = new QTableWidgetItem(statusText);
        if (status == 1) statusItem->setForeground(QColor("#38A169"));
        else if (status == 0) statusItem->setForeground(QColor("#E67E22"));
        myGoodsTable->setItem(row, 2, statusItem);

        // 操作按钮：编辑/下架/重新上架等
        QWidget *actionWidget = new QWidget();
        QHBoxLayout *actionLayout = new QHBoxLayout(actionWidget);
        actionLayout->setContentsMargins(5, 1, 5, 1);
        actionLayout->setSpacing(5);

        // 编辑按钮（除交易中、已售出外都显示）
        if (status != 2 && status != 3) {
            QPushButton *editBtn = new QPushButton("编辑");
            editBtn->setFixedSize(60, 28);
            editBtn->setProperty("goodsId", goodsId);
            connect(editBtn, &QPushButton::clicked, this, &UserCenterPage::onEditGoods);
            actionLayout->addWidget(editBtn);
        }

        if (status == 1) {
            // 在售：下架
            QPushButton *offShelfBtn = new QPushButton("下架");
            offShelfBtn->setFixedSize(60, 28);
            offShelfBtn->setProperty("goodsId", goodsId);
            connect(offShelfBtn, &QPushButton::clicked, this, &UserCenterPage::onOffShelf);
            actionLayout->addWidget(offShelfBtn);
        } else if (status == 5) {
            // 已下架：申请上架
            QPushButton *applyBtn = new QPushButton("申请上架");
            applyBtn->setFixedSize(80, 28);
            applyBtn->setProperty("goodsId", goodsId);
            connect(applyBtn, &QPushButton::clicked, this, &UserCenterPage::onApplyShelve);
            actionLayout->addWidget(applyBtn);
        }
        // 待审核(0)、已拒绝(4) 只有编辑按钮，不添加其他
        actionLayout->addStretch();
        myGoodsTable->setCellWidget(row, 3, actionWidget);
    }
    bool hasMore = (goodsList.size() == pageSize);
    m_publishNextBtn->setEnabled(hasMore);
    m_publishPrevBtn->setEnabled(page > 1);
}

void UserCenterPage::loadMyReviews(int page, int pageSize) {
    m_reviewCurrentPage = page;
    m_reviewPageSize = pageSize;
    m_reviewPageLabel->setText(QString("第 %1 页").arg(page));

    reviewList->clear();
    QJsonArray reviews = ApiService::instance()->getMyReviews(page, pageSize);
    for (const QJsonValue &val : reviews) {
        QJsonObject review = val.toObject();
        QString date = review.value("create_time").toString().left(10);
        QString orderId = QString::number(review.value("order_id").toInt());
        QString goodsName = review.value("goods_name").toString();
        int rating = review.value("score").toString().toInt();
        QString comment = review.value("content").toString();
        addReviewItem(date, "订单 #" + orderId, goodsName, rating, comment);
    }

    bool hasMore = (reviews.size() == pageSize);
    m_reviewNextBtn->setEnabled(hasMore);
    m_reviewPrevBtn->setEnabled(page > 1);
}

void UserCenterPage::loadBrowseHistory(int page, int pageSize) {
    m_historyCurrentPage = page;
    m_historyPageSize = pageSize;
    m_historyPageLabel->setText(QString("第 %1 页").arg(page));

    // 清空容器
    QLayoutItem *child;
    while ((child = m_historyContainerLayout->takeAt(0)) != nullptr) {
        if (child->widget()) delete child->widget();
        delete child;
    }

    QJsonArray history = ApiService::instance()->getBrowseHistory(page, pageSize);
    if (history.isEmpty()) {
        QWidget *emptyWidget = createEmptyHistoryWidget();
        m_historyContainerLayout->addWidget(emptyWidget);
    } else {
        for (const QJsonValue &val : history) {
            QJsonObject item = val.toObject();
            QWidget *historyItem = createHistoryItem(item);
            m_historyContainerLayout->addWidget(historyItem);
        }
        m_historyContainerLayout->addStretch();
    }

    bool hasMore = (history.size() == pageSize);
    m_historyNextBtn->setEnabled(hasMore);
    m_historyPrevBtn->setEnabled(page > 1);
}

QWidget* UserCenterPage::createHistoryItem(const QJsonObject &goods) {
    QWidget *widget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(10, 10, 10, 10);

    QLabel *nameLabel = new QLabel(goods.value("name").toString());
    nameLabel->setStyleSheet("font-weight: bold;");
    QLabel *priceLabel = new QLabel(QString("¥%1").arg(goods.value("price").toDouble()));
    QLabel *timeLabel = new QLabel(goods.value("browse_time").toString());
    QPushButton *viewBtn = new QPushButton("查看详情");
    viewBtn->setFixedSize(80, 28);
    viewBtn->setProperty("goodsId", goods.value("id").toInt());
    connect(viewBtn, &QPushButton::clicked, [this, viewBtn](){
        emit goodsDetailRequested(viewBtn->property("goodsId").toInt());
    });

    layout->addWidget(nameLabel, 2);
    layout->addWidget(priceLabel, 1);
    layout->addWidget(timeLabel, 2);
    layout->addWidget(viewBtn);
    return widget;
}

void UserCenterPage::onOffShelf() {
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    int goodsId = btn->property("goodsId").toInt();
    // 调用下架 API (假设 updateGoodsStatus 或类似)
    QJsonObject result = ApiService::instance()->updateGoodsStatus(goodsId, 5); // 4=下架
    if (result.value("success").toBool()) {
        QMessageBox::information(this, "成功", "商品已下架");
        loadMyGoods();  // 刷新列表
    } else {
        QMessageBox::warning(this, "失败", result.value("error").toString());
    }
}

void UserCenterPage::onEditGoods()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    int goodsId = btn->property("goodsId").toInt();

    // 获取当前商品的完整数据（可从表格中已有的数据构建，或重新从服务端获取）
    // 简单起见，可以从服务端重新获取商品详情
    QJsonObject result = ApiService::instance()->getGoodsDetail(goodsId);
    if (!result.value("success").toBool()) {
        QMessageBox::warning(this, "错误", "获取商品信息失败");
        return;
    }
    QJsonObject goodsData = result.value("data").toObject();

    GoodsEditDialog *dialog = new GoodsEditDialog(goodsId, goodsData, this);
    connect(dialog, &GoodsEditDialog::goodsUpdated, this, [this]() {
        loadMyGoods(m_publishCurrentPage, m_publishPageSize);
    });
    dialog->show();
}

QWidget* UserCenterPage::createEmptyHistoryWidget() {
    QWidget *emptyWidget = new QWidget();
    emptyWidget->setStyleSheet("background-color: white; border-radius: 12px;");
    QVBoxLayout *emptyLayout = new QVBoxLayout(emptyWidget);
    emptyLayout->setAlignment(Qt::AlignCenter);
    emptyLayout->setSpacing(16);

    QWidget *iconContainer = new QWidget();
    iconContainer->setFixedSize(80, 80);
    iconContainer->setStyleSheet("background-color: #F7FAFC; border-radius: 40px;");
    QVBoxLayout *iconLayout = new QVBoxLayout(iconContainer);
    QLabel *iconLabel = new QLabel("👁️");
    iconLabel->setStyleSheet("font-size: 32px;");
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLayout->addWidget(iconLabel);

    QLabel *textLabel = new QLabel("暂无浏览历史");
    textLabel->setStyleSheet("font-size: 16px; color: #718096;");

    emptyLayout->addWidget(iconContainer);
    emptyLayout->addWidget(textLabel);
    return emptyWidget;
}

void UserCenterPage::onApplyShelve()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    int goodsId = btn->property("goodsId").toInt();

    QJsonObject result = ApiService::instance()->updateGoodsStatus(goodsId, 0); // 0 = 待审核
    if (result.value("success").toBool()) {
        QMessageBox::information(this, "成功", "已提交上架申请，等待管理员审核");
        loadMyGoods(); // 刷新列表
    } else {
        QMessageBox::warning(this, "失败", result.value("error").toString());
    }
}
