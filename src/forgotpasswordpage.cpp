#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QRegularExpression>
#include <QScreen>
#include <QApplication>
#include <QMouseEvent> // 新增：用于无边框窗口拖动
#include "ForgotPasswordPage.h"

// 新增：用于无边框窗口拖动的变量
static QPoint g_dragPos;

ForgotPasswordPage::ForgotPasswordPage(QWidget *parent)
    : QDialog(parent), countdownSeconds(60) {
    // 关键修改1：去掉默认标题栏，自定义关闭按钮
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setWindowTitle("找回密码");
    setFixedSize(450, 550);

    // 优化窗口样式：去掉黑框，优化圆角透明
    setStyleSheet(R"(
        QDialog {
            background-color: transparent; /* 完全透明，避免黑框 */
        }
    )");
    setAttribute(Qt::WA_TranslucentBackground);

    // 居中显示
    QScreen *screen = QGuiApplication::primaryScreen();
    move(screen->geometry().center() - rect().center());

    setupUI();

    // 初始化定时器
    countdownTimer = new QTimer(this);
    connect(countdownTimer, &QTimer::timeout, this, &ForgotPasswordPage::updateCountdown);
}

void ForgotPasswordPage::setupUI() {
    // 外层容器：核心容器，解决黑框问题，优化阴影和圆角
    QWidget *container = new QWidget(this);
    container->setStyleSheet(R"(
        QWidget {
            background-color: white;
            border-radius: 12px;
            box-shadow: 0 4px 20px rgba(0, 0, 0, 0.08);
        }
    )");
    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0); // 关键：去掉外层margin，避免黑框
    outerLayout->addWidget(container);

    QVBoxLayout *mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(40, 40, 40, 40);
    mainLayout->setSpacing(25);

    // 顶部区域：标题 + 自定义关闭按钮
    QWidget *topWidget = new QWidget();
    QHBoxLayout *topLayout = new QHBoxLayout(topWidget);
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->setSpacing(0);

    // 标题：关键修改2：去掉底部边框（标题框），优化样式
    QLabel *titleLabel = new QLabel("找回密码");
    titleLabel->setStyleSheet(R"(
        QLabel {
            font-size: 24px;
            font-weight: 600;
            color: #2d3748;
            padding-bottom: 8px;
            /* 移除border-bottom，去掉标题框 */
        }
    )");
    titleLabel->setAlignment(Qt::AlignCenter);

    // 自定义关闭按钮：关键修改3：放到标题栏下方区域右上角
    closeBtn = new QPushButton("×");
    closeBtn->setStyleSheet(R"(
        QPushButton {
            background-color: transparent;
            color: #718096;
            border: none;
            font-size: 20px;
            width: 30px;
            height: 30px;
            border-radius: 15px;
        }
        QPushButton:hover {
            background-color: #f7fafc;
            color: #2d3748;
        }
        QPushButton:pressed {
            background-color: #e2e8f0;
        }
    )");
    connect(closeBtn, &QPushButton::clicked, this, &ForgotPasswordPage::onCloseClicked);

    // 顶部布局：标题居中，关闭按钮靠右
    topLayout->addStretch();
    topLayout->addWidget(titleLabel);
    topLayout->addStretch();
    topLayout->addWidget(closeBtn);

    // 步骤说明
    stepLabel = new QLabel("请输入注册时使用的邮箱");
    stepLabel->setStyleSheet(R"(
        QLabel {
            font-size: 15px;
            color: #718096;
            line-height: 1.5;
        }
    )");
    stepLabel->setAlignment(Qt::AlignCenter);
    stepLabel->setWordWrap(true);

    // 邮箱输入
    emailEdit = new QLineEdit();
    emailEdit->setPlaceholderText("邮箱地址");
    emailEdit->setStyleSheet(R"(
        QLineEdit {
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            padding: 12px 15px;
            font-size: 15px;
            color: #2d3748;
            background-color: #f7fafc;
        }
        QLineEdit:focus {
            border-color: #4299e1;
            background-color: white;
            outline: none;
            box-shadow: 0 0 0 3px rgba(66, 153, 225, 0.1);
        }
        QLineEdit:disabled {
            background-color: #fafafa;
            color: #94a3b8;
        }
    )");

    // 验证码区域
    QWidget *codeWidget = new QWidget();
    QHBoxLayout *codeLayout = new QHBoxLayout(codeWidget);
    codeLayout->setContentsMargins(0, 0, 0, 0);
    codeLayout->setSpacing(12);

    codeEdit = new QLineEdit();
    codeEdit->setPlaceholderText("验证码（6位）");
    codeEdit->setStyleSheet(R"(
        QLineEdit {
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            padding: 12px 15px;
            font-size: 15px;
            color: #2d3748;
            background-color: #f7fafc;
        }
        QLineEdit:focus {
            border-color: #4299e1;
            background-color: white;
            outline: none;
            box-shadow: 0 0 0 3px rgba(66, 153, 225, 0.1);
        }
    )");

    sendCodeBtn = new QPushButton("获取验证码");
    sendCodeBtn->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #4299e1, stop:1 #38b2ac);
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px 20px;
            font-size: 14px;
            font-weight: 500;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #3182ce, stop:1 #319795);
        }
        QPushButton:disabled {
            background: #e2e8f0;
            color: #a0aec0;
        }
        QPushButton:pressed {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #2b6cb0, stop:1 #2c7a7b);
        }
    )");

    codeLayout->addWidget(codeEdit, 1);
    codeLayout->addWidget(sendCodeBtn);
    codeWidget->hide(); // 默认隐藏

    // 新密码输入
    newPasswordEdit = new QLineEdit();
    newPasswordEdit->setPlaceholderText("新密码（6-20位，含字母+数字）");
    newPasswordEdit->setEchoMode(QLineEdit::Password);
    newPasswordEdit->setStyleSheet(R"(
        QLineEdit {
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            padding: 12px 15px;
            font-size: 15px;
            color: #2d3748;
            background-color: #f7fafc;
        }
        QLineEdit:focus {
            border-color: #4299e1;
            background-color: white;
            outline: none;
            box-shadow: 0 0 0 3px rgba(66, 153, 225, 0.1);
        }
    )");
    newPasswordEdit->hide(); // 默认隐藏

    // 确认密码输入
    confirmPasswordEdit = new QLineEdit();
    confirmPasswordEdit->setPlaceholderText("确认新密码");
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    confirmPasswordEdit->setStyleSheet(R"(
        QLineEdit {
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            padding: 12px 15px;
            font-size: 15px;
            color: #2d3748;
            background-color: #f7fafc;
        }
        QLineEdit:focus {
            border-color: #4299e1;
            background-color: white;
            outline: none;
            box-shadow: 0 0 0 3px rgba(66, 153, 225, 0.1);
        }
    )");
    confirmPasswordEdit->hide(); // 默认隐藏

    // 显示密码复选框
    showPasswordCheck = new QCheckBox("显示密码");
    showPasswordCheck->setStyleSheet(R"(
        QCheckBox {
            font-size: 14px;
            color: #4a5568;
            spacing: 8px;
        }
        QCheckBox::indicator {
            width: 18px;
            height: 18px;
            border-radius: 4px;
            border: 1px solid #cbd5e0;
            background-color: #f7fafc;
        }
        QCheckBox::indicator:checked {
            background-color: #4299e1;
            border-color: #4299e1;
            image: url(:/icons/check.png); /* 可选：添加对勾图标，需自行准备 */
        }
        QCheckBox::indicator:hover {
            border-color: #94a3b8;
        }
    )");
    showPasswordCheck->hide(); // 默认隐藏

    // 按钮区域
    QWidget *buttonWidget = new QWidget();
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(15);

    backBtn = new QPushButton("返回");
    backBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #e2e8f0;
            color: #4a5568;
            border: none;
            border-radius: 8px;
            padding: 12px 25px;
            font-size: 15px;
            font-weight: 500;
        }
        QPushButton:hover {
            background-color: #cbd5e0;
        }
        QPushButton:pressed {
            background-color: #a0aec0;
        }
    )");
    backBtn->hide(); // 默认隐藏

    nextBtn = new QPushButton("下一步");
    nextBtn->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #4299e1, stop:1 #38b2ac);
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px 30px;
            font-size: 15px;
            font-weight: 600;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #3182ce, stop:1 #319795);
        }
        QPushButton:pressed {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #2b6cb0, stop:1 #2c7a7b);
        }
    )");

    resetBtn = new QPushButton("重置密码");
    resetBtn->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #48bb78, stop:1 #38b2ac);
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px 30px;
            font-size: 15px;
            font-weight: 600;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #38a169, stop:1 #319795);
        }
        QPushButton:pressed {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #2f855a, stop:1 #2c7a7b);
        }
    )");
    resetBtn->hide(); // 默认隐藏

    buttonLayout->addWidget(backBtn);
    buttonLayout->addStretch();
    buttonLayout->addWidget(nextBtn);
    buttonLayout->addWidget(resetBtn);

    // 添加到主布局
    mainLayout->addWidget(topWidget); // 替换原标题Label，使用包含关闭按钮的顶部布局
    mainLayout->addSpacing(15);
    mainLayout->addWidget(stepLabel);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(emailEdit);
    mainLayout->addWidget(codeWidget);
    mainLayout->addWidget(newPasswordEdit);
    mainLayout->addWidget(confirmPasswordEdit);
    mainLayout->addWidget(showPasswordCheck, 0, Qt::AlignRight); // 右对齐
    mainLayout->addStretch();
    mainLayout->addWidget(buttonWidget);

    // 连接信号槽
    connect(sendCodeBtn, &QPushButton::clicked, this, &ForgotPasswordPage::onSendCodeClicked);
    connect(backBtn, &QPushButton::clicked, this, &ForgotPasswordPage::onBackClicked);
    connect(nextBtn, &QPushButton::clicked, this, &ForgotPasswordPage::onNextClicked);
    connect(resetBtn, &QPushButton::clicked, this, &ForgotPasswordPage::onResetClicked);
    connect(showPasswordCheck, &QCheckBox::stateChanged, this, &ForgotPasswordPage::onShowPasswordChanged);
}

// 新增：自定义关闭按钮槽函数
void ForgotPasswordPage::onCloseClicked() {
    this->close();
}

// 新增：实现无边框窗口拖动（可选，提升体验）
void ForgotPasswordPage::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        g_dragPos = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void ForgotPasswordPage::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPos() - g_dragPos);
        event->accept();
    }
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
        stepLabel->setText("请输入发送到您邮箱的6位验证码");
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
        stepLabel->setText("请设置您的新密码（6-20位，含字母+数字）");
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
