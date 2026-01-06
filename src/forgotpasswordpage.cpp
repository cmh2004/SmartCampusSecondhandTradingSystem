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
#include <QPainterPath>
#include <QPushButton>
#include <QScreen>

// 自定义步骤指示器
class StepIndicator : public QWidget {
    Q_OBJECT
public:
    StepIndicator(QWidget *parent = nullptr) : QWidget(parent), currentStep(0) {
        setFixedHeight(80);
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
        int radius = 20;
        int lineHeight = 4;
        int circleY = height() / 2 - 20;

        for (int i = 0; i < steps.size(); ++i) {
            // 绘制步骤之间的连接线
            if (i > 0) {
                int lineStartX = (i - 1) * stepWidth + stepWidth / 2 + radius;
                int lineEndX = i * stepWidth + stepWidth / 2 - radius;

                // 背景线（灰色）
                painter.setPen(QPen(QColor("#e5e7eb"), lineHeight));
                painter.drawLine(lineStartX, circleY, lineEndX, circleY);

                // 进度线（蓝色）
                if (i <= currentStep) {
                    painter.setPen(QPen(QColor("#3b82f6"), lineHeight));
                    painter.drawLine(lineStartX, circleY, lineEndX, circleY);
                }
            }

            // 绘制圆圈
            int circleX = i * stepWidth + stepWidth / 2;

            // 绘制圆圈背景
            if (i < currentStep) {
                // 已完成步骤 - 实心蓝色
                painter.setBrush(QColor("#3b82f6"));
                painter.setPen(Qt::NoPen);
                painter.drawEllipse(QPoint(circleX, circleY), radius, radius);

                // 绘制对勾
                painter.setPen(QPen(Qt::white, 3));
                painter.drawLine(circleX - 8, circleY, circleX - 3, circleY + 8);
                painter.drawLine(circleX - 3, circleY + 8, circleX + 8, circleY - 8);
            } else if (i == currentStep) {
                // 当前步骤 - 蓝色边框，白色填充
                painter.setBrush(Qt::white);
                painter.setPen(QPen(QColor("#3b82f6"), 3));
                painter.drawEllipse(QPoint(circleX, circleY), radius, radius);

                // 绘制步骤编号
                painter.setPen(QColor("#3b82f6"));
                painter.setFont(QFont("Arial", 12, QFont::Bold));
                painter.drawText(QRect(circleX - 10, circleY - 10, 20, 20),
                                 Qt::AlignCenter, QString::number(i + 1));
            } else {
                // 未完成步骤 - 灰色边框
                painter.setBrush(Qt::white);
                painter.setPen(QPen(QColor("#e5e7eb"), 3));
                painter.drawEllipse(QPoint(circleX, circleY), radius, radius);

                // 绘制步骤编号
                painter.setPen(QColor("#9ca3af"));
                painter.setFont(QFont("Arial", 12, QFont::Bold));
                painter.drawText(QRect(circleX - 10, circleY - 10, 20, 20),
                                 Qt::AlignCenter, QString::number(i + 1));
            }

            // 绘制步骤文本
            painter.setPen(i <= currentStep ? QColor("#1f2937") : QColor("#9ca3af"));
            painter.setFont(QFont("Arial", 10, i == currentStep ? QFont::Bold : QFont::Normal));
            painter.drawText(QRect(i * stepWidth, circleY + 35, stepWidth, 30),
                             Qt::AlignCenter, steps[i]);
        }
    }

private:
    QStringList steps;
    int currentStep;
};

ForgotPasswordPage::ForgotPasswordPage(QWidget *parent)
    : QDialog(parent), currentStep(0), countdownSeconds(60) {
    // 设置无边框窗口
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(500, 600);

    // 移动到屏幕中央
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);

    setupUI();
    setupStyles();

    // 初始化定时器
    countdownTimer = new QTimer(this);
    connect(countdownTimer, &QTimer::timeout, this, &ForgotPasswordPage::updateCountdown);
}

