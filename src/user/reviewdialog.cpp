#include <QVBoxLayout>
#include <QScrollArea>
#include <QFileDialog>
#include <QJsonObject>
#include "..\apiservice.h"
#include "reviewdialog.h"

ReviewDialog::ReviewDialog(QWidget *parent, int orderId, QString sellerName)
    : QDialog(parent), orderId(orderId), sellerName(sellerName) {
    setWindowTitle("评价订单");
    setFixedSize(600, 800);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    setupUI();
}

void ReviewDialog::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 订单信息
    QGroupBox *orderGroup = new QGroupBox("订单信息");
    QVBoxLayout *orderLayout = new QVBoxLayout();

    QLabel *orderLabel = new QLabel(QString("订单号: %1").arg(orderId));
    QLabel *sellerLabel = new QLabel(QString("卖家: %1").arg(sellerName));
    QLabel *goodsLabel = new QLabel("商品: 二手iPhone 12 128GB");

    orderLayout->addWidget(orderLabel);
    orderLayout->addWidget(sellerLabel);
    orderLayout->addWidget(goodsLabel);
    orderGroup->setLayout(orderLayout);
    mainLayout->addWidget(orderGroup);

    // 评分
    QGroupBox *ratingGroup = new QGroupBox("请为本次交易评分");
    QVBoxLayout *ratingLayout = new QVBoxLayout();

    QWidget *starWidget = new QWidget();
    QHBoxLayout *starLayout = new QHBoxLayout(starWidget);
    starLayout->setContentsMargins(0, 0, 0, 0);

    for (int i = 0; i < 5; i++) {
        QRadioButton *starBtn = new QRadioButton();
        starBtn->setIcon(QIcon(":/icons/img/star_empty.png"));
        starBtn->setIconSize(QSize(40, 40));
        starBtn->setProperty("rating", i + 1);

        // 设置选中时的图标
        connect(starBtn, &QRadioButton::toggled, [this, i](bool checked) {
            if (checked) {
                // 选中当前星 → 0~i 全部实心，i+1~4 全部空心
                for(int j=0; j<5; j++){
                    if(j <= i){
                        starButtons[j]->setIcon(QIcon(":/icons/img/star_filled.png"));
                    }else{
                        starButtons[j]->setIcon(QIcon(":/icons/img/star_empty.png"));
                    }
                }
            }
        });

        starButtons.append(starBtn);
        starLayout->addWidget(starBtn);
    }

    starButtons[4]->setChecked(true); // 默认5星

    ratingLayout->addWidget(starWidget);
    ratingGroup->setLayout(ratingLayout);
    mainLayout->addWidget(ratingGroup);

    // 评价内容
    QGroupBox *contentGroup = new QGroupBox("评价内容");
    QVBoxLayout *contentLayout = new QVBoxLayout();

    reviewEdit = new QTextEdit();
    reviewEdit->setPlaceholderText("请详细描述您的购物体验...\n（建议包含：商品质量、卖家服务、物流速度等）");
    reviewEdit->setMaximumHeight(150);

    contentLayout->addWidget(reviewEdit);
    contentGroup->setLayout(contentLayout);
    mainLayout->addWidget(contentGroup);

    // 标签选择
    QGroupBox *tagGroup = new QGroupBox("选择标签（可选）");
    QVBoxLayout *tagLayout = new QVBoxLayout();

    QWidget *tagWidget = new QWidget();
    QGridLayout *tagGrid = new QGridLayout(tagWidget);

    QStringList tags = {"商品与描述一致", "发货速度快", "卖家态度好",
                        "包装完好", "性价比高", "会推荐给同学"};

    for (int i = 0; i < tags.size(); i++) {
        QCheckBox *tagCheck = new QCheckBox(tags[i]);
        tagGrid->addWidget(tagCheck, i / 2, i % 2);
    }

    tagLayout->addWidget(tagWidget);
    tagGroup->setLayout(tagLayout);
    mainLayout->addWidget(tagGroup);

    // 上传图片
    QGroupBox *imageGroup = new QGroupBox("上传图片（可选）");
    QVBoxLayout *imageLayout = new QVBoxLayout();

    uploadImageBtn = new QPushButton("选择图片");
    uploadImageBtn->setObjectName("primaryBtn");

    // 创建水平布局来放置按钮和图片预览
    QHBoxLayout *imageBtnLayout = new QHBoxLayout();
    imageBtnLayout->addStretch();
    imageBtnLayout->addWidget(uploadImageBtn);
    imageBtnLayout->addStretch();

    // 创建滚动区域用于显示多张图片预览
    QScrollArea *imageScrollArea = new QScrollArea();
    imageScrollArea->setFixedHeight(180);
    imageScrollArea->setWidgetResizable(true);
    imageScrollArea->setStyleSheet("border: none; background-color: transparent;");

    // 创建滚动区域的内容部件
    QWidget *imageContainer = new QWidget();
    imageContainerLayout = new QHBoxLayout(imageContainer); // 将 layout 改为成员变量以便访问
    imageContainerLayout->setSpacing(10);
    imageContainerLayout->setContentsMargins(5, 5, 5, 5);
    imageContainerLayout->setAlignment(Qt::AlignLeft);

    // 添加初始的"添加图片"按钮
    QLabel *addImageLabel = createImagePreviewLabel(true);
    imageContainerLayout->addWidget(addImageLabel);

    imageScrollArea->setWidget(imageContainer);

    imageLayout->addLayout(imageBtnLayout);
    imageLayout->addWidget(imageScrollArea);
    imageGroup->setLayout(imageLayout);
    mainLayout->addWidget(imageGroup);

    // 连接信号槽
    connect(uploadImageBtn, &QPushButton::clicked, [this]() {
        QStringList fileNames = QFileDialog::getOpenFileNames(this, "选择评价图片",
                                                              "", "Images (*.png *.jpg *.jpeg *.bmp)");
        if (!fileNames.isEmpty()) {
            // 检查是否已存在"添加图片"按钮
            bool hasAddButton = false;
            for (int i = 0; i < imageContainerLayout->count(); i++) {
                QLayoutItem *item = imageContainerLayout->itemAt(i);
                if (item && item->widget()) {
                    QLabel *label = qobject_cast<QLabel*>(item->widget());
                    if (label && label->property("isAddButton").toBool()) {
                        hasAddButton = true;
                        break;
                    }
                }
            }

            // 如果没有"添加图片"按钮，先添加一个
            if (!hasAddButton) {
                QLabel *addLabel = createImagePreviewLabel(true);
                imageContainerLayout->addWidget(addLabel);
            }

            // 添加新选择的图片
            for (const QString &fileName : fileNames) {
                QPixmap pixmap(fileName);
                if (!pixmap.isNull()) {
                    QLabel *imageLabel = createImagePreviewLabel(false);
                    imageLabel->setPixmap(pixmap.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    imageLabel->setProperty("imagePath", fileName);

                    // 在"添加图片"按钮之前插入
                    int insertIndex = imageContainerLayout->count() - 2;
                    if (insertIndex < 0) insertIndex = 0;
                    imageContainerLayout->insertWidget(insertIndex, imageLabel);

                    // 保存图片路径
                    uploadedImages.append(fileName);
                }
            }

            // 限制最多5张图片
            if (uploadedImages.size() >= 5) {
                // 移除"添加图片"按钮
                for (int i = imageContainerLayout->count() - 1; i >= 0; i--) {
                    QLayoutItem *item = imageContainerLayout->itemAt(i);
                    if (item && item->widget()) {
                        QLabel *label = qobject_cast<QLabel*>(item->widget());
                        if (label && label->property("isAddButton").toBool()) {
                            imageContainerLayout->removeWidget(label);
                            delete label;
                            break;
                        }
                    }
                }
            }
        }
    });

    // 按钮
    QWidget *buttonWidget = new QWidget();
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);

    submitBtn = new QPushButton("提交评价");
    cancelBtn = new QPushButton("取消");

    submitBtn->setObjectName("primaryBtn");
    cancelBtn->setObjectName("secondaryBtn");

    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelBtn);
    buttonLayout->addWidget(submitBtn);

    mainLayout->addWidget(buttonWidget);

    // 连接信号槽
    connect(submitBtn, &QPushButton::clicked, this, &ReviewDialog::onSubmitReview);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    // 样式
    setStyleSheet(R"(
        QDialog {
            background-color: white;
        }
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
        QRadioButton {
            background: transparent;
            border: none;
        }
        QRadioButton::indicator {
            width: 0px;
            height: 0px;
            border: none;
        }
        #primaryBtn {
            background-color: #3498db;
            color: white;
            border-radius: 4px;
            padding: 10px 20px;
        }
        #secondaryBtn {
            background-color: #ecf0f1;
            color: #34495e;
            border-radius: 4px;
            padding: 8px 16px;
        }
        QScrollArea {
            border: none;
            background-color: transparent;
        }
        QScrollBar:vertical {
            border: none;
            background-color: #f5f5f5;
            width: 10px;
            border-radius: 5px;
        }
        QScrollBar::handle:vertical {
            background-color: #bdc3c7;
            border-radius: 5px;
            min-height: 30px;
        }
        QScrollBar::handle:vertical:hover {
            background-color: #95a5a6;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            border: none;
            background: none;
            height: 0px;
        }
        QScrollBar:horizontal {
            border: none;
            background-color: #f5f5f5;
            height: 10px;
            border-radius: 5px;
        }
        QScrollBar::handle:horizontal {
            background-color: #bdc3c7;
            border-radius: 5px;
            min-width: 30px;
        }
        QScrollBar::handle:horizontal:hover {
            background-color: #95a5a6;
        }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            border: none;
            background: none;
            width: 0px;
        }
    )");
}

