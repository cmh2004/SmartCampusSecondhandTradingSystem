#include "LoginPage.h"
#include "RegisterPage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QPainterPath>
#include <QFontDatabase>
#include <QShortcut>

// 自定义圆形头像标签
class AvatarLabel : public QLabel {
public:
    AvatarLabel(QWidget *parent = nullptr) : QLabel(parent) {
        setFixedSize(80, 80);
        setAlignment(Qt::AlignCenter);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        // 绘制圆形背景
        QPainterPath path;
        path.addEllipse(rect());
        painter.setClipPath(path);

        // 使用渐变色背景
        QLinearGradient gradient(0, 0, width(), height());
        gradient.setColorAt(0, QColor(0, 150, 255));
        gradient.setColorAt(1, QColor(0, 100, 220));
        painter.fillRect(rect(), gradient);

        // 绘制默认头像图标
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 30, QFont::Bold));
        painter.drawText(rect(), Qt::AlignCenter, "S");

        // 绘制白色边框
        painter.setClipping(false);
        painter.setPen(QPen(Qt::white, 2));
        painter.drawEllipse(rect().adjusted(1, 1, -1, -1));
    }
};

LoginPage::LoginPage(QWidget *parent) : QDialog(parent), isDragging(false) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(900, 520);

    setupUI();
    setupStyles();

    // 连接信号槽
    connect(toRegisterBtn, &QPushButton::clicked, this, &LoginPage::onRegisterClicked);
    connect(loginBtn, &QPushButton::clicked, this, &LoginPage::onLoginClicked);
    connect(togglePwdBtn, &QPushButton::clicked, this, &LoginPage::onTogglePassword);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(minimizeBtn, &QPushButton::clicked, this, &QWidget::showMinimized);

    // 回车键登录快捷键
    QShortcut *enterShortcut = new QShortcut(QKeySequence(Qt::Key_Return), this);
    connect(enterShortcut, &QShortcut::activated, this, &LoginPage::onLoginClicked);
}

void LoginPage::setupUI() {
    // 主容器
    QWidget *mainContainer = new QWidget(this);
    mainContainer->setObjectName("mainContainer");
    mainContainer->setFixedSize(900, 520);

    // 添加阴影效果
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(mainContainer);
    shadow->setBlurRadius(30);
    shadow->setColor(QColor(0, 0, 0, 60));
    shadow->setOffset(0, 5);
    mainContainer->setGraphicsEffect(shadow);

    // 主布局
    QHBoxLayout *mainLayout = new QHBoxLayout(mainContainer);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 添加左侧面板
    mainLayout->addWidget(createLeftPanel());

    // 添加右侧面板
    mainLayout->addWidget(createRightPanel());

    // 设置主布局 - 修复顶部透明行问题
    QVBoxLayout *windowLayout = new QVBoxLayout(this);
    windowLayout->setContentsMargins(0, 0, 0, 0);
    windowLayout->setSpacing(0); // 关键修改：去除布局间距

    // 创建标题栏并添加到布局
    QWidget *titleBar = createTitleBar();
    QWidget *seamlessWidget = new QWidget();
    seamlessWidget->setFixedHeight(1);
    seamlessWidget->setStyleSheet("background-color: white;");

    // 修改窗口布局
    windowLayout->addWidget(titleBar);
    windowLayout->addWidget(seamlessWidget);  // 添加覆盖层
    windowLayout->addWidget(mainContainer);

    // 确保标题栏与主容器无缝衔接
    titleBar->setStyleSheet(titleBar->styleSheet() + "border-bottom: none;");
    mainContainer->setStyleSheet(mainContainer->styleSheet() + "border-top-left-radius: 0px; border-top-right-radius: 0px;");

    setLayout(windowLayout);
}

QWidget* LoginPage::createTitleBar() {
    QWidget *titleBar = new QWidget(this);
    titleBar->setFixedHeight(40);
    titleBar->setObjectName("titleBar");

    // 标题
    QLabel *titleLabel = new QLabel("校园二手交易系统", titleBar);
    titleLabel->setStyleSheet("color: #333; font-weight: bold; font-size: 14px; margin-left: 20px;");

    // 窗口控制按钮
    minimizeBtn = new QPushButton("－", titleBar);
    closeBtn = new QPushButton("×", titleBar);

    minimizeBtn->setFixedSize(40, 40);
    closeBtn->setFixedSize(40, 40);

    minimizeBtn->setObjectName("minimizeBtn");
    closeBtn->setObjectName("closeBtn");

    // 布局
    QHBoxLayout *layout = new QHBoxLayout(titleBar);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(titleLabel);
    layout->addStretch();
    layout->addWidget(minimizeBtn);
    layout->addWidget(closeBtn);

    return titleBar;
}

