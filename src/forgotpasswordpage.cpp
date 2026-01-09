#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QRegularExpression>
#include <QScreen>
#include <QApplication>
#include "ForgotPasswordPage.h"

ForgotPasswordPage::ForgotPasswordPage(QWidget *parent)
    : QDialog(parent), countdownSeconds(60) {
    // 设置窗口属性
    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    setWindowTitle("找回密码");
    setFixedSize(400, 500);

    // 居中显示
    QScreen *screen = QGuiApplication::primaryScreen();
    move(screen->geometry().center() - rect().center());

    setupUI();

    // 初始化定时器
    countdownTimer = new QTimer(this);
    connect(countdownTimer, &QTimer::timeout, this, &ForgotPasswordPage::updateCountdown);
}

void ForgotPasswordPage::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);

    // 标题
    QLabel *titleLabel = new QLabel("找回密码");
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #333;");
    titleLabel->setAlignment(Qt::AlignCenter);

    // 步骤说明
    stepLabel = new QLabel("请输入注册时使用的邮箱");
    stepLabel->setStyleSheet("font-size: 14px; color: #666;");
    stepLabel->setAlignment(Qt::AlignCenter);
    stepLabel->setWordWrap(true);

    // 邮箱输入
    emailEdit = new QLineEdit();
    emailEdit->setPlaceholderText("邮箱地址");
    emailEdit->setStyleSheet(R"(
        QLineEdit {
            border: 1px solid #ddd;
            border-radius: 4px;
            padding: 10px;
            font-size: 14px;
        }
        QLineEdit:focus {
            border-color: #3498db;
            outline: none;
        }
    )");

    // 验证码区域
    QWidget *codeWidget = new QWidget();
    QHBoxLayout *codeLayout = new QHBoxLayout(codeWidget);
    codeLayout->setContentsMargins(0, 0, 0, 0);
    codeLayout->setSpacing(10);

    codeEdit = new QLineEdit();
    codeEdit->setPlaceholderText("验证码");
    codeEdit->setStyleSheet(R"(
        QLineEdit {
            border: 1px solid #ddd;
            border-radius: 4px;
            padding: 10px;
            font-size: 14px;
        }
        QLineEdit:focus {
            border-color: #3498db;
            outline: none;
        }
    )");

    sendCodeBtn = new QPushButton("获取验证码");
    sendCodeBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #3498db;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 10px 15px;
            font-size: 13px;
        }
        QPushButton:hover {
            background-color: #2980b9;
        }
        QPushButton:disabled {
            background-color: #bdc3c7;
        }
    )");

    codeLayout->addWidget(codeEdit, 1);
    codeLayout->addWidget(sendCodeBtn);
    codeWidget->hide(); // 默认隐藏

    // 新密码输入
    newPasswordEdit = new QLineEdit();
    newPasswordEdit->setPlaceholderText("新密码（6-20位字符）");
    newPasswordEdit->setEchoMode(QLineEdit::Password);
    newPasswordEdit->setStyleSheet(R"(
        QLineEdit {
            border: 1px solid #ddd;
            border-radius: 4px;
            padding: 10px;
            font-size: 14px;
        }
        QLineEdit:focus {
            border-color: #3498db;
            outline: none;
        }
    )");
    newPasswordEdit->hide(); // 默认隐藏

    // 确认密码输入
    confirmPasswordEdit = new QLineEdit();
    confirmPasswordEdit->setPlaceholderText("确认新密码");
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    confirmPasswordEdit->setStyleSheet(R"(
        QLineEdit {
            border: 1px solid #ddd;
            border-radius: 4px;
            padding: 10px;
            font-size: 14px;
        }
        QLineEdit:focus {
            border-color: #3498db;
            outline: none;
        }
    )");
    confirmPasswordEdit->hide(); // 默认隐藏

    // 显示密码复选框
    showPasswordCheck = new QCheckBox("显示密码");
    showPasswordCheck->hide(); // 默认隐藏

    // 按钮区域
    QWidget *buttonWidget = new QWidget();
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(10);

    backBtn = new QPushButton("返回");
    backBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #95a5a6;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 10px 20px;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: #7f8c8d;
        }
    )");
    backBtn->hide(); // 默认隐藏

    nextBtn = new QPushButton("下一步");
    nextBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #3498db;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 10px 20px;
            font-size: 14px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #2980b9;
        }
    )");

    resetBtn = new QPushButton("重置密码");
    resetBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #2ecc71;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 10px 20px;
            font-size: 14px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #27ae60;
        }
    )");
    resetBtn->hide(); // 默认隐藏

    buttonLayout->addWidget(backBtn);
    buttonLayout->addStretch();
    buttonLayout->addWidget(nextBtn);
    buttonLayout->addWidget(resetBtn);

    // 添加到主布局
    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacing(20);
    mainLayout->addWidget(stepLabel);
    mainLayout->addSpacing(20);
    mainLayout->addWidget(emailEdit);
    mainLayout->addWidget(codeWidget);
    mainLayout->addWidget(newPasswordEdit);
    mainLayout->addWidget(confirmPasswordEdit);
    mainLayout->addWidget(showPasswordCheck);
    mainLayout->addStretch();
    mainLayout->addWidget(buttonWidget);

    // 连接信号槽
    connect(sendCodeBtn, &QPushButton::clicked, this, &ForgotPasswordPage::onSendCodeClicked);
    connect(backBtn, &QPushButton::clicked, this, &ForgotPasswordPage::onBackClicked);
    connect(nextBtn, &QPushButton::clicked, this, &ForgotPasswordPage::onNextClicked);
    connect(resetBtn, &QPushButton::clicked, this, &ForgotPasswordPage::onResetClicked);
    connect(showPasswordCheck, &QCheckBox::stateChanged, this, &ForgotPasswordPage::onShowPasswordChanged);
}

