#include "ForgotPasswordPage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QMouseEvent>
#include <QShortcut>
#include <QTimer>
#include <QMessageBox>
#include <QStackedWidget>
#include <QRegularExpression>

// 自定义步骤指示器
class StepIndicator : public QWidget {
public:
    StepIndicator(QWidget *parent = nullptr) : QWidget(parent) {
        setFixedHeight(60);
    }

    void setSteps(const QStringList &steps, int currentStep) {
        this->steps = steps;
        this->currentStep = currentStep;
        update();
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        int stepWidth = width() / steps.size();
        int radius = 15;

        for (int i = 0; i < steps.size(); ++i) {
            // 绘制连接线
            if (i > 0) {
                painter.setPen(QPen(QColor("#e4e6eb"), 2));
                painter.drawLine((i - 0.5) * stepWidth + radius, height() / 2,
                                 i * stepWidth - radius, height() / 2);
            }

            // 绘制圆点
            QColor circleColor;
            if (i < currentStep) {
                circleColor = QColor("#1e90ff"); // 已完成
            } else if (i == currentStep) {
                circleColor = QColor("#1e90ff"); // 当前步骤
            } else {
                circleColor = QColor("#e4e6eb"); // 未完成
            }

            painter.setBrush(circleColor);
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(QPointF(i * stepWidth + stepWidth / 2, height() / 2), radius, radius);

            // 绘制步骤文本
            painter.setPen(i <= currentStep ? QColor("#333") : QColor("#999"));
            painter.setFont(QFont("Arial", 10, i == currentStep ? QFont::Bold : QFont::Normal));
            painter.drawText(QRect(i * stepWidth, height() / 2 + 20, stepWidth, 30),
                             Qt::AlignCenter, steps[i]);
        }
    }

private:
    QStringList steps;
    int currentStep;
};

ForgotPasswordPage::ForgotPasswordPage(QWidget *parent)
    : QDialog(parent), isDragging(false), currentStep(1), countdownSeconds(60) {
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(450, 550);

    setupUI();
    setupStyles();

    // 初始化定时器
    countdownTimer = new QTimer(this);
    connect(countdownTimer, &QTimer::timeout, this, &ForgotPasswordPage::updateCountdown);
}

