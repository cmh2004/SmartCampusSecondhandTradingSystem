#include "reportsubmitdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QFileDialog>
#include <QDateTime>

ReportSubmitDialog::ReportSubmitDialog(QWidget *parent, int targetId,
                                       QString targetType, QString targetName)
    : QDialog(parent), targetId(targetId), targetType(targetType), targetName(targetName) {
    setWindowTitle("提交举报");
    setMinimumSize(600, 700);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    setupUI();
    loadTargetInfo();
}

void ReportSubmitDialog::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 标题
    QLabel *titleLabel = new QLabel("提交举报");
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #2c3e50;");
    mainLayout->addWidget(titleLabel);

    // 被举报对象信息
    QGroupBox *targetGroup = new QGroupBox("被举报对象");
    QVBoxLayout *targetLayout = new QVBoxLayout();

    targetTitleLabel = new QLabel();
    targetTitleLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #333;");

    targetInfoLabel = new QLabel();
    targetInfoLabel->setStyleSheet("color: #666;");
    targetInfoLabel->setWordWrap(true);

    targetLayout->addWidget(targetTitleLabel);
    targetLayout->addWidget(targetInfoLabel);
    targetGroup->setLayout(targetLayout);
    mainLayout->addWidget(targetGroup);

    // 举报类型
    QGroupBox *typeGroup = new QGroupBox("举报类型");
    QVBoxLayout *typeLayout = new QVBoxLayout();

    reportTypeCombo = new QComboBox();

    if (targetType == "goods") {
        reportTypeCombo->addItems({
            "虚假信息/描述不符",
            "价格欺诈/恶意抬价",
            "违禁物品",
            "商品侵权/盗版",
            "重复发布/恶意刷屏",
            "其他违规行为"
        });
    } else if (targetType == "user") {
        reportTypeCombo->addItems({
            "欺诈行为",
            "恶意骚扰",
            "虚假身份",
            "违规交易",
            "发布违规内容",
            "其他违规行为"
        });
    } else if (targetType == "order") {
        reportTypeCombo->addItems({
            "交易纠纷",
            "不履行交易",
            "恶意退款",
            "虚假发货",
            "其他违规行为"
        });
    }

    typeLayout->addWidget(reportTypeCombo);
    typeGroup->setLayout(typeLayout);
    mainLayout->addWidget(typeGroup);

    // 举报描述
    QGroupBox *descGroup = new QGroupBox("举报描述");
    QVBoxLayout *descLayout = new QVBoxLayout();

    QLabel *descHint = new QLabel("请详细描述举报原因和具体情况：");
    descHint->setStyleSheet("color: #666;");

    descriptionEdit = new QTextEdit();
    descriptionEdit->setPlaceholderText("请尽可能详细地描述具体情况，包括时间、地点、经过等。\n您的描述越详细，处理越快。");
    descriptionEdit->setMaximumHeight(120);

    descLayout->addWidget(descHint);
    descLayout->addWidget(descriptionEdit);
    descGroup->setLayout(descLayout);
    mainLayout->addWidget(descGroup);

    // 证据材料
    QGroupBox *evidenceGroup = new QGroupBox("证据材料");
    QVBoxLayout *evidenceLayout = new QVBoxLayout();

    QLabel *evidenceHint = new QLabel("请上传相关证据（图片、聊天记录、订单截图等）：");
    evidenceHint->setStyleSheet("color: #666;");

    evidenceList = new QListWidget();
    evidenceList->setMaximumHeight(100);

    QWidget *evidenceButtonWidget = new QWidget();
    QHBoxLayout *evidenceButtonLayout = new QHBoxLayout(evidenceButtonWidget);
    evidenceButtonLayout->setContentsMargins(0, 0, 0, 0);

    uploadBtn = new QPushButton("上传证据");
    QPushButton *removeBtn = new QPushButton("删除选中");

    uploadBtn->setObjectName("secondaryBtn");
    removeBtn->setObjectName("secondaryBtn");

    evidenceButtonLayout->addWidget(uploadBtn);
    evidenceButtonLayout->addWidget(removeBtn);
    evidenceButtonLayout->addStretch();

    evidenceLayout->addWidget(evidenceHint);
    evidenceLayout->addWidget(evidenceList);
    evidenceLayout->addWidget(evidenceButtonWidget);
    evidenceGroup->setLayout(evidenceLayout);
    mainLayout->addWidget(evidenceGroup);

    // 联系方式
    QGroupBox *contactGroup = new QGroupBox("联系方式（选填）");
    QVBoxLayout *contactLayout = new QVBoxLayout();

    QLabel *contactHint = new QLabel("便于我们与您联系核实情况：");
    contactHint->setStyleSheet("color: #666;");

    contactEdit = new QLineEdit();
    contactEdit->setPlaceholderText("邮箱或手机号（选填）");

    contactLayout->addWidget(contactHint);
    contactLayout->addWidget(contactEdit);
    contactGroup->setLayout(contactLayout);
    mainLayout->addWidget(contactGroup);

    // 注意事项
    QGroupBox *noticeGroup = new QGroupBox("注意事项");
    QVBoxLayout *noticeLayout = new QVBoxLayout();

    QLabel *noticeText = new QLabel();
    noticeText->setWordWrap(true);
    noticeText->setStyleSheet("color: #666; font-size: 12px;");
    noticeText->setText("1. 请确保举报内容真实有效，恶意举报将受到处罚\n"
                        "2. 管理员将在24小时内处理您的举报\n"
                        "3. 处理结果将通过系统消息通知您\n"
                        "4. 举报过程严格保密，不会泄露您的个人信息");

    noticeLayout->addWidget(noticeText);
    noticeGroup->setLayout(noticeLayout);
    mainLayout->addWidget(noticeGroup);

    // 按钮区域
    QWidget *buttonWidget = new QWidget();
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);

    cancelBtn = new QPushButton("取消");
    submitBtn = new QPushButton("提交举报");

    cancelBtn->setObjectName("secondaryBtn");
    submitBtn->setObjectName("primaryBtn");

    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelBtn);
    buttonLayout->addWidget(submitBtn);

    mainLayout->addWidget(buttonWidget);

    // 连接信号槽
    connect(submitBtn, &QPushButton::clicked, this, &ReportSubmitDialog::onSubmitReport);
    connect(cancelBtn, &QPushButton::clicked, this, &ReportSubmitDialog::onCancel);
    connect(uploadBtn, &QPushButton::clicked, this, &ReportSubmitDialog::onUploadEvidence);
    connect(removeBtn, &QPushButton::clicked, [this]() {
        QList<QListWidgetItem*> items = evidenceList->selectedItems();
        foreach (QListWidgetItem *item, items) {
            delete item;
        }
    });
    connect(reportTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ReportSubmitDialog::onReportTypeChanged);

    // 设置样式
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
        QTextEdit, QLineEdit {
            border: 1px solid #ddd;
            border-radius: 4px;
            padding: 6px;
        }
    )");
}

