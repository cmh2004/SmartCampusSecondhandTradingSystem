#include "RegisterPage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QMouseEvent>
#include <QShortcut>
#include <QTimer>
#include <QRegularExpression>

RegisterPage::RegisterPage(QWidget *parent) : QDialog(parent), isDragging(false) {
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(420, 720);

    setupUI();
    setupStyles();

    // 连接信号槽
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(toLoginBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(registerBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(togglePwdBtn, &QCheckBox::stateChanged, this, &RegisterPage::onTogglePassword);

    // 回车键注册快捷键
    QShortcut *enterShortcut = new QShortcut(QKeySequence(Qt::Key_Return), this);
    connect(enterShortcut, &QShortcut::activated, this, &QDialog::accept);
}

void RegisterPage::setupUI() {
    // 主容器
    QWidget *mainContainer = new QWidget(this);
    mainContainer->setObjectName("mainContainer");
    mainContainer->setFixedSize(420, 720);

    // 添加阴影效果
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(mainContainer);
    shadow->setBlurRadius(25);
    shadow->setColor(QColor(0, 0, 0, 60));
    shadow->setOffset(0, 5);
    mainContainer->setGraphicsEffect(shadow);

    // 标题栏
    QWidget *titleBar = new QWidget(mainContainer);
    titleBar->setFixedHeight(45);
    titleBar->setObjectName("titleBar");

    QLabel *titleLabel = new QLabel("注册新账号", titleBar);
    titleLabel->setStyleSheet("color: #333; font-weight: bold; font-size: 16px; margin-left: 20px;");

    closeBtn = new QPushButton("×", titleBar);
    closeBtn->setFixedSize(45, 45);
    closeBtn->setObjectName("closeBtn");

    QHBoxLayout *titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();
    titleLayout->addSpacing(250);
    titleLayout->addWidget(closeBtn);

    // 表单区域
    QWidget *formContainer = new QWidget(mainContainer);
    formContainer->setObjectName("formContainer");
    formContainer->move(0, 45);
    formContainer->setFixedSize(420, 675);

    // 头像区域
    QWidget *avatarContainer = new QWidget(formContainer);
    avatarContainer->setFixedSize(90, 90);
    avatarContainer->setStyleSheet(R"(
        background-color: #f0f8ff;
        border-radius: 45px;
        border: 2px dashed #1e90ff;
    )");

    QLabel *avatarIcon = new QLabel("+", avatarContainer);
    avatarIcon->setFixedSize(50, 50);
    avatarIcon->setStyleSheet("color: #1e90ff; font-size: 32px; font-weight: bold;");
    avatarIcon->setAlignment(Qt::AlignCenter);

    QLabel *avatarText = new QLabel("点击上传头像", avatarContainer);
    avatarText->setStyleSheet("color: #999; font-size: 12px;");
    avatarText->setAlignment(Qt::AlignCenter);

    QVBoxLayout *avatarLayout = new QVBoxLayout(avatarContainer);
    avatarLayout->addWidget(avatarIcon, 0, Qt::AlignCenter);
    avatarLayout->addWidget(avatarText, 0, Qt::AlignCenter);

    // 输入框容器
    QWidget *inputContainer = new QWidget(formContainer);
    inputContainer->setFixedSize(320, 300);

    // 定义输入框配置
    struct InputField {
        QString placeholder;
        QString icon;
        bool isPassword;
        int yPos;
    };

    QVector<InputField> fields = {
        {"账号", ":/icons/user.png", false, 0},
        {"设置密码", ":/icons/password.png", true, 60},
        {"昵称", ":/icons/nickname.png", false, 120},
        {"邮箱", ":/icons/email.png", false, 180},
    };

    QVBoxLayout *inputLayout = new QVBoxLayout(inputContainer);
    inputLayout->setSpacing(15);
    inputLayout->setContentsMargins(0, 0, 0, 0);

    for (int i = 0; i < fields.size(); ++i) {
        QWidget *inputWidget = new QWidget();
        inputWidget->setFixedSize(320, 50);
        inputWidget->setObjectName("inputWidget");

        QLineEdit *lineEdit = new QLineEdit(inputWidget);
        lineEdit->setGeometry(15, 10, 290, 30);
        lineEdit->setPlaceholderText(fields[i].placeholder);
        lineEdit->setStyleSheet("border: none; background: transparent; font-size: 14px;");

        if (fields[i].isPassword) {
            lineEdit->setEchoMode(QLineEdit::Password);
        }

        // 保存输入框引用
        switch(i) {
        case 0: usernameEdit = lineEdit; break;
        case 1: passwordEdit = lineEdit; break;
        case 2: nicknameEdit = lineEdit; break;
        case 3: emailEdit = lineEdit; break;
        }

        QLabel *iconLabel = new QLabel(inputWidget);
        iconLabel->setGeometry(280, 15, 20, 20);
        iconLabel->setPixmap(QPixmap(fields[i].icon).scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));

        inputLayout->addWidget(inputWidget);
    }

    // 密码选项
    togglePwdBtn = new QCheckBox("显示密码", formContainer);
    togglePwdBtn->setObjectName("togglePwdBtn");

    QWidget *buttonContainer = new QWidget(formContainer);
    QVBoxLayout *buttonLayout = new QVBoxLayout(buttonContainer);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(15); // 按钮之间的垂直间距

    // 注册按钮
    registerBtn = new QPushButton("立即注册", formContainer);
    registerBtn->setFixedSize(320, 45);
    registerBtn->setObjectName("registerBtn");

    // 去登录按钮
    toLoginBtn = new QPushButton("已有账号？去登录", formContainer);
    toLoginBtn->setObjectName("toLoginBtn");
    toLoginBtn->setStyleSheet("color: #1e90ff; border: none; font-size: 14px;");

    // 将按钮添加到按钮布局
    buttonLayout->addWidget(registerBtn, 0, Qt::AlignCenter);
    buttonLayout->addWidget(toLoginBtn, 0, Qt::AlignCenter);

    // 主布局
    QVBoxLayout *formMainLayout = new QVBoxLayout(formContainer);
    formMainLayout->setContentsMargins(50, 30, 50, 30);
    formMainLayout->setSpacing(25);
    formMainLayout->addWidget(avatarContainer, 0, Qt::AlignCenter);
    formMainLayout->addWidget(inputContainer, 0, Qt::AlignCenter);
    formMainLayout->addWidget(togglePwdBtn, 0, Qt::AlignCenter);
    formMainLayout->addSpacing(10);  // 增加间距，从 10 改为 25
    formMainLayout->addWidget(registerBtn, 0, Qt::AlignCenter);
    formMainLayout->addSpacing(15);  // 增加按钮之间的间距
    formMainLayout->addWidget(buttonContainer, 0, Qt::AlignCenter); // 添加按钮容器
    formMainLayout->addStretch();

    // 窗口布局
    QVBoxLayout *windowLayout = new QVBoxLayout(this);
    windowLayout->setContentsMargins(0, 0, 0, 0);
    windowLayout->addWidget(mainContainer);
}

void RegisterPage::setupStyles() {
    QString styleSheet = R"(
        #mainContainer {
            background-color: white;
            border-radius: 10px;
        }

        #titleBar {
            background-color: white;
            border-top-left-radius: 10px;
            border-top-right-radius: 10px;
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

        #inputWidget {
            background-color: #f8f9fa;
            border-radius: 8px;
            border: 1px solid #e4e6eb;
        }

        #inputWidget:hover {
            border-color: #1e90ff;
            background-color: #f0f8ff;
        }

        #inputWidget:focus-within {
            border-color: #1e90ff;
            background-color: #f0f8ff;
            border-width: 2px;
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
            image: url(:/icons/check_white.png);
        }

        #registerBtn {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #1e90ff, stop:1 #00bfff);
            color: white;
            border-radius: 22px;
            font-size: 16px;
            font-weight: bold;
            border: none;
        }

        #registerBtn:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #187bdd, stop:1 #00a8e8);
        }

        #registerBtn:pressed {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #0f6bcc, stop:1 #0095d3);
        }

        #registerBtn:disabled {
            background: #ccc;
            color: #999;
        }

        #toLoginBtn {
            color: #1e90ff;
            font-size: 14px;
            padding: 8px 16px;
            border-radius: 4px;
            background-color: transparent;
        }

        #toLoginBtn:hover {
            background-color: #f0f8ff;
            text-decoration: underline;
        }
    )";

    setStyleSheet(styleSheet);
}

void RegisterPage::onTogglePassword() {
    bool isVisible = togglePwdBtn->isChecked();
    QLineEdit::EchoMode mode = isVisible ? QLineEdit::Normal : QLineEdit::Password;

    passwordEdit->setEchoMode(mode);
}

void RegisterPage::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton &&
        event->pos().y() < 45) {  // 只有标题栏区域可拖动
        isDragging = true;
        dragStartPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }
}

void RegisterPage::mouseMoveEvent(QMouseEvent *event) {
    if (isDragging && event->buttons() & Qt::LeftButton) {
        move(event->globalPosition().toPoint() - dragStartPosition);
        event->accept();
    }
}

void RegisterPage::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
        event->accept();
    }
}