QWidget* LoginPage::createLeftPanel() {
    QWidget *leftPanel = new QWidget();
    leftPanel->setFixedWidth(400);
    leftPanel->setObjectName("leftPanel");

    // Logo
    QLabel *logoLabel = new QLabel(leftPanel);
    logoLabel->setPixmap(QPixmap(":/icons/qq_logo.png").scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // 标题
    QLabel *titleLabel = new QLabel("校园二手交易", leftPanel);
    titleLabel->setStyleSheet("color: white; font-size: 28px; font-weight: bold;");

    // 副标题
    QLabel *subtitleLabel = new QLabel("Campus Second-hand Trading", leftPanel);
    subtitleLabel->setStyleSheet("color: rgba(255, 255, 255, 0.9); font-size: 16px;");

    // 底部信息
    QLabel *infoLabel = new QLabel("安全 · 便捷 · 高效", leftPanel);
    infoLabel->setStyleSheet("color: rgba(255, 255, 255, 0.7); font-size: 14px;");

    // 垂直布局
    QVBoxLayout *layout = new QVBoxLayout(leftPanel);
    layout->addStretch();
    layout->addWidget(logoLabel, 0, Qt::AlignCenter);
    layout->addSpacing(20);
    layout->addWidget(titleLabel, 0, Qt::AlignCenter);
    layout->addWidget(subtitleLabel, 0, Qt::AlignCenter);
    layout->addSpacing(40);
    layout->addWidget(infoLabel, 0, Qt::AlignCenter);
    layout->addStretch();

    return leftPanel;
}

QWidget* LoginPage::createRightPanel() {
    QWidget *rightPanel = new QWidget();
    rightPanel->setFixedWidth(500);
    rightPanel->setObjectName("rightPanel");

    // 头像
    AvatarLabel *avatar = new AvatarLabel(rightPanel);

    // 输入框容器
    QWidget *inputContainer = new QWidget(rightPanel);
    inputContainer->setFixedSize(320, 150);

    // 账号输入框
    QWidget *usernameWidget = new QWidget(inputContainer);
    usernameWidget->setFixedSize(320, 50);
    usernameWidget->setObjectName("usernameWidget");

    usernameEdit = new QLineEdit(usernameWidget);
    usernameEdit->setGeometry(15, 10, 290, 30);
    usernameEdit->setPlaceholderText("账号");
    usernameEdit->setStyleSheet("border: none; background: transparent; font-size: 14px;");

    QLabel *userIcon = new QLabel(usernameWidget);
    userIcon->setGeometry(15, 15, 20, 20);
    userIcon->setPixmap(QPixmap(":/icons/user.png").scaled(20, 20));

    // 密码输入框
    QWidget *passwordWidget = new QWidget(inputContainer);
    passwordWidget->setFixedSize(320, 50);
    passwordWidget->setObjectName("passwordWidget");
    passwordWidget->move(0, 60);

    passwordEdit = new QLineEdit(passwordWidget);
    passwordEdit->setGeometry(15, 10, 290, 30);
    passwordEdit->setPlaceholderText("密码");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setStyleSheet("border: none; background: transparent; font-size: 14px;");

    // 密码显示切换按钮
    togglePwdBtn = new QPushButton(passwordWidget);
    togglePwdBtn->setGeometry(270, 10, 40, 30);
    togglePwdBtn->setIcon(QIcon(":/icons/eye_close.png"));
    togglePwdBtn->setIconSize(QSize(20, 20));
    togglePwdBtn->setStyleSheet("border: none; background: transparent;");

    QLabel *pwdIcon = new QLabel(passwordWidget);
    pwdIcon->setGeometry(320 - 40, 15, 20, 20);
    pwdIcon->setPixmap(QPixmap(":/icons/password.png").scaled(20, 20));

    // 记住密码和自动登录
    QWidget *optionsWidget = new QWidget(inputContainer);
    optionsWidget->setFixedSize(320, 30);
    optionsWidget->move(0, 120);

    QCheckBox *rememberMe = new QCheckBox("记住密码", optionsWidget);
    rememberMe->setStyleSheet("color: #666; font-size: 13px;");
    rememberMe->move(0, 5);

    QCheckBox *autoLogin = new QCheckBox("自动登录", optionsWidget);
    autoLogin->setStyleSheet("color: #666; font-size: 13px;");
    autoLogin->move(120, 5);

    // 登录按钮
    loginBtn = new QPushButton("登录", rightPanel);
    loginBtn->setFixedSize(320, 42);
    loginBtn->setObjectName("loginBtn");

    // 注册按钮
    toRegisterBtn = new QPushButton("注册账号", rightPanel);
    toRegisterBtn->setObjectName("registerBtn");
    toRegisterBtn->setStyleSheet("color: #1e90ff; border: none; font-size: 13px;");

    // 忘记密码
    forgotBtn = new QPushButton("忘记密码？", rightPanel);
    forgotBtn->setObjectName("forgotBtn");
    forgotBtn->setStyleSheet("color: #999; border: none; font-size: 13px;");
    connect(forgotBtn, &QPushButton::clicked, this, &LoginPage::onForgotPasswordClicked);

    // 底部链接
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addStretch();
    bottomLayout->addWidget(forgotBtn);
    bottomLayout->addSpacing(20); // 增加按钮间距
    bottomLayout->addWidget(toRegisterBtn);
    bottomLayout->addStretch();

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(rightPanel);
    mainLayout->addStretch();
    mainLayout->addWidget(avatar, 0, Qt::AlignCenter);
    mainLayout->addSpacing(30);
    mainLayout->addWidget(inputContainer, 0, Qt::AlignCenter);
    mainLayout->addSpacing(25); // 增加输入框与按钮间距
    mainLayout->addWidget(loginBtn, 0, Qt::AlignCenter);
    mainLayout->addSpacing(25); // 增加按钮与底部链接间距
    mainLayout->addLayout(bottomLayout);
    mainLayout->addStretch();

    return rightPanel;
}

void LoginPage::setupStyles() {
    QString styleSheet = R"(
        #mainContainer {
            background-color: white;
            border-radius:  0px 0px 8px 8px;
        }

        #titleBar {
            background-color: white;
            border-top-left-radius: 8px;
            border-top-right-radius: 8px;
            border-bottom: none;
        }

        #minimizeBtn, #closeBtn {
            color: #666;
            font-size: 18px;
            border: none;
            background: transparent;
        }

        #minimizeBtn:hover {
            background-color: #f5f5f5;
            color: #333;
        }

        #closeBtn:hover {
            background-color: #e81123;
            color: white;
            border-radius: 0;
        }

        #leftPanel {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #1e90ff, stop:1 #00bfff);
            border-top-left-radius: 8px;
            border-bottom-left-radius: 8px;
        }

        #rightPanel {
            background-color: white;
            border-top-right-radius: 8px;
            border-bottom-right-radius: 8px;
        }

        #usernameWidget, #passwordWidget {
            background-color: #f5f5f5;
            border-radius: 4px;
            border: 1px solid #e0e0e0;
        }

        #usernameWidget:hover, #passwordWidget:hover {
            border-color: #1e90ff;
        }

        #usernameWidget:focus-within, #passwordWidget:focus-within {
            border-color: #1e90ff;
            background-color: #f0f8ff;
        }

        #loginBtn {
            background-color: #1e90ff;
            color: white;
            border-radius: 21px;
            font-size: 16px;
            font-weight: bold;
            border: none;
        }

        #loginBtn:hover {
            background-color: #187bdd;
        }

        #loginBtn:pressed {
            background-color: #0f6bcc;
        }

        #registerBtn:hover, #forgotBtn:hover {
            text-decoration: underline;
            background-color: transparent;
        }
    )";

    setStyleSheet(styleSheet);
}