void ReportSubmitDialog::loadTargetInfo() {
    if (targetType == "goods") {
        targetTitleLabel->setText("举报商品");
        targetInfoLabel->setText(QString("商品ID: %1\n商品名称: %2")
                                     .arg(targetId)
                                     .arg(targetName.isEmpty() ? "未知商品" : targetName));
    } else if (targetType == "user") {
        targetTitleLabel->setText("举报用户");
        targetInfoLabel->setText(QString("用户ID: %1\n用户名: %2")
                                     .arg(targetId)
                                     .arg(targetName.isEmpty() ? "未知用户" : targetName));
    } else if (targetType == "order") {
        targetTitleLabel->setText("举报订单");
        targetInfoLabel->setText(QString("订单号: %1\n相关商品: %2")
                                     .arg(targetId)
                                     .arg(targetName.isEmpty() ? "未知订单" : targetName));
    }
}

void ReportSubmitDialog::onSubmitReport() {
    QString description = descriptionEdit->toPlainText().trimmed();
    QString reportType = reportTypeCombo->currentText();

    if (description.isEmpty()) {
        QMessageBox::warning(this, "提示", "请填写举报描述");
        return;
    }

    if (description.length() < 10) {
        QMessageBox::warning(this, "提示", "举报描述过短，请详细描述具体情况");
        return;
    }

    // 验证联系方式格式（如果填写了）
    QString contact = contactEdit->text().trimmed();
    if (!contact.isEmpty()) {
        if (contact.contains('@')) {
            // 邮箱验证
            QRegularExpression emailRegex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
            if (!emailRegex.match(contact).hasMatch()) {
                QMessageBox::warning(this, "提示", "邮箱格式不正确");
                return;
            }
        } else {
            // 手机号验证
            QRegularExpression phoneRegex(R"(^1[3-9]\d{9}$)");
            if (!phoneRegex.match(contact).hasMatch()) {
                QMessageBox::warning(this, "提示", "手机号格式不正确");
                return;
            }
        }
    }

    QString evidenceCount = QString::number(evidenceList->count());

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "确认提交",
        QString("确定提交举报吗？\n\n"
                "举报类型: %1\n"
                "证据材料: %2份\n"
                "联系方式: %3\n\n"
                "提交后不可修改，管理员将在24小时内处理。")
            .arg(reportType)
            .arg(evidenceCount)
            .arg(contact.isEmpty() ? "无" : contact),
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        // 这里应该提交举报到服务器
        qDebug() << "提交举报:"
                 << "targetId:" << targetId
                 << "targetType:" << targetType
                 << "reportType:" << reportType
                 << "evidenceCount:" << evidenceCount;

        emit reportSubmitted(targetId, targetType);

        QMessageBox::information(this, "提交成功",
                                 "举报已提交成功！\n\n"
                                 "管理员将在24小时内处理，处理结果将通过系统消息通知您。\n"
                                 "感谢您为维护平台秩序做出的贡献！");

        accept();
    }
}

