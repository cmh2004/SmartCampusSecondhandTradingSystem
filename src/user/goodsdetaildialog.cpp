#include <QGridLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QComboBox>
#include <QDateTime>
#include <QFileDialog>
#include <QInputDialog>
#include <QLineEdit>
#include "goodsdetaildialog.h"

GoodsDetailDialog::GoodsDetailDialog(QWidget *parent, int goodsId)
    : QDialog(parent), goodsId(goodsId) {
    setWindowTitle("商品详情");
    setMinimumSize(1200, 800);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    setupUI();
    loadGoodsData(goodsId);
}

void GoodsDetailDialog::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 创建标签页
    detailTabs = new QTabWidget();

    // 标签1: 商品信息
    QWidget *infoTab = new QWidget();
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("border: none;");

    QWidget *scrollContent = new QWidget();
    QVBoxLayout *contentLayout = new QVBoxLayout(scrollContent);
    contentLayout->setContentsMargins(20, 20, 20, 20);
    contentLayout->setSpacing(20);

    // 上部分：图片 + 基本信息（左右布局）
    QWidget *topSection = new QWidget();
    QHBoxLayout *topLayout = new QHBoxLayout(topSection);
    topLayout->setSpacing(20);
    topLayout->setContentsMargins(0, 0, 0, 0);

    // 左侧：图片区域
    QWidget *leftPanel = new QWidget();
    leftPanel->setFixedWidth(400);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(10);

    // 主图片
    goodsImageLabel = new QLabel();
    goodsImageLabel->setFixedSize(380, 380);
    goodsImageLabel->setStyleSheet("border: 2px solid #ddd; border-radius: 8px;");
    goodsImageLabel->setAlignment(Qt::AlignCenter);
    goodsImageLabel->setPixmap(QPixmap(":/icons/img/buy.png").scaled(380, 380, Qt::KeepAspectRatio));
    leftLayout->addWidget(goodsImageLabel);

    // 缩略图
    QWidget *thumbnailContainer = new QWidget();
    QHBoxLayout *thumbnailLayout = new QHBoxLayout(thumbnailContainer);
    thumbnailLayout->setSpacing(8);
    thumbnailLayout->setContentsMargins(0, 0, 0, 0);

    for (int i = 0; i < 3; i++) {
        QLabel *thumbnail = new QLabel();
        thumbnail->setFixedSize(80, 80);
        thumbnail->setStyleSheet("border: 1px solid #ddd; border-radius: 4px;");
        thumbnail->setAlignment(Qt::AlignCenter);
        thumbnail->setCursor(Qt::PointingHandCursor);
        thumbnailLayout->addWidget(thumbnail);
    }
    thumbnailLayout->addStretch();
    leftLayout->addWidget(thumbnailContainer);
    leftLayout->addStretch();

    // 右侧：商品信息区域
    QWidget *rightPanel = new QWidget();
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(16);

    // 商品标题
    goodsTitleLabel = new QLabel("二手iPhone 12 128GB");
    goodsTitleLabel->setStyleSheet("font-size: 22px; font-weight: bold; color: #333;");
    goodsTitleLabel->setWordWrap(true);
    rightLayout->addWidget(goodsTitleLabel);

    // 价格区域
    QWidget *priceWidget = new QWidget();
    QHBoxLayout *priceRowLayout = new QHBoxLayout(priceWidget);
    priceRowLayout->setContentsMargins(0, 0, 0, 0);

    priceLabel = new QLabel("¥2500");
    priceLabel->setStyleSheet("color: #e74c3c; font-size: 28px; font-weight: bold;");
    priceRowLayout->addWidget(priceLabel);
    priceRowLayout->addStretch();

    rightLayout->addWidget(priceWidget);

    // 基本信息卡片
    QGroupBox *basicInfoGroup = new QGroupBox("基本信息");
    basicInfoGroup->setStyleSheet(R"(
        QGroupBox {
            border: 1px solid #ddd;
            border-radius: 8px;
            margin-top: 10px;
            padding-top: 10px;
            background-color: white;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
            font-weight: bold;
            font-size:14px;
            color: #333;
        }
    )");

    QGridLayout *basicLayout = new QGridLayout();
    basicLayout->setVerticalSpacing(12);
    basicLayout->setHorizontalSpacing(16);

    // 第1行
    basicLayout->addWidget(new QLabel("卖家:"), 0, 0);
    sellerLabel = new QLabel("张三同学");
    sellerLabel->setStyleSheet("color: #3498db; font-weight: 500;");
    basicLayout->addWidget(sellerLabel, 0, 1);

    basicLayout->addWidget(new QLabel("商品状态:"), 0, 2);
    conditionLabel = new QLabel("9成新");
    QLabel *conditionBadge = new QLabel("在售");
    conditionBadge->setStyleSheet("padding: 2px 8px; border-radius: 10px; font-size: 12px; background-color: #2ecc71; color: white;");
    QHBoxLayout *conditionLayout = new QHBoxLayout();
    conditionLayout->addWidget(conditionLabel);
    conditionLayout->addWidget(conditionBadge);
    conditionLayout->setSpacing(8);
    basicLayout->addLayout(conditionLayout, 0, 3);

    // 第2行
    basicLayout->addWidget(new QLabel("联系方式:"), 1, 0);
    contactLabel = new QLabel("138****1234");
    contactLabel->setStyleSheet("color: #2c3e50;");
    basicLayout->addWidget(contactLabel, 1, 1);

    basicLayout->addWidget(new QLabel("商品分类:"), 1, 2);
    categoryLabel = new QLabel("电子产品");
    basicLayout->addWidget(categoryLabel, 1, 3);

    // 第3行
    basicLayout->addWidget(new QLabel("位置:"), 2, 0);
    locationLabel = new QLabel("学生宿舍10号楼");
    basicLayout->addWidget(locationLabel, 2, 1);

    basicLayout->addWidget(new QLabel("发布时间:"), 2, 2);
    publishTimeLabel = new QLabel(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm"));
    basicLayout->addWidget(publishTimeLabel, 2, 3);

    basicInfoGroup->setLayout(basicLayout);
    rightLayout->addWidget(basicInfoGroup);

    // 按钮区域
    QWidget *buttonWidget = new QWidget();
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);
    buttonLayout->setSpacing(10);
    buttonLayout->setContentsMargins(0, 10, 0, 0);

    contactBtn = new QPushButton("联系卖家");
    contactBtn->setObjectName("secondaryBtn");

    collectBtn = new QPushButton("收藏");
    collectBtn->setObjectName("secondaryBtn");

    riskBtn = new QPushButton("风险提醒");
    riskBtn->setObjectName("warningBtn");

    reportBtn = new QPushButton("举报商品");
    reportBtn->setObjectName("warningBtn");

    buyBtn = new QPushButton("立即购买");
    buyBtn->setObjectName("primaryBtn");
    buyBtn->setFixedHeight(44);

    buttonLayout->addWidget(collectBtn);
    buttonLayout->addWidget(contactBtn);
    buttonLayout->addWidget(riskBtn);
    buttonLayout->addWidget(reportBtn);
    buttonLayout->addStretch();
    buttonLayout->addWidget(buyBtn);

    rightLayout->addWidget(buttonWidget);
    rightLayout->addStretch();

    // 将左右两部分添加到顶部区域
    topLayout->addWidget(leftPanel);
    topLayout->addWidget(rightPanel);

    contentLayout->addWidget(topSection);

    // 下部分：商品详细描述
    QWidget *descSection = new QWidget();
    QVBoxLayout *descSectionLayout = new QVBoxLayout(descSection);
    descSectionLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *descTitle = new QLabel("商品描述");
    descTitle->setStyleSheet("font-size: 18px; font-weight: bold; color: #333; padding-bottom: 10px;");
    descSectionLayout->addWidget(descTitle);

    QGroupBox *descGroup = new QGroupBox("");
    descGroup->setStyleSheet("QGroupBox { border: 1px solid #ddd; border-radius: 8px; padding: 15px; }");
    QVBoxLayout *descLayout = new QVBoxLayout(descGroup);

    descriptionText = new QTextEdit();
    descriptionText->setReadOnly(true);
    descriptionText->setMaximumHeight(300);
    descriptionText->setText("自用iPhone 12，使用一年，保护完好，无划痕。包含原装充电器、数据线。屏幕无划痕，电池健康度85%。");
    descLayout->addWidget(descriptionText);

    descSectionLayout->addWidget(descGroup);
    contentLayout->addWidget(descSection);

    // 设置滚动区域内容
    scrollArea->setWidget(scrollContent);
    infoTab->setLayout(new QVBoxLayout());
    infoTab->layout()->addWidget(scrollArea);
    detailTabs->addTab(infoTab, "商品详情");

    // 标签2: AI智能评估
    QWidget *aiTab = new QWidget();
    QVBoxLayout *aiLayout = new QVBoxLayout(aiTab);

    QGroupBox *aiAssessmentGroup = new QGroupBox("AI智能评估");
    QGridLayout *aiGrid = new QGridLayout();

    aiGrid->addWidget(new QLabel("估价范围:"), 0, 0);
    aiPriceRangeLabel = new QLabel("¥2300 - ¥2700");
    aiPriceRangeLabel->setStyleSheet("color: #e67e22; font-weight: bold;");
    aiGrid->addWidget(aiPriceRangeLabel, 0, 1);

    aiGrid->addWidget(new QLabel("成色评估:"), 1, 0);
    aiConditionLabel = new QLabel("9成新");
    aiGrid->addWidget(aiConditionLabel, 1, 1);

    aiGrid->addWidget(new QLabel("品牌识别:"), 2, 0);
    aiBrandLabel = new QLabel("Apple iPhone 12 128GB");
    aiGrid->addWidget(aiBrandLabel, 2, 1);

    aiGrid->addWidget(new QLabel("风险等级:"), 3, 0);
    aiRiskLevelLabel = new QLabel("低风险");
    aiRiskLevelLabel->setStyleSheet("color: #27ae60; font-weight: bold;");
    aiGrid->addWidget(aiRiskLevelLabel, 3, 1);

    aiGrid->addWidget(new QLabel("购买建议:"), 4, 0);
    aiRecommendationLabel = new QLabel("价格合理，建议购买");
    aiGrid->addWidget(aiRecommendationLabel, 4, 1);

    aiAssessmentGroup->setLayout(aiGrid);
    aiLayout->addWidget(aiAssessmentGroup);

    aiAssessmentBtn = new QPushButton("重新评估");
    aiAssessmentBtn->setObjectName("primaryBtn");
    aiLayout->addWidget(aiAssessmentBtn);

    detailTabs->addTab(aiTab, "AI评估");

    mainLayout->addWidget(detailTabs);

    // 连接信号槽
    connect(buyBtn, &QPushButton::clicked, [this]() {
        emit buyNowRequested(goodsId);
        accept();  // 关闭对话框
    });

    connect(contactBtn, &QPushButton::clicked, [this]() {
        emit contactSellerRequested(goodsId, sellerLabel->text());
        showMinimized();
    });

    connect(collectBtn, &QPushButton::clicked, this, &GoodsDetailDialog::onCollectGoods);
    connect(riskBtn, &QPushButton::clicked, this, &GoodsDetailDialog::onShowRiskAssessment);
    connect(aiAssessmentBtn, &QPushButton::clicked, this, &GoodsDetailDialog::onAIAssessment);
    connect(reportBtn, &QPushButton::clicked, [this]() {
        emit reportGoodsRequested(goodsId);
    });

    // 样式表
    setStyleSheet(R"(
        QGroupBox {
            font-weight: bold;
            border: 1px solid #ddd;
            border-radius: 6px;
            margin-top: 10px;
            padding-top: 10px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
        }
        #primaryBtn {
            background-color: #3498db;
            color: white;
            border-radius: 4px;
            padding: 8px 18px;
            font-size: 14px;
            font-weight: bold;
        }
        #primaryBtn:hover {
            background-color: #2980b9;
        }
        #secondaryBtn {
            background-color: #ecf0f1;
            color: #34495e;
            border-radius: 4px;
            padding: 8px 16px;
            border:1px solid #d5dbdb;
            font-size: 14px;
        }
        #secondaryBtn:hover {
            background-color: #ccd1d1;
        }
        #warningBtn {
            background-color: #e74c3c;
            color: white;
            border-radius: 4px;
            padding: 8px 16px;
            font-size: 14px;
        }
        #warningBtn:hover {
            background-color: #c0392b;
        }
        QTextEdit {
            border: 1px solid #ddd;
            border-radius: 4px;
            padding: 6px;
            background-color: white;
            font-size: 14px;
            line-height: 1.6;
        }
        QTextEdit:focus {
            border-color: #3498db;
            outline: none;
        }
        QTabWidget::pane {
            border: 1px solid #ddd;
            background-color: white;
            border-radius: 0 0 12px 12px;
        }
        QTabBar::tab {
            padding: 8px 16px;
            background-color: #f8f9fa;
            border: 1px solid #ddd;
            border-bottom: none;
            border-radius: 4px 4px 0 0;
            font-size: 14px;
        }
        QTabBar::tab:selected {
            background-color: white;
            border-bottom: 2px solid #3498db;
            font-weight: bold;
        }
        QTabBar::tab:hover:!selected {
            background-color: #e9ecef;
        }
        QScrollArea {
            border: none;
            background-color: white;
        }
        QLabel {
            font-size: 14px;
        }
        QLabel[title] {
            font-size: 13px;
            color: #666;
        }
    )");
}

