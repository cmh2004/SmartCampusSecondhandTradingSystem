#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QDateTime>
#include <QMessageBox>
#include <QScrollArea>
#include <QMouseEvent>
#include "HomePage.h"

HomePage::HomePage(QWidget *parent) : QWidget(parent) {
    setupUI();
}

void HomePage::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
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
    categoryWidget->setFixedWidth(175);
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

    // 右侧商品网格区域
    QWidget *goodsWidget = new QWidget();
    goodsWidget->setObjectName("goodsWidget");
    QVBoxLayout *goodsLayout = new QVBoxLayout(goodsWidget);
    goodsLayout->setContentsMargins(0, 0, 0, 0);

    // 创建滚动区域用于商品网格
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setObjectName("goodsScrollArea");
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setStyleSheet(R"(
        QScrollArea {
            border: 2px solid #f5f5f5;
            background-color: white;
        }
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
    )");

    // 网格容器
    goodsGridContainer = new QWidget();
    goodsGridContainer->setObjectName("goodsGridContainer");
    goodsGridContainer->setStyleSheet("#goodsGridContainer{background-color: #f3f4f6;}");
    goodsGridLayout = new QGridLayout(goodsGridContainer);
    goodsGridLayout->setContentsMargins(0, 0, 0, 0);
    goodsGridLayout->setSpacing(3);
    goodsGridLayout->setAlignment(Qt::AlignTop);

    scrollArea->setWidget(goodsGridContainer);

    goodsLayout->addWidget(scrollArea, 1);

    // 添加到内容区
    contentLayout->addWidget(categoryWidget);
    contentLayout->addWidget(goodsWidget, 1);

    // 添加到主布局
    mainLayout->addWidget(searchBar);
    mainLayout->addWidget(welcomeLabel);
    mainLayout->addWidget(contentArea, 1);

    // 连接信号槽
    connect(categoryList, &QListWidget::itemClicked, this, &HomePage::onCategoryClicked);
    connect(searchBtn, &QPushButton::clicked, this, &HomePage::onSearchClicked);
}

