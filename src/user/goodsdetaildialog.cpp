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
    setMinimumSize(800, 600);
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
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollContent);

    // 商品图片
    QWidget *imageSection = new QWidget();
    QHBoxLayout *imageLayout = new QHBoxLayout(imageSection);
    goodsImageLabel = new QLabel();
    goodsImageLabel->setFixedSize(150, 150);
    goodsImageLabel->setStyleSheet("border: 2px solid #ddd; border-radius: 8px;");
    goodsImageLabel->setAlignment(Qt::AlignCenter);
    goodsImageLabel->setPixmap(QPixmap(":/icons/img/buy.png").scaled(150, 150, Qt::KeepAspectRatio));
    imageLayout->addWidget(goodsImageLabel);
    scrollLayout->addWidget(imageSection);

    // 基本信息卡片
    QGroupBox *basicInfoGroup = new QGroupBox("基本信息");
    QGridLayout *basicLayout = new QGridLayout();

    goodsTitleLabel = new QLabel("二手iPhone 12 128GB");
    goodsTitleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #333;");
    basicLayout->addWidget(goodsTitleLabel, 0, 0, 1, 3);

    basicLayout->addWidget(new QLabel("价格:"), 1, 0);
    priceLabel = new QLabel("¥2500");
    priceLabel->setStyleSheet("color: #e74c3c; font-size: 24px; font-weight: bold;");
    basicLayout->addWidget(priceLabel, 1, 1);

    basicLayout->addWidget(new QLabel("原价:"), 2, 0);
    originalPriceLabel = new QLabel("¥5999");
    originalPriceLabel->setStyleSheet("color: #999; text-decoration: line-through;");
    basicLayout->addWidget(originalPriceLabel, 2, 1);

    basicLayout->addWidget(new QLabel("卖家:"), 3, 0);
    sellerLabel = new QLabel("张三同学");
    sellerLabel->setStyleSheet("color: #3498db;");
    basicLayout->addWidget(sellerLabel, 3, 1);

    basicLayout->addWidget(new QLabel("联系方式:"), 4, 0);
    contactLabel = new QLabel("138****1234");
    contactLabel->setStyleSheet("color: #2c3e50;");
    basicLayout->addWidget(contactLabel, 4, 1);

    basicLayout->addWidget(new QLabel("位置:"), 5, 0);
    locationLabel = new QLabel("学生宿舍10号楼");
    basicLayout->addWidget(locationLabel, 5, 1);

    basicLayout->addWidget(new QLabel("发布时间:"), 6, 0);
    publishTimeLabel = new QLabel(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm"));
    basicLayout->addWidget(publishTimeLabel, 6, 1);

    basicLayout->addWidget(new QLabel("商品状态:"), 7, 0);
    conditionLabel = new QLabel("9成新");
    QLabel *conditionBadge = new QLabel("待售");
    conditionBadge->setStyleSheet("padding: 2px 8px; border-radius: 10px; font-size: 12px; background-color: #2ecc71; color: white;");
    basicLayout->addWidget(conditionBadge, 7, 1);

    basicLayout->addWidget(new QLabel("商品分类:"), 8, 0);
    categoryLabel = new QLabel("电子产品");
    basicLayout->addWidget(categoryLabel, 8, 1);

    basicInfoGroup->setLayout(basicLayout);
    scrollLayout->addWidget(basicInfoGroup);

    // 商品描述
    QGroupBox *descGroup = new QGroupBox("商品描述");
    QVBoxLayout *descLayout = new QVBoxLayout();
    descriptionText = new QTextEdit();
    descriptionText->setReadOnly(true);
    descriptionText->setMaximumHeight(200);
    descriptionText->setText("自用iPhone 12，使用一年，保护完好，无划痕。包含原装充电器、数据线。屏幕无划痕，电池健康度85%。");
    descLayout->addWidget(descriptionText);
    descGroup->setLayout(descLayout);
    scrollLayout->addWidget(descGroup);

    scrollArea->setWidget(scrollContent);
    infoTab->setLayout(new QVBoxLayout());
    infoTab->layout()->addWidget(scrollArea);
    detailTabs->addTab(infoTab, "商品信息");

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

    // 底部按钮区域
    QWidget *buttonWidget = new QWidget();
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);

    buyBtn = new QPushButton("立即购买");
    buyBtn->setObjectName("primaryBtn");
    buyBtn->setFixedHeight(45);

    contactBtn = new QPushButton("联系卖家");
    contactBtn->setObjectName("secondaryBtn");

    collectBtn = new QPushButton("收藏");
    collectBtn->setObjectName("secondaryBtn");

    offerBtn = new QPushButton("我要议价");
    offerBtn->setObjectName("secondaryBtn");

    riskBtn = new QPushButton("风险提醒");
    riskBtn->setObjectName("warningBtn");

    reportBtn = new QPushButton("举报商品");
    reportBtn->setObjectName("secondaryBtn");

    buttonLayout->addWidget(collectBtn);
    buttonLayout->addWidget(contactBtn);
    buttonLayout->addWidget(offerBtn);
    buttonLayout->addWidget(riskBtn);
    buttonLayout->addWidget(reportBtn);
    buttonLayout->addStretch();
    buttonLayout->addWidget(buyBtn);

    mainLayout->addWidget(buttonWidget);

    // 连接信号槽
    connect(buyBtn, &QPushButton::clicked, [this]() {
        emit buyNowRequested(goodsId);
        accept();  // 关闭对话框
    });

    connect(contactBtn, &QPushButton::clicked, [this]() {
        emit contactSellerRequested(goodsId);
    });

    connect(offerBtn, &QPushButton::clicked, [this]() {
        emit makeOfferRequested(goodsId);
    });

    connect(collectBtn, &QPushButton::clicked, this, &GoodsDetailDialog::onCollectGoods);
    connect(riskBtn, &QPushButton::clicked, this, &GoodsDetailDialog::onShowRiskAssessment);
    connect(aiAssessmentBtn, &QPushButton::clicked, this, &GoodsDetailDialog::onAIAssessment);
    connect(reportBtn, &QPushButton::clicked, [this]() {
        emit reportGoodsRequested(goodsId);
    });

    // 样式
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
            padding: 10px 20px;
        }
        #primaryBtn:hover {
            background-color: #2980b9;
        }
        #secondaryBtn {
            background-color: #ecf0f1;
            color: #34495e;
            border-radius: 4px;
            padding: 8px 16px;
        }
        #secondaryBtn:hover {
            background-color: #d5dbdb;
        }
        #warningBtn {
            background-color: #e74c3c;
            color: white;
            border-radius: 4px;
            padding: 8px 16px;
        }
        #warningBtn:hover {
            background-color: #c0392b;
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

