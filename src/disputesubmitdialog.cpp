// disputesubmitdialog.cpp 完整实现
#include "disputesubmitdialog.h"
#include <QComboBox>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>
#include <QMessageBox>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>

DisputeSubmitDialog::DisputeSubmitDialog(QWidget *parent, int orderId)
    : QDialog(parent), orderId(orderId) {
    setWindowTitle("提交售后纠纷");
    setMinimumSize(500, 600);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    setupUI();
    loadOrderInfo(orderId);
}

void DisputeSubmitDialog::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 标题
    QLabel *titleLabel = new QLabel("提交售后纠纷");
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; margin-bottom: 20px;");
    mainLayout->addWidget(titleLabel);

    // 订单信息
    QGroupBox *orderGroup = new QGroupBox("订单信息");
    QGridLayout *orderLayout = new QGridLayout();

    orderLayout->addWidget(new QLabel("订单号:"), 0, 0);
    orderIdLabel = new QLabel(QString::number(orderId));
    orderLayout->addWidget(orderIdLabel, 0, 1);

    orderLayout->addWidget(new QLabel("商品名称:"), 1, 0);
    goodsNameLabel = new QLabel("二手iPhone 12 128GB");
    orderLayout->addWidget(goodsNameLabel, 1, 1);

    orderLayout->addWidget(new QLabel("订单金额:"), 2, 0);
    orderAmountLabel = new QLabel("¥2500");
    orderLayout->addWidget(orderAmountLabel, 2, 1);

    orderLayout->addWidget(new QLabel("卖家:"), 3, 0);
    sellerNameLabel = new QLabel("张三同学");
    orderLayout->addWidget(sellerNameLabel, 3, 1);

    orderGroup->setLayout(orderLayout);
    mainLayout->addWidget(orderGroup);

    // 纠纷信息
    QGroupBox *disputeGroup = new QGroupBox("纠纷信息");
    QVBoxLayout *disputeLayout = new QVBoxLayout();

    // 纠纷类型
    QWidget *typeWidget = new QWidget();
    QHBoxLayout *typeLayout = new QHBoxLayout(typeWidget);
    typeLayout->addWidget(new QLabel("纠纷类型:"));
    disputeTypeCombo = new QComboBox();
    disputeTypeCombo->addItems({"商品与描述不符", "商品质量问题", "卖家未发货",
                                "卖家发错货", "价格纠纷", "其他"});
    typeLayout->addWidget(disputeTypeCombo, 1);
    disputeLayout->addWidget(typeWidget);

    // 纠纷描述
    disputeLayout->addWidget(new QLabel("详细描述:"));
    descriptionEdit = new QTextEdit();
    descriptionEdit->setPlaceholderText("请详细描述纠纷情况...");
    descriptionEdit->setMaximumHeight(100);
    disputeLayout->addWidget(descriptionEdit);

    disputeGroup->setLayout(disputeLayout);
    mainLayout->addWidget(disputeGroup);

    // 证据材料
    QGroupBox *evidenceGroup = new QGroupBox("证据材料");
    QVBoxLayout *evidenceLayout = new QVBoxLayout();

    QLabel *evidenceHint = new QLabel("请上传相关证据（图片、聊天记录等）:");
    evidenceHint->setStyleSheet("color: #666; margin-bottom: 10px;");
    evidenceLayout->addWidget(evidenceHint);

    evidenceList = new QListWidget();
    evidenceList->setMaximumHeight(120);
    evidenceLayout->addWidget(evidenceList);

    QWidget *buttonWidget = new QWidget();
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);

    uploadBtn = new QPushButton("上传证据");
    uploadBtn->setObjectName("primaryBtn");

    removeBtn = new QPushButton("删除选中");
    removeBtn->setObjectName("secondaryBtn");

    buttonLayout->addWidget(uploadBtn);
    buttonLayout->addWidget(removeBtn);
    buttonLayout->addStretch();

    evidenceLayout->addWidget(buttonWidget);
    evidenceGroup->setLayout(evidenceLayout);
    mainLayout->addWidget(evidenceGroup);

    // AI分析
    QGroupBox *aiGroup = new QGroupBox("AI纠纷分析");
    QVBoxLayout *aiLayout = new QVBoxLayout();

    aiAnalysisText = new QTextEdit();
    aiAnalysisText->setReadOnly(true);
    aiAnalysisText->setMaximumHeight(80);
    aiAnalysisText->setText("根据您的描述，AI初步分析：\n• 可能属于商品质量问题\n• 建议提供更多图片证据");
    aiLayout->addWidget(aiAnalysisText);

    analyzeBtn = new QPushButton("重新分析");
    analyzeBtn->setObjectName("secondaryBtn");
    aiLayout->addWidget(analyzeBtn);

    aiGroup->setLayout(aiLayout);
    mainLayout->addWidget(aiGroup);

    // 提交按钮
    submitBtn = new QPushButton("提交纠纷申请");
    submitBtn->setObjectName("primaryBtn");
    submitBtn->setFixedHeight(45);
    mainLayout->addWidget(submitBtn);

    // 连接信号槽
    connect(uploadBtn, &QPushButton::clicked, this, &DisputeSubmitDialog::onUploadEvidence);
    connect(removeBtn, &QPushButton::clicked, [this]() {
        QListWidgetItem *item = evidenceList->currentItem();
        if (item) {
            delete item;
        }
    });
    connect(submitBtn, &QPushButton::clicked, this, &DisputeSubmitDialog::onSubmitDispute);
    connect(disputeTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &DisputeSubmitDialog::onDisputeTypeChanged);
    connect(analyzeBtn, &QPushButton::clicked, [this]() {
        aiAnalysisText->setText("正在分析...\n建议上传更多证据图片以获取更准确的分析结果");
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
    )");
}

