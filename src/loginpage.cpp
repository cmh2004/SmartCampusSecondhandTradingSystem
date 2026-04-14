#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QPainterPath>
#include <QFontDatabase>
#include <QShortcut>
#include <QSettings>
#include "ForgotPasswordPage.h"
#include "loginpage.h"
#include "registerpage.h"
#include "commonwidgets.h"
#include "apiservice.h"

LoginPage::LoginPage(QWidget *parent) : QDialog(parent), isDragging(false), selectedRole("user") {
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(900, 520);

    setupUI();
    setupStyles();
    loadLoginInfo();
    attemptAutoLogin();

    // 连接信号槽
    connect(toRegisterBtn, &QPushButton::clicked, this, &LoginPage::onRegisterClicked);
    connect(loginBtn, &QPushButton::clicked, this, &LoginPage::onLoginClicked);
    connect(togglePwdBtn, &QPushButton::clicked, this, &LoginPage::onTogglePassword);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(minimizeBtn, &QPushButton::clicked, this, &QWidget::showMinimized);

    if (roleCombo) {
        connect(roleCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &LoginPage::onRoleChanged);
    }

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
    shadow->setColor(QColor(0, 0, 0, 50));
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

    // 设置主布局
    QVBoxLayout *windowLayout = new QVBoxLayout(this);
    windowLayout->setContentsMargins(0, 0, 0, 0);
    windowLayout->setSpacing(0);
    windowLayout->addWidget(mainContainer);

    mainContainer->setStyleSheet(mainContainer->styleSheet() + "border-top-left-radius: 0px; border-top-right-radius: 0px;");

    setLayout(windowLayout);
}

QWidget* LoginPage::createLeftPanel() {
    QWidget *leftPanel = new QWidget();
    leftPanel->setFixedWidth(400);
    leftPanel->setObjectName("leftPanel");

    // Logo
    QLabel *logoLabel = new QLabel(leftPanel);
    logoLabel->setPixmap(QPixmap(":/icons/img/logo.png").scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));

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
    layout->addSpacing(85);
    layout->addWidget(logoLabel, 0, Qt::AlignCenter);
    layout->addSpacing(25);
    layout->addWidget(titleLabel, 0, Qt::AlignCenter);
    layout->addWidget(subtitleLabel, 0, Qt::AlignCenter);
    layout->addSpacing(30);
    layout->addWidget(infoLabel, 0, Qt::AlignCenter);
    layout->addStretch();

    return leftPanel;
}

QWidget* LoginPage::createRightPanel() {
    QWidget *rightPanel = new QWidget();
    rightPanel->setFixedWidth(500);
    rightPanel->setObjectName("rightPanel");

    minimizeBtn = new QPushButton("－", rightPanel);
    closeBtn = new QPushButton("×", rightPanel);
    // 定位到右上角
    minimizeBtn->setGeometry(423, 5, 35, 35);
    closeBtn->setGeometry(460, 5, 35, 35);
    // 设置按钮标识
    minimizeBtn->setObjectName("minimizeBtn");
    closeBtn->setObjectName("closeBtn");

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
    togglePwdBtn->setIcon(QIcon(":/icons/img/eye_close.png"));
    togglePwdBtn->setIconSize(QSize(20, 20));
    togglePwdBtn->setStyleSheet("border: none; background: transparent;");

    // 记住密码和自动登录
    QWidget *optionsWidget = new QWidget(inputContainer);
    optionsWidget->setFixedSize(320, 30);
    optionsWidget->move(0, 120);

    rememberMeCheckBox  = new QCheckBox("记住密码", optionsWidget);
    rememberMeCheckBox ->setStyleSheet("color: #666; font-size: 13px;");
    rememberMeCheckBox ->move(0, 5);

    // autoLoginCheckBox = new QCheckBox("自动登录", optionsWidget);
    autoLoginCheckBox = new QCheckBox("自动登录");
    autoLoginCheckBox ->setStyleSheet("color: #666; font-size: 13px;");
    autoLoginCheckBox ->move(120, 5);

    QWidget *roleWidget = new QWidget(rightPanel);
    roleWidget->setFixedSize(104, 32);
    roleWidget->move(10,10);
    roleWidget->setObjectName("roleWidget");

    roleCombo = new QComboBox(roleWidget);
    roleCombo->setGeometry(1, 1, 100, 30);
    roleCombo->addItem("👤普通用户");
    roleCombo->addItem("🛡️管理员");
    roleCombo->setStyleSheet(R"(
    border: 1px solid #e0e0e0;
    border-radius: 4px;
    padding: 8px;
    background-color: white;
)");

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
    mainLayout->addStretch(2);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(avatar, 0, Qt::AlignCenter);
    mainLayout->addSpacing(30);
    mainLayout->addWidget(inputContainer, 0, Qt::AlignCenter);
    mainLayout->addSpacing(25);
    mainLayout->addWidget(loginBtn, 0, Qt::AlignCenter);
    mainLayout->addSpacing(25);
    mainLayout->addLayout(bottomLayout);
    mainLayout->addStretch(1);

    return rightPanel;
}

void LoginPage::setupStyles() {
    QString styleSheet = R"(
        #mainContainer {
            background-color: white;
            border-radius:  15px;
            border: 1px solid #e0e0e0;
        }

        #minimizeBtn, #closeBtn {
            color: #666;
            font-size: 18px;
            border: none;
            background: transparent;、
            border-radius: 4px; /* 轻微圆角 */
        }

        #minimizeBtn:hover {
            background-color: #f5f5f5;
            color: #333;
        }

        #closeBtn:hover {
            background-color: #e81123;
            color: white;
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
            border: 1px solid #e0e0e0;
        }

        #roleWidget {
            background-color: #f5f5f5;
            border-radius: 4px;
            border: 1px solid #e0e0e0;
        }

        #roleWidget:hover {
            border-color: #1e90ff;
        }

        #roleWidget:focus-within {
            border-color: #1e90ff;
            background-color: #f0f8ff;
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

