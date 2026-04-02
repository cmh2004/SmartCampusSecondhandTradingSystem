#include <QMessageBox>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QFont>
#include <QPalette>
#include <QJsonObject>
#include <QListWidgetItem>
#include "..\apiservice.h"
#include "disputesubmitdialog.h"

DisputeSubmitDialog::DisputeSubmitDialog(QWidget *parent, int orderId)
    : QDialog(parent), orderId(orderId) {
    setWindowTitle("提交售后纠纷");
    setMinimumSize(550, 850);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); // 移除帮助按钮
    setStyleSheet("QDialog { background-color: #f8f9fa; }"); // 背景色柔和化

    setupUI();
    loadOrderInfo(orderId);
    updateSubmitButtonState(); // 初始化按钮状态
}

void DisputeSubmitDialog::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10); // 外间距
    mainLayout->setSpacing(10); // 控件间距

    // ========== 标题区域 ==========
    QLabel *titleLabel = new QLabel("提交售后纠纷");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #2c3e50; margin-bottom: 5px;");
    titleLabel->setAlignment(Qt::AlignCenter); // 标题居中
    mainLayout->addWidget(titleLabel);

    // ========== 订单信息组 ==========
    QGroupBox *orderGroup = new QGroupBox("订单信息");
    orderGroup->setStyleSheet(R"(
        QGroupBox {
            font-size: 14px;
            font-weight: bold;
            color: #2c3e50;
            border: 1px solid #e0e6ed;
            border-radius: 8px;
            margin-top: 8px;
            padding-top: 15px;
            background-color: white;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 15px;
            padding: 0 8px 0 8px;
            color: #3498db;
        }
    )");

    QGridLayout *orderLayout = new QGridLayout(orderGroup);
    orderLayout->setContentsMargins(20, 10, 20, 20); // 组内间距
    orderLayout->setSpacing(15); // 网格间距
    orderLayout->setColumnStretch(1, 1); // 第二列自适应拉伸

    // 订单信息行（统一标签样式）
    auto createInfoLabel = [](const QString &text) -> QLabel* {
        QLabel *label = new QLabel(text);
        label->setStyleSheet("color: #7f8c8d; font-size: 13px;");
        label->setMinimumWidth(80);
        return label;
    };

    // 订单号
    orderLayout->addWidget(createInfoLabel("订单号:"), 0, 0);
    orderIdLabel = new QLabel(QString::number(orderId));
    orderIdLabel->setStyleSheet("color: #2c3e50; font-size: 13px;");
    orderLayout->addWidget(orderIdLabel, 0, 1);

    // 商品名称
    orderLayout->addWidget(createInfoLabel("商品名称:"), 1, 0);
    goodsNameLabel = new QLabel("二手iPhone 12 128GB");
    goodsNameLabel->setStyleSheet("color: #2c3e50; font-size: 13px;");
    orderLayout->addWidget(goodsNameLabel, 1, 1);

    // 订单金额
    orderLayout->addWidget(createInfoLabel("订单金额:"), 2, 0);
    orderAmountLabel = new QLabel("¥2500");
    orderAmountLabel->setStyleSheet("color: #e74c3c; font-size: 13px; font-weight: bold;");
    orderLayout->addWidget(orderAmountLabel, 2, 1);

    // 卖家
    orderLayout->addWidget(createInfoLabel("卖家:"), 3, 0);
    sellerNameLabel = new QLabel("张三同学");
    sellerNameLabel->setStyleSheet("color: #2c3e50; font-size: 13px;");
    orderLayout->addWidget(sellerNameLabel, 3, 1);

    mainLayout->addWidget(orderGroup);

    // ========== 纠纷信息组 ==========
    QGroupBox *disputeGroup = new QGroupBox("纠纷信息");
    disputeGroup->setStyleSheet(orderGroup->styleSheet()); // 复用组样式
    QVBoxLayout *disputeLayout = new QVBoxLayout(disputeGroup);
    disputeLayout->setContentsMargins(20, 5, 20, 10);
    disputeLayout->setSpacing(10);

    // 纠纷类型
    QWidget *typeWidget = new QWidget();
    QHBoxLayout *typeLayout = new QHBoxLayout(typeWidget);
    typeLayout->setContentsMargins(0, 0, 0, 0);
    typeLayout->setSpacing(10);
    typeLayout->addWidget(createInfoLabel("纠纷类型:"));

    disputeTypeCombo = new QComboBox();
    disputeTypeCombo->addItems({"商品与描述不符", "商品质量问题", "卖家未发货",
                                "卖家发错货", "价格纠纷", "其他"});
    disputeTypeCombo->setStyleSheet(R"(
        QComboBox {
            font-size: 13px;
            color: #2c3e50;
            border: 1px solid #e0e6ed;
            border-radius: 6px;
            padding: 4px 6px;
            background-color: white;
            min-height: 18px;
        }
        QComboBox::drop-down {
            border-left: 1px solid #e0e6ed;
            width: 30px;
        }
        QComboBox::down-arrow {
            image: url(:/icons/arrow_down.png); // 可选：替换下拉箭头图标
            width: 10px;
            height: 10px;
        }
        QComboBox:hover {
            border-color: #3498db;
        }
        QComboBox:focus {
            border-color: #3498db;
            outline: none;
            box-shadow: 0 0 0 2px rgba(52, 152, 219, 0.2);
        }
    )");
    typeLayout->addWidget(disputeTypeCombo, 1);
    disputeLayout->addWidget(typeWidget);

    // 纠纷描述
    disputeLayout->addWidget(createInfoLabel("详细描述:"));
    descriptionEdit = new QTextEdit();
    descriptionEdit->setPlaceholderText("请详细描述纠纷情况（如：商品收到后发现屏幕有破损，与卖家描述的全新不符...）");
    descriptionEdit->setStyleSheet(R"(
        QTextEdit {
            font-size: 13px;
            color: #2c3e50;
            border: 1px solid #e0e6ed;
            border-radius: 6px;
            padding: 6px;
            background-color: white;
            min-height: 100px;
            max-height: 120px;
        }
        QTextEdit:hover {
            border-color: #3498db;
        }
        QTextEdit:focus {
            border-color: #3498db;
            outline: none;
        }
        QTextEdit::placeholder {
            color: #bdc3c7;
        }
    )");
    // 监听描述输入变化，更新提交按钮状态
    connect(descriptionEdit, &QTextEdit::textChanged, this, &DisputeSubmitDialog::onDescriptionTextChanged);
    disputeLayout->addWidget(descriptionEdit);

    mainLayout->addWidget(disputeGroup);

    // ========== 证据材料组 ==========
    QGroupBox *evidenceGroup = new QGroupBox("证据材料");
    evidenceGroup->setStyleSheet(orderGroup->styleSheet());
    QVBoxLayout *evidenceLayout = new QVBoxLayout(evidenceGroup);
    evidenceLayout->setContentsMargins(20, 10, 20, 15);
    evidenceLayout->setSpacing(10);

    QLabel *evidenceHint = new QLabel("请上传图片/视频等相关证据（支持png/jpg/jpeg格式）");
    evidenceHint->setStyleSheet("color: #95a5a6; font-size: 12px;");
    evidenceLayout->addWidget(evidenceHint);

    // 证据列表
    evidenceList = new QListWidget();
    evidenceList->setStyleSheet(R"(
        QListWidget {
            font-size: 13px;
            color: #2c3e50;
            border: 1px solid #e0e6ed;
            border-radius: 6px;
            padding: 6px;
            background-color: white;
            min-height: 100px;
            max-height: 120px;
        }
        QListWidget::item {
            padding: 6px 8px;
            border-bottom: 1px solid #f1f5f9;
        }
        QListWidget::item:selected {
            background-color: #ebf5fb;
            color: #3498db;
            border-radius: 4px;
        }
        QListWidget:focus {
            border-color: #3498db;
            outline: none;
        }
    )");
    // 监听列表选中状态，更新删除按钮
    connect(evidenceList, &QListWidget::itemSelectionChanged, this, &DisputeSubmitDialog::onEvidenceItemSelected);
    evidenceLayout->addWidget(evidenceList);

    // 证据操作按钮
    QWidget *buttonWidget = new QWidget();
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(15);

    // 上传按钮
    uploadBtn = new QPushButton("上传证据");
    uploadBtn->setObjectName("primaryBtn");
    // 删除按钮（默认禁用）
    removeBtn = new QPushButton("删除选中");
    removeBtn->setObjectName("secondaryBtn");
    removeBtn->setEnabled(false); // 初始无选中，禁用

    buttonLayout->addWidget(uploadBtn);
    buttonLayout->addWidget(removeBtn);
    buttonLayout->addStretch();

    evidenceLayout->addWidget(buttonWidget);
    mainLayout->addWidget(evidenceGroup);

    // ========== 提交按钮 ==========
    submitBtn = new QPushButton("提交纠纷申请");
    submitBtn->setObjectName("submitBtn");
    submitBtn->setFixedHeight(36);
    submitBtn->setFont(QFont("Microsoft YaHei", 12));
    submitBtn->setEnabled(false); // 初始禁用（描述为空）
    mainLayout->addWidget(submitBtn);

    // ========== 信号槽连接 ==========
    connect(uploadBtn, &QPushButton::clicked, this, &DisputeSubmitDialog::onUploadEvidence);
    connect(removeBtn, &QPushButton::clicked, [this]() {
        QListWidgetItem *item = evidenceList->takeItem(evidenceList->currentRow());
        if (item) delete item;
    });
    connect(submitBtn, &QPushButton::clicked, this, &DisputeSubmitDialog::onSubmitDispute);

    // ========== 全局样式 ==========
    setStyleSheet(R"(
        #primaryBtn {
            background-color: #3498db;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 8px 18px;
            font-size: 13px;
            font-weight: 500;
        }
        #primaryBtn:hover {
            background-color: #2980b9;
        }
        #primaryBtn:pressed {
            background-color: #1f618d;
        }
        #secondaryBtn {
            background-color: #e74c3c;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 8px 18px;
            font-size: 13px;
            font-weight: 500;
        }
        #secondaryBtn:hover:enabled {
            background-color: #c0392b;
        }
        #secondaryBtn:pressed:enabled {
            background-color: #a93226;
        }
        #secondaryBtn:disabled {
            background-color: #ec7063;
            color: #f8f9fa;
        }
        /* 提交按钮 */
        #submitBtn {
            background-color: #2ecc71;
            color: white;
            border: none;
            border-radius: 8px;
            font-weight: bold;
        }
        #submitBtn:hover:enabled {
            background-color: #27ae60;
        }
        #submitBtn:pressed:enabled {
            background-color: #219653;
        }
        #submitBtn:disabled {
            background-color: #a5d6a7;
            color: #f8f9fa;
        }
    )");
}