// 以下为原有事件处理函数，保持不变
void LoginPage::onTogglePassword() {
    static bool visible = false;
    visible = !visible;
    if (visible) {
        passwordEdit->setEchoMode(QLineEdit::Normal);
        togglePwdBtn->setIcon(QIcon(":/icons/eye_open.png"));
    } else {
        passwordEdit->setEchoMode(QLineEdit::Password);
        togglePwdBtn->setIcon(QIcon(":/icons/eye_close.png"));
    }
}

void LoginPage::onLoginClicked() {
    // 这里添加登录验证逻辑
    accept();
}

void LoginPage::onRegisterClicked() {
    RegisterPage registerPage;
    registerPage.exec();
}

void LoginPage::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton &&
        event->pos().y() < 40) {  // 只有标题栏区域可拖动
        isDragging = true;
        dragStartPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }
}

void LoginPage::mouseMoveEvent(QMouseEvent *event) {
    if (isDragging && event->buttons() & Qt::LeftButton) {
        move(event->globalPosition().toPoint() - dragStartPosition);
        event->accept();
    }
}

void LoginPage::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
        event->accept();
    }
}

void LoginPage::onForgotPasswordClicked() {
    ForgotPasswordPage forgotPage(this);
    forgotPage.setModal(true);
    forgotPage.exec();
}
