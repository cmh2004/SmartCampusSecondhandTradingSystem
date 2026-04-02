#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QDateTime>
#include <QMessageBox>
#include <QScrollArea>
#include <QMouseEvent>
#include <QJsonArray>
#include <QJsonObject>
#include "HomePage.h"
#include "..\apiservice.h"

HomePage::HomePage(QWidget *parent) : QWidget(parent) {
    setupUI();
    loadGoodsFromServer("", "全部", 0, 0, "newest", 1, 20);
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
    connect(sortCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index) {
        loadGoodsFromServer(searchEdit->text().trimmed(),
                            getCurrentCategory(),
                            0, 0,
                            getSortByValue(),
                            1, 20);
    });
}

void HomePage::loadGoodsFromServer(const QString &keyword, const QString &category,
                                   double minPrice, double maxPrice, const QString &sortBy,
                                   int page, int pageSize) {
    QJsonArray goodsArray = ApiService::instance()->searchGoods(keyword, category, minPrice, maxPrice, sortBy, page, pageSize);

    // 1. 清空现有商品网格
    clearGoodsGrid();  // 该函数需在 HomePage 中实现

    // 2. 添加新商品卡片到网格
    int columns = 4;  // 固定列数，可根据窗口宽度动态调整
    for (int i = 0; i < goodsArray.size(); ++i) {
        QJsonObject goods = goodsArray[i].toObject();
        int goodsId = goods.value("goods_id").toInt();
        QString name = goods.value("title").toString();
        double price = goods.value("price").toDouble();
        QString categoryName = goods.value("category").toString();
        QString status = goods.value("status").toString(); // 待售等

        // 创建商品卡片
        QWidget *card = createGoodsCard(goodsId, name, QString::number(price), categoryName, status);

        // 计算行列位置
        int row = i / columns;
        int col = i % columns;
        goodsGridLayout->addWidget(card, row, col);
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

    loadGoodsFromServer(searchEdit->text().trimmed(), category, 0, 0, getSortByValue(), 1, 20);
}

void HomePage::onSearchClicked() {
    QString keyword = searchEdit->text().trimmed();
    if (keyword.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入搜索关键词");
        return;
    }
    emit searchRequested(keyword);

    // 根据当前选择的分类和排序条件，执行搜索
    QString currentCategory = categoryList->currentItem()->text();
    // 移除 emoji 图标
    currentCategory = currentCategory.mid(currentCategory.indexOf(" ") + 1);
    QString sortBy = getSortByValue(); // 需要实现从 sortCombo 获取排序字段
    loadGoodsFromServer(keyword, currentCategory, 0, 0, sortBy, 1, 20);
}

void HomePage::clearGoodsGrid() {
    QLayoutItem *child;
    while ((child = goodsGridLayout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            delete child->widget();
        }
        delete child;
    }
}

QString HomePage::getSortByValue() const {
    int idx = sortCombo->currentIndex();
    switch (idx) {
        case 0: return "newest";      // 最新发布
        case 1: return "price_asc";   // 价格最低
        case 2: return "price_desc";  // 价格最高
        case 3: return "view_count";  // 最热商品
        default: return "newest";
    }
}

QString HomePage::getCurrentCategory() const {
    if (!categoryList->currentItem()) return "全部";
    QString text = categoryList->currentItem()->text();
    // 移除开头的 emoji 图标（如"📦 全部商品" -> "全部商品"）
    int spacePos = text.indexOf(" ");
    if (spacePos != -1) {
        text = text.mid(spacePos + 1);
    }
    // 注意：服务端可能使用特定的分类名称，如"书籍教材"、"电子产品"等
    // 如果 UI 中的文本与服务端一致，直接返回；否则需要映射
    return text;
}
