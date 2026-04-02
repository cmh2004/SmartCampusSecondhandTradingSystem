#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QShortcut>
#include <QTimer>
#include <QRegularExpression>
#include <QJsonObject>
#include "registerpage.h"
#include "apiservice.h"
#include "commonwidgets.h"

RegisterPage::RegisterPage(QWidget *parent) : QDialog(parent), isDragging(false), isPasswordVisible(false) {
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(420, 575);

    setupUI();
    setupStyles();

    // 连接信号槽
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(toLoginBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(registerBtn, &QPushButton::clicked, this, &RegisterPage::onRegisterClicked);
    connect(togglePwdBtn, &QPushButton::clicked, this, &RegisterPage::onTogglePassword);

    // 回车键注册快捷键
    QShortcut *enterShortcut = new QShortcut(QKeySequence(Qt::Key_Return), this);
    connect(enterShortcut, &QShortcut::activated, this, &QDialog::accept);
}

void RegisterPage::setupUI() {
    // 主容器
    QWidget *mainContainer = new QWidget(this);
    mainContainer->setObjectName("mainContainer");
    mainContainer->setFixedSize(420, 570);

    // 添加阴影效果
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(mainContainer);
    shadow->setBlurRadius(25);
    shadow->setColor(QColor(0, 0, 0, 60));
    shadow->setOffset(0, 3);
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
    QWidget *avatarArea = new QWidget(formContainer);
    avatarArea->setFixedHeight(110);

    QVBoxLayout *avatarAreaLayout = new QVBoxLayout(avatarArea);
    avatarAreaLayout->setContentsMargins(0, 0, 0, 0);
    avatarAreaLayout->setSpacing(2);

    // 头像容器
    QWidget *avatarContainer = new QWidget();
    avatarContainer->setFixedSize(80, 80);
    avatarContainer->setStyleSheet(R"(
        background-color: #f0f8ff;
        border-radius: 40px;
        border: 2px dashed #1e90ff;
    )");

    QLabel *avatarIcon = new QLabel("+", avatarContainer);
    avatarIcon->setFixedSize(50, 50);
    avatarIcon->setStyleSheet("color: #1e90ff; font-size: 24px; font-weight: bold;");
    avatarIcon->setAlignment(Qt::AlignCenter);
    avatarIcon->move(15, 15); // 居中显示

    // 头像文字
    QLabel *avatarText = new QLabel("点击上传头像");
    avatarText->setStyleSheet(R"(
        color: #666;
        font-size: 11px;
        border: none;
        background-color: transparent;
        padding: 0;
        margin: 0;
    )");
    avatarText->setAlignment(Qt::AlignCenter);
    avatarText->setFixedHeight(15);
    avatarAreaLayout->addWidget(avatarContainer, 0, Qt::AlignCenter);
    avatarAreaLayout->addWidget(avatarText, 0, Qt::AlignCenter);

    // 输入框容器
    QWidget *inputContainer = new QWidget(formContainer);
    inputContainer->setFixedSize(320, 260);
    inputContainer->setContentsMargins(0, 0, 0, 0); // 清除边距

    // 定义输入框配置
    struct InputField {
        QString placeholder;
        bool isPassword;
    };

    QVector<InputField> fields = {
                                  {"账号", false},
                                  {"设置密码", true},
                                  {"昵称", false},
                                  {"邮箱", false},
                                  };

    QVBoxLayout *inputLayout = new QVBoxLayout(inputContainer);
    inputLayout->setSpacing(8);
    inputLayout->setContentsMargins(0, 0, 0, 0);

    for (int i = 0; i < fields.size(); ++i) {
        QWidget *inputWidget = new QWidget();
        inputWidget->setFixedHeight(45);
        inputWidget->setObjectName("inputWidget");

        QLineEdit *lineEdit = new QLineEdit(inputWidget);
        lineEdit->setGeometry(15, 8, 290, 30);
        lineEdit->setPlaceholderText(fields[i].placeholder);
        lineEdit->setStyleSheet(R"(
            border: none;
            background: transparent;
            font-size: 13px;
            padding: 0;
        )");

        if (fields[i].isPassword) {
            lineEdit->setEchoMode(QLineEdit::Password);
            togglePwdBtn = new QPushButton(inputWidget);
            togglePwdBtn->setGeometry(270, 8, 40, 30);
            togglePwdBtn->setIcon(QIcon(":/icons/img/eye_close.png"));
            togglePwdBtn->setIconSize(QSize(20, 20));
            togglePwdBtn->setStyleSheet(R"(
                border: none;
                background: transparent;
            )");
        }

        // 保存输入框引用
        switch(i) {
        case 0: usernameEdit = lineEdit; break;
        case 1: passwordEdit = lineEdit; break;
        case 2: nicknameEdit = lineEdit; break;
        case 3: emailEdit = lineEdit; break;
        }

        inputLayout->addWidget(inputWidget);
    }

    // 按钮区域
    QWidget *buttonContainer = new QWidget(formContainer);
    buttonContainer->setFixedHeight(100);
    QVBoxLayout *buttonLayout = new QVBoxLayout(buttonContainer);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(8);

    // 注册按钮
    registerBtn = new QPushButton("立即注册");
    registerBtn->setFixedSize(320, 42);
    registerBtn->setObjectName("registerBtn");

    // 去登录按钮
    toLoginBtn = new QPushButton("已有账号？去登录");
    toLoginBtn->setObjectName("toLoginBtn");
    toLoginBtn->setFixedHeight(20);
    toLoginBtn->setStyleSheet(R"(
        color: #1e90ff;
        border: none;
        font-size: 13px;
        background-color: transparent;
        padding: 0;
    )");

    // 将按钮添加到按钮布局
    buttonLayout->addWidget(registerBtn, 0, Qt::AlignCenter);
    buttonLayout->addWidget(toLoginBtn, 0, Qt::AlignCenter);

    // 主布局
    QVBoxLayout *formMainLayout = new QVBoxLayout(formContainer);
    formMainLayout->setContentsMargins(50, 15, 50, 15);
    formMainLayout->setSpacing(0);

    // 添加组件，精确控制间距
    formMainLayout->addSpacing(5);  // 顶部间距
    formMainLayout->addWidget(avatarArea, 0, Qt::AlignCenter);
    formMainLayout->addSpacing(15); // 头像和输入框之间的间距
    formMainLayout->addWidget(inputContainer, 0, Qt::AlignCenter);
    formMainLayout->addSpacing(10); // 输入框和按钮之间的间距
    formMainLayout->addWidget(buttonContainer, 0, Qt::AlignCenter);
    formMainLayout->addStretch();   // 弹性空间，让内容保持在上部

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
            border: 1px solid #e0e0e0;
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
    isPasswordVisible = !isPasswordVisible;
    if (isPasswordVisible) {
        passwordEdit->setEchoMode(QLineEdit::Normal);
        togglePwdBtn->setIcon(QIcon(":/icons/img/eye_open.png"));
    } else {
        passwordEdit->setEchoMode(QLineEdit::Password);
        togglePwdBtn->setIcon(QIcon(":/icons/img/eye_close.png"));
    }
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

void RegisterPage::onRegisterClicked() {
    QString username = usernameEdit->text().trimmed();
    QString password = passwordEdit->text().trimmed();
    QString nickname = nicknameEdit->text().trimmed();
    QString email = emailEdit->text().trimmed();
    // 可以加手机号，但当前界面没有，可以忽略或添加
    QString phone = ""; // 未提供

    if (username.isEmpty() || password.isEmpty()) {
        showMessageBox(this, "提示", "请填写账号和密码", QMessageBox::Warning);
        return;
    }
    // 密码强度验证等...
    QJsonObject result = ApiService::instance()->registerUser(username, password, email, phone, nickname);
    if (result.value("success").toBool()) {
        showMessageBox(this, "成功", "注册成功！请登录", QMessageBox::Information);
        accept(); // 关闭注册页面，回到登录页
    } else {
        showMessageBox(this, "失败", result.value("error").toString(), QMessageBox::Warning);
    }
}