void DisputeSubmitDialog::loadOrderInfo(int orderId) {
    this->orderId = orderId;
    switch(orderId % 3) {
    case 0:
        goodsNameLabel->setText("二手iPhone 12 128GB 国行版");
        orderAmountLabel->setText("¥2500.00");
        sellerNameLabel->setText("张三同学（学号：2022001）");
        break;
    case 1:
        goodsNameLabel->setText("大学物理（第七版）上下册 + 习题解答");
        orderAmountLabel->setText("¥35.00");
        sellerNameLabel->setText("李四同学（学号：2022002）");
        break;
    case 2:
        goodsNameLabel->setText("Nike Air Zoom 篮球鞋 43码 95新");
        orderAmountLabel->setText("¥280.00");
        sellerNameLabel->setText("王五同学（学号：2022003）");
        break;
    }
}

void DisputeSubmitDialog::onUploadEvidence() {
    QStringList fileNames = QFileDialog::getOpenFileNames(
        this,
        "选择证据文件",
        "",
        "图片文件 (*.png *.jpg *.jpeg);;所有文件 (*.*)"
        );
    if (!fileNames.isEmpty()) {
        for (const QString &fileName : fileNames) {
            QString shortName = QFileInfo(fileName).fileName();
            QListWidgetItem *item = new QListWidgetItem(shortName);
            item->setToolTip(fileName); // 悬停显示完整路径
            evidenceList->addItem(item);
        }
    }
}

