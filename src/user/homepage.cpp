#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QDateTime>
#include <QMessageBox>
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

    // 连接信号槽
    connect(categoryList, &QListWidget::itemClicked, this, &HomePage::onCategoryClicked);
    connect(searchBtn, &QPushButton::clicked, this, &HomePage::onSearchClicked);
    connect(goodsTable, &QTableWidget::cellDoubleClicked, this, &HomePage::onShowGoodsDetail);
}

void HomePage::loadMockData() {
    // 模拟数据加载实现...
    goodsTable->setRowCount(0);

    QStringList goodsNames = {
        "二手iPhone 12 128GB", "大学物理教材", "篮球鞋 Nike Air",
        "笔记本电脑戴尔", "英语四级词汇书", "小米手环6", "吉他雅马哈", "考研数学复习全书"
    };

    QStringList prices = {"2500", "35", "280", "3200", "15", "150", "800", "40"};

    for (int i = 0; i < goodsNames.size(); i++) {
        int row = goodsTable->rowCount();
        goodsTable->insertRow(row);
        goodsTable->setRowHeight(row, 90);

        // 商品图片
        QLabel *imageLabel = new QLabel();
        imageLabel->setPixmap(QPixmap(":/icons/img/buy.png").scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        imageLabel->setAlignment(Qt::AlignCenter);
        goodsTable->setCellWidget(row, 0, imageLabel);

        // 商品名称
        QTableWidgetItem *nameItem = new QTableWidgetItem(goodsNames[i]);
        nameItem->setData(Qt::UserRole, i + 1000); // 模拟商品ID
        goodsTable->setItem(row, 1, nameItem);

        // 价格
        goodsTable->setItem(row, 2, new QTableWidgetItem(QString("¥%1").arg(prices[i])));

        // 发布时间
        goodsTable->setItem(row, 3, new QTableWidgetItem(
                                        QDateTime::currentDateTime().addDays(-i).toString("yyyy-MM-dd hh:mm")));

        // 状态
        QString status = (i % 3 == 0) ? "待售" : (i % 3 == 1) ? "交易中" : "已售出";
        QTableWidgetItem *statusItem = new QTableWidgetItem(status);
        goodsTable->setItem(row, 4, statusItem);
    }
}

void HomePage::onCategoryClicked(QListWidgetItem* item) {
    QString category = item->text();
    welcomeLabel->setText(QString("当前分类: %1").arg(category));
    emit categoryChanged(category);
}

void HomePage::onSearchClicked() {
    QString keyword = searchEdit->text().trimmed();
    if (keyword.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入搜索关键词");
        return;
    }
    emit searchRequested(keyword);
}

void HomePage::onShowGoodsDetail(int row, int column) {
    if (row < 0 || column < 0) return;

    QTableWidgetItem *nameItem = goodsTable->item(row, 1);
    if (!nameItem) return;

    int goodsId = nameItem->data(Qt::UserRole).toInt();
    emit goodsDetailRequested(goodsId);
}