void ForgotPasswordPage::setupUI() {
    // 主容器 - 使用布局而不是move
    QWidget *mainContainer = new QWidget(this);
    mainContainer->setObjectName("mainContainer");

    // 添加阴影效果
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(mainContainer);
    shadow->setBlurRadius(25);
    shadow->setColor(QColor(0, 0, 0, 60));
    shadow->setOffset(0, 5);
    mainContainer->setGraphicsEffect(shadow);

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(mainContainer);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 标题栏
    QWidget *titleBar = new QWidget();
    titleBar->setFixedHeight(60);
    titleBar->setObjectName("titleBar");

    QHBoxLayout *titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(20, 0, 20, 0);

    QLabel *titleLabel = new QLabel("找回密码");
    titleLabel->setObjectName("titleLabel");

    closeBtn = new QPushButton("×");
    closeBtn->setFixedSize(40, 40);
    closeBtn->setObjectName("closeBtn");

    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();
    titleLayout->addWidget(closeBtn);

    // 步骤指示器
    stepIndicator = new StepIndicator();

    // 内容区域 - 使用堆栈窗口
    contentStack = new QStackedWidget();
    contentStack->setObjectName("contentStack");

    // 步骤1: 输入邮箱
    QWidget *step1Widget = createStep1Widget();
    // 步骤2: 输入验证码
    QWidget *step2Widget = createStep2Widget();
    // 步骤3: 设置新密码
    QWidget *step3Widget = createStep3Widget();

    contentStack->addWidget(step1Widget);
    contentStack->addWidget(step2Widget);
    contentStack->addWidget(step3Widget);

    // 添加到主布局
    mainLayout->addWidget(titleBar);
    mainLayout->addWidget(stepIndicator);
    mainLayout->addWidget(contentStack);

    // 设置主窗口布局
    QVBoxLayout *windowLayout = new QVBoxLayout(this);
    windowLayout->setContentsMargins(20, 20, 20, 20);
    windowLayout->addWidget(mainContainer);

    // 连接信号槽
    connect(closeBtn, &QPushButton::clicked, this, &ForgotPasswordPage::reject);

    // 回车键快捷键
    QShortcut *enterShortcut = new QShortcut(QKeySequence(Qt::Key_Return), this);
    connect(enterShortcut, &QShortcut::activated, this, &ForgotPasswordPage::onEnterPressed);
}

QWidget* ForgotPasswordPage::createStep1Widget() {
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(30);

    // 图标
    QLabel *iconLabel = new QLabel("📧");
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setStyleSheet("font-size: 48px;");

    // 标题
    QLabel *titleLabel = new QLabel("输入注册邮箱");
    titleLabel->setObjectName("stepTitle");
    titleLabel->setAlignment(Qt::AlignCenter);

    // 描述
    QLabel *descLabel = new QLabel("我们将向该邮箱发送验证码，用于验证您的身份");
    descLabel->setObjectName("stepDesc");
    descLabel->setAlignment(Qt::AlignCenter);
    descLabel->setWordWrap(true);

    // 邮箱输入框
    emailEdit = new QLineEdit();
    emailEdit->setPlaceholderText("请输入邮箱地址");
    emailEdit->setObjectName("stepInput");
    emailEdit->setMinimumHeight(48);

    // 下一步按钮
    nextBtn1 = new QPushButton("发送验证码");
    nextBtn1->setObjectName("primaryBtn");
    nextBtn1->setMinimumHeight(48);

    layout->addWidget(iconLabel);
    layout->addWidget(titleLabel);
    layout->addWidget(descLabel);
    layout->addWidget(emailEdit);
    layout->addStretch();
    layout->addWidget(nextBtn1);

    // 连接信号
    connect(nextBtn1, &QPushButton::clicked, [this]() {
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

        currentStep = 1;
        ((StepIndicator*)stepIndicator)->setSteps({"输入邮箱", "验证身份", "设置密码"}, 1);
        contentStack->setCurrentIndex(1);
        startCountdown();
    });

    return widget;
}