void DisputeSubmitDialog::onSubmitDispute() {
    QString disputeType = disputeTypeCombo->currentText();
    QString description = descriptionEdit->toPlainText().trimmed();
    if (description.isEmpty()) {
        QMessageBox::warning(this, "提示", "请填写纠纷描述");
        return;
    }

    // 上传证据文件
    QStringList evidenceUrls;
    for (int i = 0; i < evidenceList->count(); ++i) {
        QListWidgetItem *item = evidenceList->item(i);
        QString filePath = item->data(Qt::UserRole).toString();
        if (!filePath.isEmpty()) {
            QJsonObject uploadResult = ApiService::instance()->uploadImage(filePath);
            if (uploadResult.value("success").toBool()) {
                QString url = uploadResult.value("data").toObject().value("file_url").toString();
                evidenceUrls.append(url);
            } else {
                QMessageBox::warning(this, "证据上传失败", uploadResult.value("error").toString());
                return; // 可选择继续或中止
            }
        }
    }

    QJsonObject result = ApiService::instance()->submitDispute(orderId, disputeType, description, evidenceUrls);
    if (result.value("success").toBool()) {
        QMessageBox::information(this, "成功", "纠纷已提交");
        accept();
    } else {
        QMessageBox::warning(this, "失败", result.value("error").toString());
    }
}

void DisputeSubmitDialog::onEvidenceItemSelected() {
    // 有选中项则启用删除按钮，否则禁用
    removeBtn->setEnabled(evidenceList->currentItem() != nullptr);
}

void DisputeSubmitDialog::onDescriptionTextChanged() {
    updateSubmitButtonState();
}

void DisputeSubmitDialog::updateSubmitButtonState() {
    // 描述不为空时启用提交按钮
    QString description = descriptionEdit->toPlainText().trimmed();
    submitBtn->setEnabled(!description.isEmpty());
}
