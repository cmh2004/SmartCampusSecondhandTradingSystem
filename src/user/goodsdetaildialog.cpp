#include <QGridLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QComboBox>
#include <QDateTime>
#include <QFileDialog>
#include <QInputDialog>
#include <QLineEdit>
#include <QNetworkRequest>
#include "..\apiservice.h"
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
        thumbnail->setObjectName(QString("thumbnail_%1").arg(i));
        m_thumbnailLabels.append(thumbnail);
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
    QJsonObject result = ApiService::instance()->getGoodsDetail(goodsId);
    if (result.value("success").toBool()) {
        QJsonObject data = result.value("data").toObject();

        // 基本信息
        goodsTitleLabel->setText(data.value("name").toString());
        priceLabel->setText(QString("¥%1").arg(data.value("price").toDouble()));
        descriptionText->setText(data.value("description").toString());
        categoryLabel->setText(getCategoryName(data.value("category_id").toInt())); // 或从分类表获取名称
        publishTimeLabel->setText(data.value("publish_time").toString());
        conditionLabel->setText(data.value("condition").toString()); // 需要服务端提供，若没有可暂时隐藏或显示默认

        // 卖家信息
        sellerLabel->setText(data.value("seller_name").toString());
        contactLabel->setText("点击联系卖家"); // 或从卖家资料中获取联系方式
        locationLabel->setText("校园内"); // 可从卖家信息中获取

        // 商品图片
        QJsonArray images = data.value("images").toArray();
        if (!images.isEmpty()) {
            // 主图
            QString mainImageUrl = images[0].toObject().value("image_url").toString();
            // 拼接完整 URL
            QString fullUrl = "http://127.0.0.1:8080" + mainImageUrl;
            // 异步加载图片并设置到 goodsImageLabel
            QNetworkAccessManager *nam = new QNetworkAccessManager(this);
            connect(nam, &QNetworkAccessManager::finished, [this, nam](QNetworkReply *reply) {
                if (reply->error() == QNetworkReply::NoError) {
                    QPixmap pixmap;
                    pixmap.loadFromData(reply->readAll());
                    if (!pixmap.isNull()) {
                        goodsImageLabel->setPixmap(pixmap.scaled(380, 380, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    }
                }
                reply->deleteLater();
                nam->deleteLater();
            });
            nam->get(QNetworkRequest(QUrl(fullUrl)));

            // 缩略图
            for (int i = 0; i < qMin(images.size(), m_thumbnailLabels.size()); ++i) {
                QString thumbUrl = "http://127.0.0.1:8080" +images[i].toObject().value("image_url").toString();
                QLabel *thumbLabel = m_thumbnailLabels[i];
                QNetworkAccessManager *thumbNam = new QNetworkAccessManager(this);
                connect(thumbNam, &QNetworkAccessManager::finished, [thumbLabel, thumbNam](QNetworkReply *reply) {
                    if (reply->error() == QNetworkReply::NoError) {
                        QPixmap pixmap;
                        pixmap.loadFromData(reply->readAll());
                        if (!pixmap.isNull()) {
                            thumbLabel->setPixmap(pixmap.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                        }
                    }
                    reply->deleteLater();
                    thumbNam->deleteLater();
                });
                thumbNam->get(QNetworkRequest(QUrl(thumbUrl)));
            }
        }

        bool isFavorited = data.value("is_favorited").toBool();
        qDebug() << "is_favorited value:" << data.value("is_favorited").toBool();
        m_isFavorited=isFavorited;
        if (isFavorited) {
            collectBtn->setText("已收藏");
        } else {
            collectBtn->setText("收藏");
        }

        // AI评估信息（需要额外调用 estimatePrice，或从商品数据中已有，这里假设从 data 中获取）
        if (data.contains("min_price") && data.contains("max_price")) {
            aiPriceRangeLabel->setText(QString("¥%1 - ¥%2")
                                           .arg(data.value("min_price").toDouble())
                                           .arg(data.value("max_price").toDouble()));
        }

        // 其他 AI 评估字段，若服务端返回则填充
        aiConditionLabel->setText(data.value("ai_condition").toString());
        aiBrandLabel->setText(data.value("brand").toString());
        aiRiskLevelLabel->setText(data.value("risk_level").toString());
        aiRecommendationLabel->setText(data.value("recommendation").toString());

        // 添加浏览记录（异步，不需要等待结果）
        ApiService::instance()->addBrowseHistory(goodsId);
    } else {
        QMessageBox::warning(this, "错误", "加载商品详情失败");
        close();
    }
}

void GoodsDetailDialog::loadAIAssessment(int goodsId) {
    // AI评估逻辑
    // 这里可以调用AI服务进行估价
}

void GoodsDetailDialog::onCollectGoods() {
    if (m_isFavorited) {
        // 取消收藏
        QJsonObject result = ApiService::instance()->removeFavorite(goodsId);
        if (result.value("success").toBool()) {
            m_isFavorited = false;
            emit ApiService::instance()->favoriteChanged();
            collectBtn->setText("收藏");
            collectBtn->setEnabled(true);
            QMessageBox::information(this, "提示", "已取消收藏");
        } else {
            QMessageBox::warning(this, "失败", result.value("error").toString());
        }
    } else {
        // 添加收藏
        QJsonObject result = ApiService::instance()->addFavorite(goodsId);
        emit ApiService::instance()->favoriteChanged();
        if (result.value("success").toBool()) {
            m_isFavorited = true;
            collectBtn->setText("已收藏");
            QMessageBox::information(this, "提示", "收藏成功");
        } else {
            QMessageBox::warning(this, "失败", result.value("error").toString());
        }
    }
}

void GoodsDetailDialog::onAIAssessment() {
    // 显示加载提示
    aiPriceRangeLabel->setText("加载中...");
    QString description = descriptionText->toPlainText();
    QJsonObject estimate = ApiService::instance()->estimatePrice(description, "");
    if (estimate.value("success").toBool()) {
        QJsonObject data = estimate.value("data").toObject();
        aiPriceRangeLabel->setText(QString("¥%1 - ¥%2")
                                       .arg(data.value("min_price").toDouble())
                                       .arg(data.value("max_price").toDouble()));
        aiRiskLevelLabel->setText(data.value("risk_level").toString());
        aiConditionLabel->setText(data.value("ai_condition").toString());
        aiBrandLabel->setText(data.value("brand").toString());
        aiRecommendationLabel->setText(data.value("recommendation").toString());
    } else {
        aiPriceRangeLabel->setText("估价失败");
        QMessageBox::warning(this, "AI估价失败", estimate.value("error").toString());
    }
}

void GoodsDetailDialog::onShowRiskAssessment() {
    QMessageBox::warning(this, "风险提醒",
                         "风险分析结果:\n"
                         "1. 卖家信用良好，无历史投诉\n"
                         "2. 价格在合理范围内\n"
                         "3. 建议线下验货后再付款\n"
                         "4. 注意保留聊天记录作为凭证");
}


QString GoodsDetailDialog::getCategoryName(int categoryId) {
    switch (categoryId) {
    case 1: return "书籍教材";
    case 2: return "电子产品";
    case 3: return "服饰鞋包";
    case 4: return "生活用品";
    case 5: return "体育器材";
    case 6: return "学习工具";
    case 7: return "美妆个护";
    default: return "其他";
    }
}