void GoodsDetailDialog::loadGoodsData(int goodsId) {
    // 这里应该从数据库加载商品数据
    // 目前使用模拟数据
    this->goodsId = goodsId;

    // 根据不同的商品ID显示不同的数据
    switch(goodsId) {
    case 0:
        goodsTitleLabel->setText("二手iPhone 12 128GB");
        priceLabel->setText("¥2500");
        break;
    case 1:
        goodsTitleLabel->setText("大学物理教材");
        priceLabel->setText("¥35");
        break;
    case 2:
        goodsTitleLabel->setText("篮球鞋 Nike Air");
        priceLabel->setText("¥280");
        break;
    default:
        goodsTitleLabel->setText("商品名称");
        priceLabel->setText("¥0");
    }
}

void GoodsDetailDialog::loadAIAssessment(int goodsId) {
    // AI评估逻辑
    // 这里可以调用AI服务进行估价
}

void GoodsDetailDialog::onCollectGoods() {
    QMessageBox::information(this, "收藏", "商品已添加到收藏夹");
}

void GoodsDetailDialog::onAIAssessment() {
    QMessageBox::information(this, "AI评估", "正在重新进行AI评估...");
    // 模拟AI评估结果
    aiPriceRangeLabel->setText("¥2400 - ¥2600");
    aiRiskLevelLabel->setText("低风险");
    aiRecommendationLabel->setText("价格适中，建议购买");
}

void GoodsDetailDialog::onShowRiskAssessment() {
    QMessageBox::warning(this, "风险提醒",
                         "风险分析结果:\n"
                         "1. 卖家信用良好，无历史投诉\n"
                         "2. 价格在合理范围内\n"
                         "3. 建议线下验货后再付款\n"
                         "4. 注意保留聊天记录作为凭证");
}