void ReviewDialog::onSubmitReview() {
    // 获取评分、评论等
    int rating = getRating();
    QString comment = reviewEdit->toPlainText().trimmed();
    if (comment.isEmpty()) {
        QMessageBox::warning(this, "提示", "请填写评价内容");
        return;
    }

    // 上传图片
    QStringList imageUrls;
    for (const QString &path : uploadedImages) {
        QJsonObject uploadResult = ApiService::instance()->uploadImage(path);
        if (uploadResult.value("success").toBool()) {
            QString url = uploadResult.value("data").toObject().value("file_url").toString();
            imageUrls.append(url);
        } else {
            QMessageBox::warning(this, "图片上传失败", uploadResult.value("error").toString());
            return; // 可选择继续或中止
        }
    }

    // 提交评价，传递图片 URL 列表
    QJsonObject result = ApiService::instance()->submitReview(orderId, rating, comment, imageUrls);
    if (result.value("success").toBool()) {
        QMessageBox::information(this, "成功", "评价提交成功");
        accept();
    } else {
        QMessageBox::warning(this, "失败", result.value("error").toString());
    }
}

QLabel* ReviewDialog::createImagePreviewLabel(bool isAddButton) {
    QLabel *label = new QLabel();
    label->setFixedSize(150, 150);
    label->setStyleSheet(isAddButton ?
                             "border: 2px dashed #3498db; border-radius: 8px; background-color: #f8f9fa;" :
                             "border: 2px solid #e0e0e0; border-radius: 8px; background-color: white;"
                         );
    label->setAlignment(Qt::AlignCenter);

    if (isAddButton) {
        label->setProperty("isAddButton", true);

        // 创建添加图标
        QLabel *addIcon = new QLabel(label);
        addIcon->setPixmap(QPixmap(":/icons/img/plus.png").scaled(110, 110, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        addIcon->setAlignment(Qt::AlignCenter);
        addIcon->setGeometry(0, 0, 150, 150);
    } else {
        label->setProperty("isAddButton", false);

        // 添加删除按钮
        QPushButton *deleteBtn = new QPushButton(label);
        deleteBtn->setFixedSize(26, 26);
        deleteBtn->setStyleSheet(R"(
            QPushButton {
                background-color: rgba(231, 76, 60, 0.9);
                color: white;
                border-radius: 12px;
                border: 2px solid white;
                font-weight: bold;
                padding:3px;
            }
            QPushButton:hover {
                background-color: rgba(192, 57, 43, 0.9);
            }
        )");
        deleteBtn->setText("×");
        deleteBtn->setFont(QFont("Arial", 12, QFont::Bold));
        deleteBtn->move(120, 3);

        // 删除按钮点击事件
        connect(deleteBtn, &QPushButton::clicked, [this, label]() {
            // 从列表中移除图片路径
            QString imagePath = label->property("imagePath").toString();
            uploadedImages.removeAll(imagePath);

            // 从布局中移除并删除标签
            imageContainerLayout->removeWidget(label);
            delete label;

            // 检查是否需要重新添加"添加图片"按钮
            bool hasAddButton = false;
            for (int i = 0; i < imageContainerLayout->count(); i++) {
                QLayoutItem *item = imageContainerLayout->itemAt(i);
                if (item && item->widget()) {
                    QLabel *existingLabel = qobject_cast<QLabel*>(item->widget());
                    if (existingLabel && existingLabel->property("isAddButton").toBool()) {
                        hasAddButton = true;
                        break;
                    }
                }
            }

            // 如果图片少于5张且没有添加按钮，则添加一个
            if (uploadedImages.size() < 5 && !hasAddButton) {
                QLabel *addLabel = createImagePreviewLabel(true);
                imageContainerLayout->addWidget(addLabel);
            }
        });
    }

    return label;
}

int ReviewDialog::getRating() const {
    for (int i = 0; i < starButtons.size(); ++i) {
        if (starButtons[i]->isChecked()) {
            return i + 1;   // 索引0对应1星，索引1对应2星，...，索引4对应5星
        }
    }
    return 5; // 默认5星（理论上总有一个被选中）
}