void DisputeSubmitDialog::loadOrderInfo(int orderId) {
    // 加载订单信息
    // 这里应该从数据库加载
    this->orderId = orderId;

    // 模拟数据
    switch(orderId % 3) {
    case 0:
        goodsNameLabel->setText("二手iPhone 12 128GB");
        orderAmountLabel->setText("¥2500");
        break;
    case 1:
        goodsNameLabel->setText("大学物理教材");
        orderAmountLabel->setText("¥35");
        break;
    case 2:
        goodsNameLabel->setText("篮球鞋 Nike Air");
        orderAmountLabel->setText("¥280");
        break;
    }
}

void DisputeSubmitDialog::onUploadEvidence() {
    QString fileName = QFileDialog::getOpenFileName(this, "选择证据文件",
                                                    "", "Images (*.png *.jpg *.jpeg);;All Files (*)");
    if (!fileName.isEmpty()) {
        QString shortName = QFileInfo(fileName).fileName();
        evidenceList->addItem(shortName);
    }
}

void DisputeSubmitDialog::onSubmitDispute() {
    QString description = descriptionEdit->toPlainText().trimmed();
    if (description.isEmpty()) {
        QMessageBox::warning(this, "提示", "请填写纠纷描述");
        return;
    }

    QString disputeType = disputeTypeCombo->currentText();

    QMessageBox::StandardButton reply = QMessageBox::question(this, "确认提交",
                                                              QString("确定提交纠纷申请吗？\n类型: %1\n描述: %2").arg(disputeType).arg(description.left(50)),
                                                              QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        QMessageBox::information(this, "提交成功",
                                 "纠纷申请已提交！\n管理员将在24小时内处理\n处理结果将通过消息通知您");
        accept();
    }
}

void DisputeSubmitDialog::onDisputeTypeChanged(int index) {
    // 根据选择的纠纷类型给出建议
    QStringList suggestions = {
        "请提供商品与描述不符的对比图片",
        "请提供商品质量问题的详细照片",
        "请提供卖家承诺发货时间的聊天记录",
        "请提供正确商品与错误商品的对比",
        "请提供价格协商的聊天记录",
        "请详细描述具体情况"
    };

    if (index >= 0 && index < suggestions.size()) {
        QString suggestion = QString("建议: %1").arg(suggestions[index]);
        aiAnalysisText->setText(suggestion);
    }
}
