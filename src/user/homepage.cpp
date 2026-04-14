#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QDateTime>
#include <QMessageBox>
#include <QScrollArea>
#include <QMouseEvent>
#include <QJsonArray>
#include <QJsonObject>
#include <QTextEdit>
#include "HomePage.h"
#include "..\apiservice.h"

HomePage::HomePage(QWidget *parent) : QWidget(parent),m_currentPage(1),m_totalPages(1) {
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

    QPushButton *aiSearchBtn = new QPushButton("AI推荐");
    aiSearchBtn->setObjectName("primaryBtn");
    aiSearchBtn->setFixedWidth(90);
    aiSearchBtn->setMinimumHeight(38);
    searchLayout->addWidget(aiSearchBtn);

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
    categoryList->setCurrentRow(0);
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
    connect(aiSearchBtn, &QPushButton::clicked, this, &HomePage::onAISearchClicked);

    QWidget *paginationWidget = new QWidget();
    QHBoxLayout *paginationLayout = new QHBoxLayout(paginationWidget);
    paginationLayout->setContentsMargins(0, 10, 0, 0);
    paginationLayout->setAlignment(Qt::AlignCenter);

    prevPageBtn = new QPushButton("上一页");
    prevPageBtn->setObjectName("primaryBtn");
    prevPageBtn->setFixedSize(80, 32);
    nextPageBtn = new QPushButton("下一页");
    nextPageBtn->setObjectName("primaryBtn");
    nextPageBtn->setFixedSize(80, 32);
    pageInfoLabel = new QLabel("第 1 页");
    pageInfoLabel->setStyleSheet("font-size: 13px; color: #475569; margin: 0 15px;");

    paginationLayout->addWidget(prevPageBtn);
    paginationLayout->addWidget(pageInfoLabel);
    paginationLayout->addWidget(nextPageBtn);
    mainLayout->addWidget(paginationWidget);

    connect(prevPageBtn, &QPushButton::clicked, this, &HomePage::goToPrevPage);
    connect(nextPageBtn, &QPushButton::clicked, this, &HomePage::goToNextPage);
}

void HomePage::loadGoodsFromServer(const QString &keyword, const QString &category,
                                   double minPrice, double maxPrice, const QString &sortBy,
                                   int page, int pageSize) {
    QJsonArray goodsArray = ApiService::instance()->searchGoods(keyword, category, minPrice, maxPrice, sortBy, page, pageSize);

    // 1. 清空现有商品网格
    clearGoodsGrid();

    // 判断是否还有更多（如果返回数量小于 pageSize，说明是最后一页）
    bool hasMore = (goodsArray.size() == pageSize);
    nextPageBtn->setEnabled(hasMore);
    prevPageBtn->setEnabled(page > 1);

    pageInfoLabel->setText(QString("第 %1 页").arg(page));
    m_currentPage = page;

    // 2. 添加新商品卡片到网格
    int columns = 4;  // 固定列数，可根据窗口宽度动态调整
    for (int i = 0; i < goodsArray.size(); ++i) {
        QJsonObject goods = goodsArray[i].toObject();
        int goodsId = goods.value("id").toInt();
        QString name = goods.value("name").toString();
        double price = goods.value("price").toDouble();
        int categoryId = goods.value("category_id").toInt();
        QString status = goods.value("status").toString(); // 待售等

        // 将状态码转换为显示文本
        QString statusText;
        if (status == "1") statusText = "在售";
        else if (status == "0") statusText = "待审核";
        else if (status == "2") statusText = "交易中";
        else if (status == "3") statusText = "已售出";
        else statusText = "未知";
        QString categoryName = QString::number(categoryId);

        QString imageUrl = goods.value("image_url").toString();
        // 创建商品卡片
        QWidget *card = createGoodsCard(goodsId, name, QString::number(price), categoryName, statusText, imageUrl);

        // 计算行列位置
        int row = i / columns;
        int col = i % columns;
        goodsGridLayout->addWidget(card, row, col);
    }
}