void ReportSubmitDialog::onUploadEvidence() {
    QStringList fileNames = QFileDialog::getOpenFileNames(this, "选择证据文件",
                                                          "",
                                                          "图片文件 (*.png *.jpg *.jpeg);;"
                                                          "文本文件 (*.txt);;"
                                                          "所有文件 (*)");

    if (fileNames.isEmpty()) return;

    foreach (QString fileName, fileNames) {
        QString shortName = QFileInfo(fileName).fileName();
        QListWidgetItem *item = new QListWidgetItem(shortName);
        item->setData(Qt::UserRole, fileName); // 保存完整路径

        // 根据文件类型设置图标
        if (fileName.endsWith(".png") || fileName.endsWith(".jpg") || fileName.endsWith(".jpeg")) {
            item->setIcon(QIcon(":/icons/image.png"));
        } else if (fileName.endsWith(".txt")) {
            item->setIcon(QIcon(":/icons/text.png"));
        } else {
            item->setIcon(QIcon(":/icons/file.png"));
        }

        evidenceList->addItem(item);
    }
}

void ReportSubmitDialog::onReportTypeChanged(int index) {
    // 根据举报类型给出建议
    QStringList suggestions;

    if (targetType == "goods") {
        suggestions = {
            "请提供商品与描述不符的对比图片",
            "请提供价格欺诈的相关证据（如聊天记录）",
            "请说明违禁物品的具体情况",
            "请提供商品侵权的证明文件",
            "请提供重复发布的截图证据",
            "请详细描述具体违规情况"
        };
    } else if (targetType == "user") {
        suggestions = {
            "请提供欺诈行为的聊天记录和证据",
            "请提供骚扰内容的截图",
            "请说明虚假身份的具体表现",
            "请提供违规交易的证据",
            "请提供违规内容的截图",
            "请详细描述具体违规情况"
        };
    } else if (targetType == "order") {
        suggestions = {
            "请提供交易纠纷的详细说明和证据",
            "请提供对方不履行交易的证据",
            "请提供恶意退款的聊天记录",
            "请提供虚假发货的物流信息",
            "请详细描述具体违规情况"
        };
    }

    if (index >= 0 && index < suggestions.size()) {
        QString suggestion = QString("建议: %1").arg(suggestions[index]);
        QMessageBox::information(this, "举报建议", suggestion);
    }
}

void ReportSubmitDialog::onCancel() {
    QString description = descriptionEdit->toPlainText().trimmed();

    if (!description.isEmpty()) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "确认取消",
            "确定要取消举报吗？已填写的内容将丢失。",
            QMessageBox::Yes | QMessageBox::No
            );

        if (reply == QMessageBox::Yes) {
            reject();
        }
    } else {
        reject();
    }
}
