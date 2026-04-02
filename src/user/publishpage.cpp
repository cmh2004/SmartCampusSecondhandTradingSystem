#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>
#include <QJsonObject>
#include "..\apiservice.h"
#include "PublishPage.h"

PublishPage::PublishPage(QWidget *parent) : QWidget(parent) {
    setupUI();
}

void PublishPage::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    QWidget *titleWidget = new QWidget(); // 标题容器，方便统一控制
    QHBoxLayout *titleLayout = new QHBoxLayout(titleWidget);
    titleLayout->setContentsMargins(0, 0, 0, 0); // 底部间距，与表单拉开距离
    titleLayout->setAlignment(Qt::AlignCenter); // 标题居中

    QLabel *titleLabel = new QLabel("发布新商品");
    // 基础字体样式
    QFont titleFont;
    titleFont.setFamily("Microsoft YaHei"); // 适配中文的字体
    titleFont.setPointSize(16);
    titleFont.setWeight(QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet(R"(
        QLabel {
            color: #2c3e50;
            letter-spacing: 2px; /* 字间距，更舒展 */
            padding: 4px 0; /* 上下内边距，增加点击/视觉区域 */
        }
    )");

    // 添加文字阴影，提升层次感
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setColor(QColor(0, 0, 0, 20)); // 半透明阴影，不刺眼
    shadowEffect->setOffset(0, 2); // 阴影偏移（x,y）
    shadowEffect->setBlurRadius(5); // 阴影模糊度
    titleLabel->setGraphicsEffect(shadowEffect);

    // 底部装饰线（单独的Label实现）
    QLabel *titleLine = new QLabel();
    titleLine->setFixedSize(120, 3); // 装饰线尺寸
    titleLine->setStyleSheet("background-color: #3498db; border-radius: 1.5px;"); // 与提交按钮同色系，视觉统一

    // 标题布局：标题 + 底部装饰线（垂直布局）
    QVBoxLayout *titleContentLayout = new QVBoxLayout();
    titleContentLayout->setSpacing(4); // 标题与装饰线的间距
    titleContentLayout->setAlignment(Qt::AlignCenter);
    titleContentLayout->addWidget(titleLabel);
    titleContentLayout->addWidget(titleLine, 0, Qt::AlignCenter); // 装饰线居中

    titleLayout->addLayout(titleContentLayout);

    // 表单容器
    QWidget *formContainer = new QWidget();
    QVBoxLayout *formLayout = new QVBoxLayout(formContainer);
    formLayout->setSpacing(15);

    // 商品名称
    QWidget *nameRow = new QWidget();
    QHBoxLayout *nameLayout = new QHBoxLayout(nameRow);
    nameLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *nameLabel = new QLabel("商品名称:");
    nameLabel->setFixedWidth(70);
    goodsNameEdit = new QLineEdit();
    goodsNameEdit->setPlaceholderText("请输入商品名称（最多50字）");

    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(goodsNameEdit, 1);

    // 商品分类
    QWidget *categoryRow = new QWidget();
    QHBoxLayout *categoryLayout = new QHBoxLayout(categoryRow);
    categoryLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *categoryLabel = new QLabel("商品分类:");
    categoryLabel->setFixedWidth(70);
    goodsCategoryCombo = new QComboBox();
    goodsCategoryCombo->addItems({"书籍教材", "电子产品", "服饰鞋包", "生活用品",
                                  "体育器材", "学习工具", "美妆个护", "其他"});

    categoryLayout->addWidget(categoryLabel);
    categoryLayout->addWidget(goodsCategoryCombo, 1);

    // 商品价格
    QWidget *priceRow = new QWidget();
    QHBoxLayout *priceLayout = new QHBoxLayout(priceRow);
    priceLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *priceLabel = new QLabel("期望价格:");
    priceLabel->setFixedWidth(70);
    goodsPriceEdit = new QLineEdit();
    goodsPriceEdit->setPlaceholderText("单位：元");

    QPushButton *aiPriceBtn = new QPushButton("AI估价");
    aiPriceBtn->setObjectName("secondaryBtn");

    priceLayout->addWidget(priceLabel);
    priceLayout->addWidget(goodsPriceEdit, 1);
    priceLayout->addWidget(aiPriceBtn);

    // 商品描述
    QWidget *descRow = new QWidget();
    QVBoxLayout *descLayout = new QVBoxLayout(descRow);
    descLayout->setContentsMargins(0, 0, 0, 0);
    descLayout->setSpacing(12);

    QLabel *descLabel = new QLabel("商品描述:");
    goodsDescEdit = new QTextEdit();
    goodsDescEdit->setPlaceholderText("请详细描述商品信息、使用状况、包含附件等");
    goodsDescEdit->setMaximumHeight(150);

    descLayout->addWidget(descLabel);
    descLayout->addWidget(goodsDescEdit);

    // 图片上传
    QWidget *imageRow = new QWidget();
    QVBoxLayout *imageLayout = new QVBoxLayout(imageRow);
    imageLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *imageLabel = new QLabel("商品图片:");
    uploadImageBtn = new QPushButton("+ 上传图片");
    uploadImageBtn->setObjectName("secondaryBtn");
    uploadImageBtn->setFixedSize(150, 150);

    imagePreview = new QLabel();
    imagePreview->setFixedSize(150, 150);
    imagePreview->setStyleSheet("border: 2px dashed #ccc; border-radius: 8px;");
    imagePreview->setAlignment(Qt::AlignCenter);
    imagePreview->setText("暂无图片");

    connect(uploadImageBtn, &QPushButton::clicked, this, &PublishPage::onUploadImage);

    QHBoxLayout *imageBtnLayout = new QHBoxLayout();
    imageBtnLayout->addWidget(uploadImageBtn);
    imageBtnLayout->addWidget(imagePreview);
    imageBtnLayout->addStretch();

    imageLayout->addWidget(imageLabel);
    imageLayout->addLayout(imageBtnLayout);

    // 提交按钮
    QPushButton *submitBtn = new QPushButton("发布商品");
    submitBtn->setObjectName("primaryBtn");
    submitBtn->setFixedHeight(45);
    connect(submitBtn, &QPushButton::clicked, this, &PublishPage::onPublishGoods);

    // 添加到表单
    formLayout->addWidget(nameRow);
    formLayout->addWidget(categoryRow);
    formLayout->addWidget(priceRow);
    formLayout->addWidget(descRow);
    formLayout->addWidget(imageRow);
    formLayout->addWidget(submitBtn);

    // 添加到主布局
    mainLayout->addWidget(titleWidget);
    mainLayout->addWidget(formContainer);
    mainLayout->addStretch();

    // 设置样式
    this->setStyleSheet(R"(
        QLineEdit, QTextEdit, QComboBox {
            border: 1px solid #ddd;
            border-radius: 4px;
            padding: 6px;
            background-color: white;
        }
        QLineEdit:focus, QTextEdit:focus, QComboBox:focus {
            border-color: #3498db;
            outline: none;
        }
        QPushButton#primaryBtn {
            background-color: #3498db;
            color: white;
            font-weight: bold;
            padding: 10px 20px;
            border-radius: 4px;
            border: none;
        }
        QPushButton#primaryBtn:hover {
            background-color: #2980b9;
        }
        QPushButton#secondaryBtn {
            background-color: #7f8c8d;
            color: white;
            font-weight: bold;
            padding: 8px 16px;
            border-radius: 4px;
            border: none;
        }
    )");

    connect(aiPriceBtn, &QPushButton::clicked, this, &PublishPage::onAIPriceEstimate);
}