QWidget* ForgotPasswordPage::createStep2Widget() {
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(30);

    // 图标
    QLabel *iconLabel = new QLabel("🔑");
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setStyleSheet("font-size: 48px;");

    // 标题
    QLabel *titleLabel = new QLabel("输入验证码");
    titleLabel->setObjectName("stepTitle");
    titleLabel->setAlignment(Qt::AlignCenter);

    // 邮箱显示
    QLabel *emailDisplay = new QLabel();
    emailDisplay->setObjectName("emailDisplay");
    emailDisplay->setAlignment(Qt::AlignCenter);
    emailDisplay->setText(emailEdit->text().trimmed());

    // 验证码输入框
    QWidget *codeWidget = new QWidget();
    QHBoxLayout *codeLayout = new QHBoxLayout(codeWidget);
    codeLayout->setContentsMargins(0, 0, 0, 0);
    codeLayout->setSpacing(10);

    codeEdit = new QLineEdit();
    codeEdit->setPlaceholderText("请输入6位验证码");
    codeEdit->setObjectName("stepInput");
    codeEdit->setMaxLength(6);
    codeEdit->setAlignment(Qt::AlignCenter);
    codeEdit->setMinimumHeight(48);

    sendCodeBtn = new QPushButton("重新发送");
    sendCodeBtn->setObjectName("secondaryBtn");
    sendCodeBtn->setFixedWidth(120);

    codeLayout->addWidget(codeEdit, 1);
    codeLayout->addWidget(sendCodeBtn);

    // 倒计时标签
    countdownLabel = new QLabel("60秒后可重新发送");
    countdownLabel->setObjectName("countdownLabel");
    countdownLabel->setAlignment(Qt::AlignCenter);

    // 按钮区域
    QWidget *buttonWidget = new QWidget();
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(15);

    QPushButton *backBtn = new QPushButton("上一步");
    backBtn->setObjectName("secondaryBtn");
    backBtn->setMinimumHeight(48);

    nextBtn2 = new QPushButton("下一步");
    nextBtn2->setObjectName("primaryBtn");
    nextBtn2->setMinimumHeight(48);

    buttonLayout->addWidget(backBtn);
    buttonLayout->addWidget(nextBtn2);

    layout->addWidget(iconLabel);
    layout->addWidget(titleLabel);
    layout->addWidget(emailDisplay);
    layout->addWidget(codeWidget);
    layout->addWidget(countdownLabel);
    layout->addStretch();
    layout->addWidget(buttonWidget);

    // 连接信号
    connect(backBtn, &QPushButton::clicked, [this]() {
        currentStep = 0;
        ((StepIndicator*)stepIndicator)->setSteps({"输入邮箱", "验证身份", "设置密码"}, 0);
        contentStack->setCurrentIndex(0);
    });

    connect(sendCodeBtn, &QPushButton::clicked, this, &ForgotPasswordPage::startCountdown);

    connect(nextBtn2, &QPushButton::clicked, [this]() {
        QString code = codeEdit->text().trimmed();
        if (code.isEmpty() || code.length() != 6) {
            QMessageBox::warning(this, "提示", "请输入6位验证码");
            return;
        }

        // 简单验证码验证（这里应该是123456）
        if (code == "123456") {
            currentStep = 2;
            ((StepIndicator*)stepIndicator)->setSteps({"输入邮箱", "验证身份", "设置密码"}, 2);
            contentStack->setCurrentIndex(2);
        } else {
            QMessageBox::warning(this, "验证失败", "验证码错误，请重新输入");
        }
    });

    return widget;
}

QWidget* ForgotPasswordPage::createStep3Widget() {
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(30);

    // 图标
    QLabel *iconLabel = new QLabel("🔒");
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setStyleSheet("font-size: 48px;");

    // 标题
    QLabel *titleLabel = new QLabel("设置新密码");
    titleLabel->setObjectName("stepTitle");
    titleLabel->setAlignment(Qt::AlignCenter);

    // 新密码输入框
    newPasswordEdit = new QLineEdit();
    newPasswordEdit->setPlaceholderText("新密码（6-20位字符）");
    newPasswordEdit->setEchoMode(QLineEdit::Password);
    newPasswordEdit->setObjectName("stepInput");
    newPasswordEdit->setMinimumHeight(48);

    // 确认密码输入框
    confirmPasswordEdit = new QLineEdit();
    confirmPasswordEdit->setPlaceholderText("确认新密码");
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    confirmPasswordEdit->setObjectName("stepInput");
    confirmPasswordEdit->setMinimumHeight(48);

    // 显示密码复选框
    togglePwdBtn = new QCheckBox("显示密码");
    togglePwdBtn->setObjectName("togglePwdBtn");

    // 密码规则提示
    QLabel *rulesLabel = new QLabel("• 长度6-20位字符\n• 包含字母和数字\n• 不能与旧密码相同");
    rulesLabel->setObjectName("rulesLabel");

    // 重置按钮
    resetBtn = new QPushButton("重置密码");
    resetBtn->setObjectName("primaryBtn");
    resetBtn->setMinimumHeight(48);

    layout->addWidget(iconLabel);
    layout->addWidget(titleLabel);
    layout->addWidget(newPasswordEdit);
    layout->addWidget(confirmPasswordEdit);
    layout->addWidget(togglePwdBtn);
    layout->addWidget(rulesLabel);
    layout->addStretch();
    layout->addWidget(resetBtn);

    // 连接信号
    connect(togglePwdBtn, &QCheckBox::stateChanged, [this](int state) {
        bool visible = (state == Qt::Checked);
        QLineEdit::EchoMode mode = visible ? QLineEdit::Normal : QLineEdit::Password;
        newPasswordEdit->setEchoMode(mode);
        confirmPasswordEdit->setEchoMode(mode);
    });

    connect(resetBtn, &QPushButton::clicked, this, &ForgotPasswordPage::onResetPasswordClicked);

    return widget;
}