void ForgotPasswordPage::setupUI() {
    // 主容器
    QWidget *mainContainer = new QWidget(this);
    mainContainer->setObjectName("mainContainer");
    mainContainer->setFixedSize(450, 550);

    // 添加阴影效果
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(mainContainer);
    shadow->setBlurRadius(25);
    shadow->setColor(QColor(0, 0, 0, 60));
    shadow->setOffset(0, 5);
    mainContainer->setGraphicsEffect(shadow);

    // 标题栏
    QWidget *titleBar = new QWidget(mainContainer);
    titleBar->setFixedHeight(50);
    titleBar->setObjectName("titleBar");

    QLabel *titleLabel = new QLabel("找回密码", titleBar);
    titleLabel->setStyleSheet("color: #333; font-weight: bold; font-size: 18px; margin-left: 25px;");

    closeBtn = new QPushButton("×", titleBar);
    closeBtn->setFixedSize(50, 50);
    closeBtn->setObjectName("closeBtn");

    QHBoxLayout *titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();
    titleLayout->addWidget(closeBtn);

    // 步骤指示器
    stepIndicator = new StepIndicator(mainContainer);
    stepIndicator->move(0, 50);
    stepIndicator->setFixedWidth(450);
    ((StepIndicator*)stepIndicator)->setSteps({"输入邮箱", "验证身份", "设置新密码"}, 0);

    // 内容区域
    QStackedWidget *contentStack = new QStackedWidget(mainContainer);
    contentStack->move(0, 110);
    contentStack->setFixedSize(450, 390);

    // 步骤1: 输入邮箱
    QWidget *step1Widget = new QWidget();
    QVBoxLayout *step1Layout = new QVBoxLayout(step1Widget);
    step1Layout->setContentsMargins(50, 30, 50, 30);
    step1Layout->setSpacing(20);

    QLabel *step1Title = new QLabel("请输入注册时使用的邮箱");
    step1Title->setStyleSheet("font-size: 16px; color: #333; font-weight: 500;");

    emailEdit = new QLineEdit();
    emailEdit->setPlaceholderText("请输入邮箱地址");
    emailEdit->setObjectName("stepInput");

    QLabel *hintLabel1 = new QLabel("我们将向该邮箱发送验证码");
    hintLabel1->setStyleSheet("color: #999; font-size: 12px;");

    QPushButton *nextBtn1 = new QPushButton("下一步");
    nextBtn1->setObjectName("primaryBtn");
    nextBtn1->setFixedHeight(45);

    step1Layout->addStretch();
    step1Layout->addWidget(step1Title, 0, Qt::AlignCenter);
    step1Layout->addWidget(emailEdit);
    step1Layout->addWidget(hintLabel1, 0, Qt::AlignCenter);
    step1Layout->addStretch();
    step1Layout->addWidget(nextBtn1);

    // 步骤2: 输入验证码
    QWidget *step2Widget = new QWidget();
    QVBoxLayout *step2Layout = new QVBoxLayout(step2Widget);
    step2Layout->setContentsMargins(50, 30, 50, 30);
    step2Layout->setSpacing(20);

    QLabel *step2Title = new QLabel("输入验证码");
    step2Title->setStyleSheet("font-size: 16px; color: #333; font-weight: 500;");

    QWidget *codeWidget = new QWidget();
    QHBoxLayout *codeLayout = new QHBoxLayout(codeWidget);
    codeLayout->setContentsMargins(0, 0, 0, 0);
    codeLayout->setSpacing(10);

    codeEdit = new QLineEdit();
    codeEdit->setPlaceholderText("6位验证码");
    codeEdit->setObjectName("stepInput");
    codeEdit->setMaxLength(6);

    sendCodeBtn = new QPushButton("发送验证码");
    sendCodeBtn->setObjectName("secondaryBtn");
    sendCodeBtn->setFixedWidth(120);

    codeLayout->addWidget(codeEdit, 1);
    codeLayout->addWidget(sendCodeBtn);

    countdownLabel = new QLabel();
    countdownLabel->setStyleSheet("color: #999; font-size: 12px;");
    countdownLabel->hide();

    QLabel *hintLabel2 = new QLabel("验证码已发送至您的邮箱，有效期5分钟");
    hintLabel2->setStyleSheet("color: #999; font-size: 12px;");

    QWidget *buttonWidget2 = new QWidget();
    QHBoxLayout *buttonLayout2 = new QHBoxLayout(buttonWidget2);
    buttonLayout2->setContentsMargins(0, 0, 0, 0);

    backBtn = new QPushButton("上一步");
    backBtn->setObjectName("secondaryBtn");

    QPushButton *nextBtn2 = new QPushButton("下一步");
    nextBtn2->setObjectName("primaryBtn");

    buttonLayout2->addWidget(backBtn);
    buttonLayout2->addWidget(nextBtn2);

    step2Layout->addStretch();
    step2Layout->addWidget(step2Title, 0, Qt::AlignCenter);
    step2Layout->addWidget(codeWidget);
    step2Layout->addWidget(countdownLabel, 0, Qt::AlignCenter);
    step2Layout->addWidget(hintLabel2, 0, Qt::AlignCenter);
    step2Layout->addStretch();
    step2Layout->addWidget(buttonWidget2);

    // 步骤3: 设置新密码
    QWidget *step3Widget = new QWidget();
    QVBoxLayout *step3Layout = new QVBoxLayout(step3Widget);
    step3Layout->setContentsMargins(50, 30, 50, 30);
    step3Layout->setSpacing(20);

    QLabel *step3Title = new QLabel("设置新密码");
    step3Title->setStyleSheet("font-size: 16px; color: #333; font-weight: 500;");

    newPasswordEdit = new QLineEdit();
    newPasswordEdit->setPlaceholderText("新密码 (6-20位字符)");
    newPasswordEdit->setEchoMode(QLineEdit::Password);
    newPasswordEdit->setObjectName("stepInput");

    confirmPasswordEdit = new QLineEdit();
    confirmPasswordEdit->setPlaceholderText("确认新密码");
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    confirmPasswordEdit->setObjectName("stepInput");

    togglePwdBtn = new QCheckBox("显示密码");
    togglePwdBtn->setObjectName("togglePwdBtn");

    QLabel *passwordRules = new QLabel("密码需包含字母和数字，长度6-20位");
    passwordRules->setStyleSheet("color: #999; font-size: 12px;");

    resetBtn = new QPushButton("重置密码");
    resetBtn->setObjectName("primaryBtn");
    resetBtn->setFixedHeight(45);

    step3Layout->addStretch();
    step3Layout->addWidget(step3Title, 0, Qt::AlignCenter);
    step3Layout->addWidget(newPasswordEdit);
    step3Layout->addWidget(confirmPasswordEdit);
    step3Layout->addWidget(togglePwdBtn, 0, Qt::AlignCenter);
    step3Layout->addWidget(passwordRules, 0, Qt::AlignCenter);
    step3Layout->addStretch();
    step3Layout->addWidget(resetBtn);

    // 添加到堆栈
    contentStack->addWidget(step1Widget);
    contentStack->addWidget(step2Widget);
    contentStack->addWidget(step3Widget);
    contentStack->setCurrentIndex(0);

    // 连接信号槽
    connect(closeBtn, &QPushButton::clicked, this, &ForgotPasswordPage::reject);
    connect(sendCodeBtn, &QPushButton::clicked, this, &ForgotPasswordPage::onSendCodeClicked);
    connect(resetBtn, &QPushButton::clicked, this, &ForgotPasswordPage::onResetPasswordClicked);
    connect(backBtn, &QPushButton::clicked, this, &ForgotPasswordPage::onBackToLoginClicked);
    connect(nextBtn1, &QPushButton::clicked, [this, contentStack]() {
        QString email = emailEdit->text().trimmed();
        if (email.isEmpty()) {
            QMessageBox::warning(this, "提示", "请输入邮箱地址");
            return;
        }

        // 简单邮箱格式验证
        QRegularExpression emailRegex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
        if (!emailRegex.match(email).hasMatch()) {
            QMessageBox::warning(this, "提示", "邮箱格式不正确");
            return;
        }

        // 模拟发送验证码
        QMessageBox::information(this, "验证码已发送",
                                 "验证码已发送至您的邮箱\n请注意查收");

        currentStep = 2;
        ((StepIndicator*)stepIndicator)->setSteps({"输入邮箱", "验证身份", "设置新密码"}, 1);
        contentStack->setCurrentIndex(1);
        startCountdown();
    });

    connect(nextBtn2, &QPushButton::clicked, [this, contentStack]() {
        QString code = codeEdit->text().trimmed();
        if (code.isEmpty() || code.length() != 6) {
            QMessageBox::warning(this, "提示", "请输入6位验证码");
            return;
        }

        // 这里可以添加验证码验证逻辑
        // 暂时跳过验证

        currentStep = 3;
        ((StepIndicator*)stepIndicator)->setSteps({"输入邮箱", "验证身份", "设置新密码"}, 2);
        contentStack->setCurrentIndex(2);
    });

    connect(togglePwdBtn, &QCheckBox::stateChanged, [this](int state) {
        bool visible = (state == Qt::Checked);
        QLineEdit::EchoMode mode = visible ? QLineEdit::Normal : QLineEdit::Password;
        newPasswordEdit->setEchoMode(mode);
        confirmPasswordEdit->setEchoMode(mode);
    });

    // 回车键快捷键
    QShortcut *enterShortcut = new QShortcut(QKeySequence(Qt::Key_Return), this);
    connect(enterShortcut, &QShortcut::activated, [this, contentStack]() {
        if (contentStack->currentIndex() == 0) {
            Q_EMIT nextBtn1->clicked();
        } else if (contentStack->currentIndex() == 1) {
            Q_EMIT nextBtn2->clicked();
        } else if (contentStack->currentIndex() == 2) {
            Q_EMIT resetBtn->clicked();
        }
    });

    // 窗口布局
    QVBoxLayout *windowLayout = new QVBoxLayout(this);
    windowLayout->setContentsMargins(0, 0, 0, 0);
    windowLayout->addWidget(mainContainer);
}