void HomePage::loadMockData() {
    // 清空现有商品
    QLayoutItem* child;
    while ((child = goodsGridLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    QStringList goodsNames = {
        "二手iPhone 12 128GB", "大学物理教材", "篮球鞋 Nike Air",
        "笔记本电脑戴尔", "英语四级词汇书", "小米手环6", "吉他雅马哈", "考研数学复习全书",
        "无线蓝牙耳机", "冬季羽绒服", "二手iPad Air", "Java编程思想",
        "电竞游戏鼠标", "Office 365激活码", "尤克里里小吉他", "戴尔显示器"
    };

    QStringList prices = {"2500", "35", "280", "3200", "15", "150", "800", "40",
                          "120", "380", "1800", "78", "89", "25", "350", "600"};

    QStringList categories = {"电子产品", "书籍教材", "服饰鞋包", "电子产品",
                              "书籍教材", "电子产品", "其他", "书籍教材",
                              "电子产品", "服饰鞋包", "电子产品", "书籍教材",
                              "电子产品", "其他", "其他", "电子产品"};

    // 列数（根据窗口大小调整，这里固定为4列）
    int columns = 4;

    for (int i = 0; i < goodsNames.size(); i++) {
        // 创建商品卡片
        QWidget *goodsCard = createGoodsCard(
            i + 1000, // 商品ID
            goodsNames[i],
            prices[i],
            categories[i],
            (i % 3 == 0) ? "待售" : (i % 3 == 1) ? "交易中" : "已售出"
            );

        // 计算行和列
        int row = i / columns;
        int col = i % columns;

        goodsGridLayout->addWidget(goodsCard, row, col);
    }
}

QWidget* HomePage::createGoodsCard(int goodsId, const QString& name,
                                   const QString& price, const QString& category,
                                   const QString& status) {
    QWidget *card = new QWidget();
    card->setObjectName("goodsCard");
    card->setFixedSize(233, 282);
    card->setCursor(Qt::PointingHandCursor);

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(12, 12, 12, 12);
    cardLayout->setSpacing(10);

    // 商品图片区域
    QWidget *imageContainer = new QWidget();
    imageContainer->setObjectName("imageContainer");
    imageContainer->setFixedHeight(160);
    QVBoxLayout *imageLayout = new QVBoxLayout(imageContainer);
    imageLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *imageLabel = new QLabel();
    imageLabel->setAlignment(Qt::AlignCenter);

    // 根据类别选择不同的占位图
    QString iconPath = ":/icons/img/buy.png"; // 默认图标

    imageLabel->setPixmap(QPixmap(iconPath).scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    imageLayout->addWidget(imageLabel);

    // 商品信息区域
    QWidget *infoWidget = new QWidget();
    QVBoxLayout *infoLayout = new QVBoxLayout(infoWidget);
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->setSpacing(6);

    // 商品名称
    QLabel *nameLabel = new QLabel(name);
    nameLabel->setObjectName("goodsName");
    nameLabel->setWordWrap(true);
    nameLabel->setMaximumHeight(40);

    // 商品价格
    QLabel *priceLabel = new QLabel(QString("¥%1").arg(price));
    priceLabel->setObjectName("goodsPrice");

    // 状态标签
    QLabel *statusLabel = new QLabel(status);
    statusLabel->setObjectName("goodsStatus");
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setFixedHeight(22);

    // 设置状态颜色
    QString statusStyle;
    if (status == "待售") {
        statusStyle = "color: #10B981; background-color: #D1FAE5;";
    } else if (status == "交易中") {
        statusStyle = "color: #F59E0B; background-color: #FEF3C7;";
    } else {
        statusStyle = "color: #6B7280; background-color: #F3F4F6;";
    }
    statusLabel->setStyleSheet(QString("border-radius: 4px; font-size: 11px; %1").arg(statusStyle));

    infoLayout->addWidget(nameLabel);
    infoLayout->addWidget(priceLabel);
    infoLayout->addWidget(statusLabel);

    cardLayout->addWidget(imageContainer);
    cardLayout->addWidget(infoWidget);

    // 存储商品ID
    card->setProperty("goodsId", goodsId);

    // 连接点击事件
    connect(card, &QWidget::customContextMenuRequested, [this, card]() {
        int goodsId = card->property("goodsId").toInt();
        emit goodsDetailRequested(goodsId);
    });

    // 使用事件过滤器处理点击事件
    card->installEventFilter(this);

    // 设置卡片样式
    card->setStyleSheet(R"(
        #goodsCard {
            background-color: white;
            border-radius: 12px;
            border: 1px solid #E5E7EB;
        }
        #goodsCard:hover {
            border-color: #3B82F6;
        }
        #imageContainer {
            background-color: #F9FAFB;
            border-radius: 8px;
        }
        #goodsName {
            font-size: 14px;
            font-weight: 500;
            color: #1F2937;
            line-height: 1.3;
        }
        #goodsPrice {
            font-size: 18px;
            font-weight: bold;
            color: #3B82F6;
        }
    )");

    return card;
}

bool HomePage::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            QWidget *widget = qobject_cast<QWidget*>(watched);
            if (widget && widget->objectName() == "goodsCard") {
                int goodsId = widget->property("goodsId").toInt();
                emit goodsDetailRequested(goodsId);
                return true;
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void HomePage::onCategoryClicked(QListWidgetItem* item) {
    QString category = item->text();
    // 移除emoji图标
    category = category.mid(category.indexOf(" ") + 1);
    welcomeLabel->setText(QString("当前分类: %1").arg(category));
    emit categoryChanged(category);

    // 这里应该根据分类筛选商品
    // 暂时用模拟数据
    loadMockData();
}

void HomePage::onSearchClicked() {
    QString keyword = searchEdit->text().trimmed();
    if (keyword.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入搜索关键词");
        return;
    }
    emit searchRequested(keyword);

    // 这里应该执行搜索逻辑
    // 暂时用模拟数据
    loadMockData();
}