void ForgotPasswordPage::setupStyles() {
    QString styleSheet = R"(
        /* 主容器 */
        #mainContainer {
            background-color: white;
            border-radius: 16px;
        }

        /* 标题栏 */
        #titleBar {
            background-color: white;
            border-top-left-radius: 16px;
            border-top-right-radius: 16px;
            border-bottom: 1px solid #f3f4f6;
        }

        #titleLabel {
            font-size: 20px;
            font-weight: 600;
            color: #1f2937;
        }

        #closeBtn {
            color: #9ca3af;
            font-size: 24px;
            font-weight: 300;
            border: none;
            background: transparent;
            border-radius: 8px;
        }

        #closeBtn:hover {
            background-color: #f3f4f6;
            color: #6b7280;
        }

        #closeBtn:pressed {
            background-color: #e5e7eb;
            color: #4b5563;
        }

        /* 步骤标题 */
        #stepTitle {
            font-size: 22px;
            font-weight: 600;
            color: #1f2937;
        }

        #stepDesc {
            font-size: 14px;
            color: #6b7280;
            line-height: 1.5;
        }

        /* 输入框 */
        #stepInput {
            background-color: #f9fafb;
            border: 2px solid #e5e7eb;
            border-radius: 12px;
            padding: 12px 16px;
            font-size: 16px;
            color: #1f2937;
        }

        #stepInput:focus {
            border-color: #3b82f6;
            background-color: white;
            outline: none;
        }

        #stepInput::placeholder {
            color: #9ca3af;
        }

        /* 按钮 */
        #primaryBtn {
            background: linear-gradient(135deg, #3b82f6 0%, #1d4ed8 100%);
            color: white;
            border-radius: 12px;
            font-size: 16px;
            font-weight: 600;
            border: none;
        }

        #primaryBtn:hover {
            background: linear-gradient(135deg, #2563eb 0%, #1e40af 100%);
        }

        #primaryBtn:pressed {
            background: linear-gradient(135deg, #1d4ed8 0%, #1e3a8a 100%);
        }

        #secondaryBtn {
            background-color: white;
            color: #374151;
            border-radius: 12px;
            border: 2px solid #e5e7eb;
            font-size: 16px;
            font-weight: 600;
        }

        #secondaryBtn:hover {
            background-color: #f9fafb;
            border-color: #d1d5db;
        }

        #secondaryBtn:pressed {
            background-color: #f3f4f6;
            border-color: #9ca3af;
        }

        /* 倒计时标签 */
        #countdownLabel {
            font-size: 14px;
            color: #6b7280;
        }

        /* 邮箱显示 */
        #emailDisplay {
            font-size: 14px;
            color: #3b82f6;
            background-color: #eff6ff;
            padding: 8px 16px;
            border-radius: 8px;
            border: 1px solid #dbeafe;
        }

        /* 密码规则 */
        #rulesLabel {
            font-size: 14px;
            color: #6b7280;
            line-height: 1.8;
            padding: 12px 16px;
            background-color: #f9fafb;
            border-radius: 8px;
            border: 1px solid #e5e7eb;
        }

        /* 显示密码复选框 */
        #togglePwdBtn {
            color: #6b7280;
            font-size: 14px;
            spacing: 8px;
        }

        #togglePwdBtn::indicator {
            width: 20px;
            height: 20px;
        }

        #togglePwdBtn::indicator:unchecked {
            border: 2px solid #d1d5db;
            border-radius: 6px;
            background-color: white;
        }

        #togglePwdBtn::indicator:checked {
            border: 2px solid #3b82f6;
            border-radius: 6px;
            background-color: #3b82f6;
            image: url(:/icons/check.svg);
        }

        /* 堆栈窗口 */
        #contentStack {
            background-color: white;
            border-radius: 0 0 16px 16px;
        }
    )";

    setStyleSheet(styleSheet);
}

void ForgotPasswordPage::startCountdown() {
    countdownSeconds = 60;
    sendCodeBtn->setEnabled(false);
    sendCodeBtn->setText("60秒后重发");
    countdownTimer->start(1000);
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

void ForgotPasswordPage::onEnterPressed() {
    int currentIndex = contentStack->currentIndex();
    if (currentIndex == 0 && nextBtn1) {
        nextBtn1->click();
    } else if (currentIndex == 1 && nextBtn2) {
        nextBtn2->click();
    } else if (currentIndex == 2 && resetBtn) {
        resetBtn->click();
    }
}

void ForgotPasswordPage::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
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

#include "ForgotPasswordPage.moc"