void ForgotPasswordPage::setupStyles() {
    QString styleSheet = R"(
        #mainContainer {
            background-color: white;
            border-radius: 12px;
        }

        #titleBar {
            background-color: white;
            border-top-left-radius: 12px;
            border-top-right-radius: 12px;
            border-bottom: 1px solid #f0f0f0;
        }

        #closeBtn {
            color: #999;
            font-size: 20px;
            font-weight: normal;
            border: none;
            background: transparent;
            border-radius: 0;
        }

        #closeBtn:hover {
            background-color: #f5f5f5;
            color: #333;
        }

        #closeBtn:pressed {
            background-color: #e81123;
            color: white;
        }

        #stepInput {
            background-color: #f8f9fa;
            border-radius: 8px;
            border: 1px solid #e4e6eb;
            padding: 12px 15px;
            font-size: 14px;
        }

        #stepInput:focus {
            border-color: #1e90ff;
            background-color: #f0f8ff;
        }

        #primaryBtn {
            background: linear-gradient(135deg, #1e90ff 0%, #00bfff 100%);
            color: white;
            border-radius: 8px;
            font-size: 16px;
            font-weight: bold;
            border: none;
        }

        #primaryBtn:hover {
            background: linear-gradient(135deg, #187bdd 0%, #00a8e8 100%);
        }

        #primaryBtn:pressed {
            background: linear-gradient(135deg, #0f6bcc 0%, #0095d3 100%);
        }

        #secondaryBtn {
            background-color: #f8f9fa;
            color: #333;
            border-radius: 6px;
            border: 1px solid #e4e6eb;
            padding: 8px 16px;
            font-size: 14px;
        }

        #secondaryBtn:hover {
            background-color: #f0f0f0;
            border-color: #d0d0d0;
        }

        #togglePwdBtn {
            color: #666;
            font-size: 13px;
            spacing: 5px;
        }

        #togglePwdBtn::indicator {
            width: 16px;
            height: 16px;
        }

        #togglePwdBtn::indicator:unchecked {
            border: 1px solid #ddd;
            border-radius: 3px;
            background-color: white;
        }

        #togglePwdBtn::indicator:checked {
            border: 1px solid #1e90ff;
            border-radius: 3px;
            background-color: #1e90ff;
        }

        /* 步骤指示器样式 */
        StepIndicator {
            background-color: white;
        }
    )";

    setStyleSheet(styleSheet);
}

