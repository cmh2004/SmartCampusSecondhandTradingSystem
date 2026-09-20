#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRegularExpression>
#include <QScreen>
#include <QApplication>
#include <QMouseEvent>
#include <QJsonObject>
#include "ForgotPasswordPage.h"
#include "httpclient.h"
#include "commonwidgets.h"

static QPoint g_dragPos;

ForgotPasswordPage::ForgotPasswordPage(QWidget *parent)
    : QDialog(parent), countdownSeconds(60),step(0) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setWindowTitle("找回密码");
    setFixedSize(400, 525);

    setStyleSheet(R"(
        QDialog {
            background-color: transparent;
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
    QWidget *container = new QWidget(this);
    container->setObjectName("container");
    container->setStyleSheet(R"(
        QWidget {
            background-color: white;
            border-radius: 12px;
        }
        #container {
            border: 1px solid #e0e0e0;
        }
    )");
    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->addWidget(container);

    QVBoxLayout *mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(25);

    // 顶部区域：标题 + 自定义关闭按钮
    QWidget *topWidget = new QWidget();
    QHBoxLayout *topLayout = new QHBoxLayout(topWidget);
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->setSpacing(0);

    QLabel *titleLabel = new QLabel("找回密码");
    titleLabel->setStyleSheet(R"(
        QLabel {
            font-size: 24px;
            font-weight: 600;
            color: #2d3748;
            padding-bottom: 8px;
        }
    )");
    titleLabel->setAlignment(Qt::AlignCenter);

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
    stepLabel = new QLabel("请输入注册时使用的账号");
    stepLabel->setStyleSheet(R"(
        QLabel {
            font-size: 15px;
            color: #718096;
            line-height: 1.5;
        }
    )");
    stepLabel->setAlignment(Qt::AlignCenter);
    stepLabel->setWordWrap(true);

    // 账号输入
    usernameEdit = new QLineEdit();
    usernameEdit->setPlaceholderText("账号");
    usernameEdit->setStyleSheet(R"(
        QLineEdit {
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            padding: 8px 15px;
            font-size: 15px;
            color: #2d3748;
            background-color: #f7fafc;
        }
        QLineEdit:focus {
            border-color: #4299e1;
            background-color: white;
            outline: none;
        }
        QLineEdit:disabled {
            background-color: #fafafa;
            color: #94a3b8;
        }
    )");

    emailEdit = new QLineEdit();
    emailEdit->setPlaceholderText("注册邮箱");
    emailEdit->setStyleSheet(R"(
        QLineEdit {
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            padding: 8px 15px;
            font-size: 15px;
            color: #2d3748;
            background-color: #f7fafc;
        }
        QLineEdit:focus {
            border-color: #4299e1;
            background-color: white;
            outline: none;
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
            padding: 8px 15px;
            font-size: 15px;
            color: #2d3748;
            background-color: #f7fafc;
        }
        QLineEdit:focus {
            border-color: #4299e1;
            background-color: white;
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
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            padding: 8px 15px;
            font-size: 15px;
            color: #2d3748;
            background-color: #f7fafc;
        }
        QLineEdit:focus {
            border-color: #4299e1;
            background-color: white;
            outline: none;
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
            image: url(:/icons/img/check.png);
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
    mainLayout->addWidget(topWidget);
    mainLayout->addSpacing(15);
    mainLayout->addWidget(stepLabel);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(usernameEdit);
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

void ForgotPasswordPage::onCloseClicked() {
    this->close();
}

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

void ForgotPasswordPage::onSendCodeClicked()
{
    // 防止倒计时期间重复点击
    if (countdownTimer->isActive()) {
        showMessageBox(this, "提示", QString("请等待 %1 秒后再试").arg(countdownSeconds), QMessageBox::Warning);
        return;
    }

    QString username = usernameEdit->text().trimmed();
    QString email = emailEdit->text().trimmed();

    if (username.isEmpty() || email.isEmpty()) {
        showMessageBox(this, "提示", "请输入账号和注册邮箱", QMessageBox::Warning);
        return;
    }

    // 调用服务端 API
    QJsonObject request;
    request["username"] = username;
    request["email"] = email;
    QJsonObject response = HttpClient::instance()->syncRequest("/api/auth/send_reset_code", request, "POST");

    if (response.value("success").toBool()) {
        QJsonObject data = response.value("data").toObject();
        m_maskedEmail = data.value("masked_email").toString();
        m_tempCode = data.value("code").toString();   // 演示模式：服务端返回验证码

        QString msg = QString("已向您的安全邮箱 %1 发送验证码\n演示验证码：%2\n（实际系统将通过邮件发送）")
                          .arg(m_maskedEmail).arg(m_tempCode);
        showMessageBox(this, "验证码已生成", msg, QMessageBox::Information);

        // 切换到步骤1（输入验证码）
        step = 1;
        stepLabel->setText("请输入验证码");
        usernameEdit->setEnabled(false);   // 禁止修改账号
        emailEdit->setEnabled(false);      // 禁止修改邮箱
        codeEdit->parentWidget()->show();
        backBtn->show();
        nextBtn->setText("验证");
        // 清空验证码输入框
        codeEdit->clear();

        countdownSeconds = 60;
        sendCodeBtn->setEnabled(false);
        sendCodeBtn->setText(QString("%1秒后重发").arg(countdownSeconds));
        countdownTimer->start(1000);   // 每秒触发一次 updateCountdown
    } else {
        showMessageBox(this, "失败", response.value("error").toString(), QMessageBox::Warning);
    }
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

void ForgotPasswordPage::onNextClicked()
{
    if (step == 0) {
        QString username = usernameEdit->text().trimmed();
        QString email = emailEdit->text().trimmed();
        if (username.isEmpty() || email.isEmpty()) {
            showMessageBox(this, "提示", "请输入账号和注册邮箱", QMessageBox::Warning);
            return;
        }

        // 调用服务端验证账号和邮箱是否匹配
        QJsonObject request;
        request["username"] = username;
        request["email"] = email;
        QJsonObject response = HttpClient::instance()->syncRequest("/api/auth/check_account_email", request, "POST");

        if (!response.value("success").toBool()) {
            showMessageBox(this, "验证失败", response.value("error").toString(), QMessageBox::Warning);
            return;
        }

        // 验证通过，进入 step1（输验证码）
        step = 1;
        stepLabel->setText("请输入验证码");
        usernameEdit->setEnabled(false);
        emailEdit->setEnabled(false);
        codeEdit->parentWidget()->show();
        // 显示“获取验证码”按钮
        sendCodeBtn->setEnabled(true);   // 确保按钮启用
        backBtn->show();
        nextBtn->setText("验证");
        codeEdit->clear();
        return;
    }
    if (step == 1) {
        // 验证码输入后点击“验证” -> 进入 step2（设置新密码）
        QString code = codeEdit->text().trimmed();
        if (code.isEmpty()) {
            showMessageBox(this, "提示", "请输入验证码", QMessageBox::Warning);
            return;
        }
        step = 2;
        stepLabel->setText("请设置新密码");
        codeEdit->parentWidget()->hide();
        newPasswordEdit->show();
        confirmPasswordEdit->show();
        showPasswordCheck->show();
        nextBtn->hide();
        backBtn->hide();
        resetBtn->show();
    }
}

void ForgotPasswordPage::onBackClicked() {
    if (step == 1) {
        step = 0;
        stepLabel->setText("请输入注册时使用的账号和邮箱");
        usernameEdit->setEnabled(true);
        emailEdit->setEnabled(true);
        codeEdit->parentWidget()->hide();
        backBtn->hide();
        nextBtn->setText("下一步");
    }
}

void ForgotPasswordPage::onResetClicked()
{
    QString username = usernameEdit->text().trimmed();
    QString code = codeEdit->text().trimmed();
    QString newPassword = newPasswordEdit->text().trimmed();
    QString confirmPassword = confirmPasswordEdit->text().trimmed();

    if (newPassword.isEmpty() || confirmPassword.isEmpty()) {
        showMessageBox(this, "提示", "请输入密码", QMessageBox::Warning);
        return;
    }
    if (newPassword != confirmPassword) {
        showMessageBox(this, "提示", "两次输入的密码不一致", QMessageBox::Warning);
        return;
    }
    // 密码强度校验
    if (newPassword.length() < 6 || newPassword.length() > 20) {
        showMessageBox(this, "提示", "密码长度应为6-20位", QMessageBox::Warning);
        return;
    }

    // 客户端 MD5（与服务端约定一致）
    QString newPasswordMd5 = QCryptographicHash::hash(newPassword.toUtf8(), QCryptographicHash::Md5).toHex();

    QJsonObject request;
    request["username"] = username;
    request["code"] = code;
    request["new_password"] = newPasswordMd5;   // 传递 MD5

    QJsonObject response = HttpClient::instance()->syncRequest("/api/auth/reset_password", request, "POST");
    if (response.value("success").toBool()) {
        showMessageBox(this, "成功", "密码已重置，请重新登录", QMessageBox::Information);
        accept();  // 关闭对话框
    } else {
        showMessageBox(this, "失败", response.value("error").toString(), QMessageBox::Warning);
    }
}

void ForgotPasswordPage::onShowPasswordChanged(int state) {
    bool visible = (state == Qt::Checked);
    QLineEdit::EchoMode mode = visible ? QLineEdit::Normal : QLineEdit::Password;
    newPasswordEdit->setEchoMode(mode);
    confirmPasswordEdit->setEchoMode(mode);
}

void ForgotPasswordPage::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        // 根据当前步骤调用对应的函数
        if (step == 0 || step == 1) {
            onNextClicked();
        } else if (step == 2) {
            onResetClicked();
        }
        event->accept();
        return;
    }
    QDialog::keyPressEvent(event);
}