QWidget* HomePage::createGoodsCard(int goodsId, const QString& name,
                                   const QString& price, const QString& category,
                                   const QString& status, const QString& imageUrl) {
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

    if (!imageUrl.isEmpty()) {
        QString fullUrl = "http://127.0.0.1:8080" + imageUrl;
        // 异步加载图片
        QNetworkAccessManager *nam = new QNetworkAccessManager();
        connect(nam, &QNetworkAccessManager::finished, [imageLabel, nam](QNetworkReply *reply) {
            if (reply->error() == QNetworkReply::NoError) {
                QPixmap pixmap;
                pixmap.loadFromData(reply->readAll());
                if (!pixmap.isNull()) {
                    imageLabel->setPixmap(pixmap.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                }
            }
            reply->deleteLater();
            nam->deleteLater();
        });
        nam->get(QNetworkRequest(QUrl(fullUrl)));
    } else {
        // 默认图片
        imageLabel->setPixmap(QPixmap(":/icons/img/buy.png").scaled(100, 100, Qt::KeepAspectRatio));
    }
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

    loadGoodsFromServer(searchEdit->text().trimmed(), category, 0, 0, getSortByValue(), 1, 20);
}

void HomePage::onSearchClicked() {
    QString keyword = searchEdit->text().trimmed();

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
    return text;
}

void HomePage::onAISearchClicked()
{
    // 弹出对话框让用户输入需求
    QDialog dialog(this);
    dialog.setWindowTitle("AI 智能推荐");
    dialog.setFixedSize(500, 300);
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    QLabel *label = new QLabel("请描述您的需求：");
    QTextEdit *requirementEdit = new QTextEdit();
    requirementEdit->setPlaceholderText("例如：我想买一台1500元左右的二手笔记本电脑，9成新以上，联想或华硕品牌");
    QPushButton *searchBtn = new QPushButton("开始推荐");
    QPushButton *cancelBtn = new QPushButton("取消");
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(searchBtn);
    layout->addWidget(label);
    layout->addWidget(requirementEdit);
    layout->addLayout(btnLayout);

    connect(searchBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) return;

    QString requirement = requirementEdit->toPlainText().trimmed();
    if (requirement.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入需求描述");
        return;
    }

    // 调用 API
    QJsonObject request;
    request["requirement"] = requirement;
    QJsonObject response = HttpClient::instance()->syncRequest("/api/ai/search", request, "POST", 30000);

    if (!response.value("success").toBool()) {
        QMessageBox::warning(this, "AI 推荐失败", response.value("error").toString());
        return;
    }

    QJsonObject data = response.value("data").toObject();
    QJsonArray goodsList = data.value("goods_list").toArray();
    bool aiUsed = data.value("ai_used").toBool();

    // 清空当前商品网格并显示推荐结果
    clearGoodsGrid();
    if (goodsList.isEmpty()) {
        QMessageBox::information(this, "没有找到商品", "根据您的需求没有匹配的商品，请尝试更宽松的条件。");
        return;
    }

    int columns = 4;
    for (int i = 0; i < goodsList.size(); ++i) {
        QJsonObject goods = goodsList[i].toObject();
        int goodsId = goods.value("id").toInt();
        QString name = goods.value("name").toString();
        double price = goods.value("price").toDouble();
        QString imageUrl = goods.value("image_url").toString();
        // 状态字段（假设商品状态为1表示在售）
        QString status = "在售";
        QWidget *card = createGoodsCard(goodsId, name, QString::number(price), "", status, imageUrl);
        int row = i / columns;
        int col = i % columns;
        goodsGridLayout->addWidget(card, row, col);
    }
}

void HomePage::goToPrevPage() {
    if (m_currentPage <= 1) return;
    // 重新加载上一页，保持其他筛选条件不变
    loadGoodsFromServer(searchEdit->text().trimmed(),
                        getCurrentCategory(),
                        0, 0,
                        getSortByValue(),
                        m_currentPage - 1,
                        20);
}

void HomePage::goToNextPage() {
    // 下一页，页码加1
    loadGoodsFromServer(searchEdit->text().trimmed(),
                        getCurrentCategory(),
                        0, 0,
                        getSortByValue(),
                        m_currentPage + 1,
                        20);
}
