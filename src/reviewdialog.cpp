#include "reviewdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QFileDialog>

ReviewDialog::ReviewDialog(QWidget *parent, int orderId, QString sellerName)
    : QDialog(parent), orderId(orderId), sellerName(sellerName) {
    setWindowTitle("评价订单");
    setFixedSize(500, 600);
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
        starBtn->setIcon(QIcon(":/icons/star_empty.png"));
        starBtn->setIconSize(QSize(40, 40));
        starBtn->setProperty("rating", i + 1);

        // 设置选中时的图标
        connect(starBtn, &QRadioButton::toggled, [starBtn](bool checked) {
            if (checked) {
                starBtn->setIcon(QIcon(":/icons/star_filled.png"));
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
    uploadImageBtn->setObjectName("secondaryBtn");

    imagePreview = new QLabel();
    imagePreview->setFixedSize(150, 150);
    imagePreview->setStyleSheet("border: 1px dashed #ccc; border-radius: 4px;");
    imagePreview->setAlignment(Qt::AlignCenter);
    imagePreview->setText("暂无图片");

    connect(uploadImageBtn, &QPushButton::clicked, [this]() {
        QString fileName = QFileDialog::getOpenFileName(this, "选择评价图片",
                                                        "", "Images (*.png *.jpg *.jpeg)");
        if (!fileName.isEmpty()) {
            QPixmap pixmap(fileName);
            imagePreview->setPixmap(pixmap.scaled(150, 150, Qt::KeepAspectRatio));
        }
    });

    QHBoxLayout *imageBtnLayout = new QHBoxLayout();
    imageBtnLayout->addWidget(uploadImageBtn);
    imageBtnLayout->addWidget(imagePreview);
    imageBtnLayout->addStretch();

    imageLayout->addLayout(imageBtnLayout);
    imageGroup->setLayout(imageLayout);
    mainLayout->addWidget(imageGroup);

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
    )");
}

void ReviewDialog::onSubmitReview() {
    // 获取评分
    int rating = 5;
    for (int i = 0; i < starButtons.size(); i++) {
        if (starButtons[i]->isChecked()) {
            rating = i + 1;
            break;
        }
    }

    QString comment = reviewEdit->toPlainText().trimmed();

    if (comment.isEmpty()) {
        QMessageBox::warning(this, "提示", "请填写评价内容");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "确认提交",
        QString("确定提交评价吗？\n评分: %1星\n评价内容: %2")
            .arg(rating)
            .arg(comment.left(50)),
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        emit reviewSubmitted(orderId, rating, comment);
        QMessageBox::information(this, "评价成功", "感谢您的评价！\n评价已提交成功。");
        accept();
    }
}