void ForgotPasswordPage::onSendCodeClicked() {
    startCountdown();
}

void ForgotPasswordPage::startCountdown() {
    countdownSeconds = 60;
    sendCodeBtn->setEnabled(false);
    sendCodeBtn->setText("60秒后重发");
    countdownLabel->setText("验证码已发送，60秒后可重新获取");
    countdownLabel->show();
    countdownTimer->start(1000);
}

void ForgotPasswordPage::updateCountdown() {
    countdownSeconds--;
    sendCodeBtn->setText(QString("%1秒后重发").arg(countdownSeconds));

    if (countdownSeconds <= 0) {
        countdownTimer->stop();
        sendCodeBtn->setEnabled(true);
        sendCodeBtn->setText("重新发送");
        countdownLabel->hide();
    }
}

void ForgotPasswordPage::onResetPasswordClicked() {
    QString newPassword = newPasswordEdit->text().trimmed();
    QString confirmPassword = confirmPasswordEdit->text().trimmed();

    if (newPassword.isEmpty() || confirmPassword.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入新密码");
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

    // 模拟重置密码成功
    QMessageBox::information(this, "成功", "密码重置成功！\n请使用新密码登录。");
    accept();
}

void ForgotPasswordPage::onBackToLoginClicked() {
    reject();
}

void ForgotPasswordPage::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton &&
        event->pos().y() < 50) {  // 只有标题栏区域可拖动
        isDragging = true;
        dragStartPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }
}

void ForgotPasswordPage::mouseMoveEvent(QMouseEvent *event) {
    if (isDragging && event->buttons() & Qt::LeftButton) {
        move(event->globalPosition().toPoint() - dragStartPosition);
        event->accept();
    }
}

void ForgotPasswordPage::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
        event->accept();
    }
}