void GoodsDetailDialog::onContactSeller() {
    QMessageBox::information(this, "联系卖家", "已打开与卖家的聊天窗口\n卖家联系方式: 138****1234");
}

void GoodsDetailDialog::onMakeOffer() {
    bool ok;
    QString price = QInputDialog::getText(this, "议价", "请输入您的出价:",
                                          QLineEdit::Normal, priceLabel->text(), &ok);
    if (ok && !price.isEmpty()) {
        QMessageBox::information(this, "议价成功", QString("已向卖家发送出价: %1").arg(price));
    }
}

void GoodsDetailDialog::onCollectGoods() {
    QMessageBox::information(this, "收藏", "商品已添加到收藏夹");
}

void GoodsDetailDialog::onReportGoods() {
    QMessageBox::StandardButton reply = QMessageBox::question(this, "举报商品",
                                                              "您确定要举报此商品吗？\n请选择举报原因:",
                                                              QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        QStringList reasons = {"虚假信息", "价格欺诈", "商品侵权", "其他"};
        bool ok;
        QString reason = QInputDialog::getItem(this, "选择举报原因", "原因:",
                                               reasons, 0, false, &ok);
        if (ok && !reason.isEmpty()) {
            QMessageBox::information(this, "举报成功", QString("已提交举报: %1\n管理员将在24小时内处理").arg(reason));
        }
    }
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
