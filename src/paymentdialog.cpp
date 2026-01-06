#include "paymentdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QDateTime>

PaymentDialog::PaymentDialog(QWidget *parent, int orderId, double amount)
    : QDialog(parent), orderId(orderId), amount(amount) {
    setWindowTitle("支付");
    setFixedSize(400, 500);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    setupUI();
}

void PaymentDialog::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 订单信息
    QGroupBox *orderGroup = new QGroupBox("订单信息");
    QVBoxLayout *orderLayout = new QVBoxLayout();

    orderIdLabel = new QLabel(QString("订单号: %1").arg(orderId));
    amountLabel = new QLabel(QString("支付金额: ¥%1").arg(amount, 0, 'f', 2));
    goodsNameLabel = new QLabel("商品: 二手iPhone 12 128GB");

    orderLayout->addWidget(orderIdLabel);
    orderLayout->addWidget(amountLabel);
    orderLayout->addWidget(goodsNameLabel);
    orderGroup->setLayout(orderLayout);
    mainLayout->addWidget(orderGroup);

    // 支付方式
    QGroupBox *methodGroup = new QGroupBox("选择支付方式");
    QVBoxLayout *methodLayout = new QVBoxLayout();

    wechatRadio = new QRadioButton("微信支付");
    alipayRadio = new QRadioButton("支付宝");
    campusCardRadio = new QRadioButton("校园卡支付");

    wechatRadio->setIcon(QIcon(":/icons/wechat.png"));
    alipayRadio->setIcon(QIcon(":/icons/alipay.png"));
    campusCardRadio->setIcon(QIcon(":/icons/card.png"));

    wechatRadio->setChecked(true);

    methodLayout->addWidget(wechatRadio);
    methodLayout->addWidget(alipayRadio);
    methodLayout->addWidget(campusCardRadio);
    methodGroup->setLayout(methodLayout);
    mainLayout->addWidget(methodGroup);

    // 支付信息预览
    QGroupBox *previewGroup = new QGroupBox("支付信息");
    QVBoxLayout *previewLayout = new QVBoxLayout();

    QLabel *methodPreview = new QLabel("支付方式: 微信支付");
    QLabel *amountPreview = new QLabel(QString("实付金额: ¥%1").arg(amount, 0, 'f', 2));
    QLabel *timePreview = new QLabel(QString("支付时间: %1").arg(
        QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));

    connect(wechatRadio, &QRadioButton::toggled, [methodPreview](bool checked) {
        if (checked) methodPreview->setText("支付方式: 微信支付");
    });
    connect(alipayRadio, &QRadioButton::toggled, [methodPreview](bool checked) {
        if (checked) methodPreview->setText("支付方式: 支付宝");
    });
    connect(campusCardRadio, &QRadioButton::toggled, [methodPreview](bool checked) {
        if (checked) methodPreview->setText("支付方式: 校园卡支付");
    });

    previewLayout->addWidget(methodPreview);
    previewLayout->addWidget(amountPreview);
    previewLayout->addWidget(timePreview);
    previewGroup->setLayout(previewLayout);
    mainLayout->addWidget(previewGroup);

    // 支付密码
    QWidget *pwdWidget = new QWidget();
    QHBoxLayout *pwdLayout = new QHBoxLayout(pwdWidget);
    pwdLayout->addWidget(new QLabel("支付密码:"));
    QLineEdit *pwdEdit = new QLineEdit();
    pwdEdit->setEchoMode(QLineEdit::Password);
    pwdEdit->setPlaceholderText("请输入6位支付密码");
    pwdEdit->setMaxLength(6);
    pwdLayout->addWidget(pwdEdit, 1);
    mainLayout->addWidget(pwdWidget);

    // 按钮
    QWidget *buttonWidget = new QWidget();
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);

    confirmBtn = new QPushButton("确认支付");
    cancelBtn = new QPushButton("取消");

    confirmBtn->setObjectName("primaryBtn");
    cancelBtn->setObjectName("secondaryBtn");

    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelBtn);
    buttonLayout->addWidget(confirmBtn);

    mainLayout->addWidget(buttonWidget);

    // 连接信号槽
    connect(confirmBtn, &QPushButton::clicked, this, &PaymentDialog::onConfirmPayment);
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
            padding: 8px;
            font-size: 14px;
        }
        QRadioButton::indicator {
            width: 20px;
            height: 20px;
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

void PaymentDialog::onConfirmPayment() {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "确认支付",
        QString("确认支付 ¥%1 吗？").arg(amount, 0, 'f', 2),
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        // 模拟支付处理
        QMessageBox::information(this, "支付成功",
                                 QString("支付成功！\n订单号: %1\n支付金额: ¥%2\n支付时间: %3")
                                     .arg(orderId)
                                     .arg(amount, 0, 'f', 2)
                                     .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
        accept();
    }
}

void PaymentDialog::onPaymentMethodChanged() {
    // 支付方式改变时的处理
}