void ForgotPasswordPage::onSendCodeClicked() {
    QString email = emailEdit->text().trimmed();

    if (email.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入邮箱地址");
        return;
    }

    QRegularExpression emailRegex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    if (!emailRegex.match(email).hasMatch()) {
        QMessageBox::warning(this, "提示", "邮箱格式不正确");
        return;
    }

    // 开始倒计时
    countdownSeconds = 60;
    sendCodeBtn->setEnabled(false);
    sendCodeBtn->setText("60秒后重发");
    countdownTimer->start(1000);

    QMessageBox::information(this, "提示", "验证码已发送到您的邮箱");
}

void ForgotPasswordPage::updateCountdown() {
    countdownSeconds--;
    sendCodeBtn->setText(QString("%1秒后重发").arg(countdownSeconds));

    if (countdownSeconds <= 0) {
        countdownTimer->stop();
        sendCodeBtn->setEnabled(true);
        sendCodeBtn->setText("重新发送");
    }
}

void ForgotPasswordPage::onNextClicked() {
    static int step = 0; // 0:输入邮箱, 1:输入验证码, 2:设置密码

    if (step == 0) {
        // 检查邮箱
        QString email = emailEdit->text().trimmed();
        if (email.isEmpty()) {
            QMessageBox::warning(this, "提示", "请输入邮箱地址");
            return;
        }

        QRegularExpression emailRegex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
        if (!emailRegex.match(email).hasMatch()) {
            QMessageBox::warning(this, "提示", "邮箱格式不正确");
            return;
        }

        // 切换到验证码步骤
        step = 1;
        stepLabel->setText("请输入发送到您邮箱的验证码");
        emailEdit->setEnabled(false);
        codeEdit->parentWidget()->show();
        backBtn->show();
        nextBtn->setText("验证");
    }
    else if (step == 1) {
        // 检查验证码
        QString code = codeEdit->text().trimmed();
        if (code.isEmpty() || code.length() != 6) {
            QMessageBox::warning(this, "提示", "请输入6位验证码");
            return;
        }

        // 简单验证码验证（实际应该从服务器验证）
        if (code != "123456") {
            QMessageBox::warning(this, "提示", "验证码错误");
            return;
        }

        // 切换到设置密码步骤
        step = 2;
        stepLabel->setText("请设置您的新密码");
        codeEdit->parentWidget()->hide();
        newPasswordEdit->show();
        confirmPasswordEdit->show();
        showPasswordCheck->show();
        nextBtn->hide();
        resetBtn->show();
    }
}

void ForgotPasswordPage::onBackClicked() {
    static int step = 1; // 当前步骤

    if (step == 1) {
        // 返回到邮箱步骤
        step = 0;
        stepLabel->setText("请输入注册时使用的邮箱");
        emailEdit->setEnabled(true);
        codeEdit->parentWidget()->hide();
        backBtn->hide();
        nextBtn->setText("下一步");
    }
}

void ForgotPasswordPage::onResetClicked() {
    QString newPassword = newPasswordEdit->text().trimmed();
    QString confirmPassword = confirmPasswordEdit->text().trimmed();

    if (newPassword.isEmpty() || confirmPassword.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入密码");
        return;
    }

    if (newPassword != confirmPassword) {
        QMessageBox::warning(this, "提示", "两次输入的密码不一致");
        return;
    }

    if (newPassword.length() < 6 || newPassword.length() > 20) {
        QMessageBox::warning(this, "提示", "密码长度应为6-20位");
        return;
    }

    // 简单密码强度验证
    bool hasLetter = false;
    bool hasDigit = false;
    for (QChar ch : newPassword) {
        if (ch.isLetter()) hasLetter = true;
        if (ch.isDigit()) hasDigit = true;
    }

    if (!hasLetter || !hasDigit) {
        QMessageBox::warning(this, "提示", "密码需包含字母和数字");
        return;
    }

    // 重置密码成功
    QMessageBox::information(this, "成功", "密码重置成功！\n请使用新密码登录。");
    accept();
}

void ForgotPasswordPage::onShowPasswordChanged(int state) {
    bool visible = (state == Qt::Checked);
    QLineEdit::EchoMode mode = visible ? QLineEdit::Normal : QLineEdit::Password;
    newPasswordEdit->setEchoMode(mode);
    confirmPasswordEdit->setEchoMode(mode);
}