void PublishPage::onPublishGoods() {
    QString name = goodsNameEdit->text().trimmed();
    QString price = goodsPriceEdit->text().trimmed();
    QString desc = goodsDescEdit->toPlainText().trimmed();
    QString category = goodsCategoryCombo->currentText();

    if (name.isEmpty() || price.isEmpty()) {
        QMessageBox::warning(this, "提示", "请填写商品名称和价格");
        return;
    }

    // 验证价格是否为数字
    bool ok;
    double priceValue = price.toDouble(&ok);
    if (!ok || priceValue <= 0) {
        QMessageBox::warning(this, "提示", "请输入有效的价格");
        return;
    }

    // 收集图片路径（如果有）
    QStringList imagePaths;
    // 如果 imagePreview 有图片，需要获取其路径，这里简化

    QJsonObject goodsData;
    goodsData["name"] = name;
    goodsData["price"] = priceValue;
    goodsData["description"] = desc;
    goodsData["category"] = category;  // 服务端可能需要 category_id，需要映射

    QJsonObject result = ApiService::instance()->publishGoods(goodsData, imagePaths);
    if (result.value("success").toBool()) {
        QMessageBox::information(this, "成功", "商品发布成功，等待审核");
        // 清空表单
    } else {
        QMessageBox::warning(this, "失败", result.value("error").toString());
    }
}

void PublishPage::onUploadImage() {
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "选择商品图片",
        "",
        "Images (*.png *.jpg *.jpeg *.bmp *.gif)"
        );

    if (!fileName.isEmpty()) {
        QPixmap pixmap(fileName);
        if (!pixmap.isNull()) {
            imagePreview->setPixmap(
                pixmap.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation)
                );
        } else {
            QMessageBox::warning(this, "错误", "无法加载图片文件");
        }
    }
}

void PublishPage::onAIPriceEstimate() {
    QString description = goodsDescEdit->toPlainText().trimmed();
    if (description.isEmpty()) {
        QMessageBox::warning(this, "提示", "请先填写商品描述");
        return;
    }

    // 如果有上传的图片，也可以传给 AI（可选）
    QString imagePath;
    QPixmap pix = imagePreview->pixmap();
    if (&pix && !pix.isNull())  {
        // 这里需要获取实际图片文件路径，但 `imagePreview` 只存了 pixmap，需要记录原始路径
        // 简化：可以不上传图片，只根据描述估价
    }

    QJsonObject result = ApiService::instance()->estimatePrice(description, imagePath);
    if (result.value("success").toBool()) {
        QJsonObject data = result.value("data").toObject();
        double minPrice = data.value("min_price").toDouble();
        double maxPrice = data.value("max_price").toDouble();
        double confidence = data.value("confidence").toDouble();
        QString message = QString("AI 估价范围：¥%1 - ¥%2\n置信度：%3%")
                              .arg(minPrice).arg(maxPrice).arg(confidence);
        QMessageBox::information(this, "AI 估价结果", message);
        // 可以自动填入价格输入框
        goodsPriceEdit->setText(QString::number((minPrice + maxPrice) / 2));
    } else {
        QMessageBox::warning(this, "估价失败", result.value("error").toString());
    }
}