void LoginPage::onTogglePassword() {
    static bool visible = false;
    visible = !visible;
    if (visible) {
        passwordEdit->setEchoMode(QLineEdit::Normal);
        togglePwdBtn->setIcon(QIcon(":/icons/img/eye_open.png"));
    } else {
        passwordEdit->setEchoMode(QLineEdit::Password);
        togglePwdBtn->setIcon(QIcon(":/icons/img/eye_close.png"));
    }
}

void LoginPage::onRegisterClicked() {
    RegisterPage registerPage;
    registerPage.exec();
}

void LoginPage::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton &&
        event->pos().y() < 60) {  // 只有标题栏区域可拖动
        isDragging = true;
        dragStartPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }
}

void LoginPage::mouseMoveEvent(QMouseEvent *event) {
    if (isDragging && event->buttons() & Qt::LeftButton) {
        QPoint targetPos = event->globalPosition().toPoint() - dragStartPosition;

        // 获取当前屏幕
        QScreen *screen = QApplication::screenAt(targetPos);
        if (!screen) {
            screen = QApplication::primaryScreen();
        }

        QRect screenRect = screen->availableGeometry();
        QSize windowSize = size();

        // 限制窗口在屏幕内
        targetPos.setX(qMax(screenRect.left(),
                            qMin(targetPos.x(),
                                 screenRect.right() - windowSize.width())));
        targetPos.setY(qMax(screenRect.top(),
                            qMin(targetPos.y(),
                                 screenRect.bottom() - windowSize.height())));

        move(targetPos);
        event->accept();
        return;
    }
    QDialog::mouseMoveEvent(event);
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

void LoginPage::onRoleChanged(int index) {
    if (index == 0) {
        selectedRole = "user";  // 普通用户
    } else if (index == 1) {
        selectedRole = "admin"; // 管理员
    }
}

void LoginPage::onLoginClicked() {
    QString username = usernameEdit->text().trimmed();
    QString password = passwordEdit->text().trimmed();

    if (username.isEmpty() || password.isEmpty()) {
        showMessageBox(this, "登录失败", "请输入账号和密码", QMessageBox::Warning);
        return;
    }

    // 调用登录 API
    QJsonObject result = ApiService::instance()->login(username, password, selectedRole);
    if (result.value("success").toBool()) {
        // 保存登录信息
        bool remember = rememberMeCheckBox->isChecked();
        bool autoLogin = autoLoginCheckBox->isChecked();
        if (autoLogin && !remember) {
            // 如果只勾选了自动登录，则自动勾选记住密码
            remember = true;
        }
        saveLoginInfo(username, password, remember, autoLogin);
        // 登录成功，保存用户信息
        QJsonObject data = result.value("data").toObject();
        // 设置当前用户 ID 等
        ApiService::instance()->setCurrentUserId(data.value("user_id").toInt());
        selectedRole = data.value("role").toString();   // 保存实际角色
        accept();
    } else {
        QString error = result.value("error").toString();
        showMessageBox(this, "登录失败", error, QMessageBox::Warning);
    }
}

void LoginPage::saveLoginInfo(const QString& username, const QString& password, bool remember, bool autoLogin)
{
    QSettings settings("CampusSecondhand", "Login");
    if (remember) {
        settings.setValue("username", username);
        // 实际生产环境应对密码进行加密存储，这里仅作演示
        settings.setValue("password", password);
        settings.setValue("autoLogin", autoLogin);
        settings.setValue("role", selectedRole);   // 保存当前角色
    } else {
        settings.remove("username");
        settings.remove("password");
        settings.remove("autoLogin");
        settings.remove("role");
    }
}

void LoginPage::loadLoginInfo()
{
    QSettings settings("CampusSecondhand", "Login");
    QString savedUsername = settings.value("username").toString();
    QString savedPassword = settings.value("password").toString();
    bool autoLoginFlag = settings.value("autoLogin", false).toBool();
    QString savedRole = settings.value("role").toString();

    if (!savedUsername.isEmpty()) {
        usernameEdit->setText(savedUsername);
        passwordEdit->setText(savedPassword);
        rememberMeCheckBox->setChecked(true);
        autoLoginCheckBox->setChecked(autoLoginFlag);
    }
    // 恢复角色下拉框
    if (savedRole == "admin") {
        roleCombo->setCurrentIndex(1);
        selectedRole = "admin";
    } else {
        roleCombo->setCurrentIndex(0);
        selectedRole = "user";
    }
}

void LoginPage::attemptAutoLogin()
{
    QSettings settings("CampusSecondhand", "Login");
    if (settings.value("autoLogin", false).toBool()) {
        QString username = settings.value("username").toString();
        QString password = settings.value("password").toString();
        QString savedRole = settings.value("role").toString();
        if (!username.isEmpty() && !password.isEmpty()) {
            // 静默尝试登录，不弹任何消息框
            // 使用保存的角色，若无则默认为普通用户
            QString autoRole = savedRole.isEmpty() ? "user" : savedRole;
            QJsonObject result = ApiService::instance()->login(username, password, autoRole);
            if (result.value("success").toBool()) {
                QJsonObject data = result.value("data").toObject();
                ApiService::instance()->setCurrentUserId(data.value("user_id").toInt());
                selectedRole = data.value("role").toString();
                accept();  // 自动登录成功，关闭登录窗口
            }
        }
    }
}
