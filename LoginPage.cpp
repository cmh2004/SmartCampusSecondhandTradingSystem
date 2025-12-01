#include "LoginPage.h"
#include "RegisterPage.h"

LoginPage::LoginPage(QWidget *parent) : QDialog(parent) {
    // 窗口基础设置
    setWindowTitle("登录");
    setFixedSize(350, 220);

    // 登录页样式表
    setStyleSheet(R"(
        QDialog {
            background-color: white;
            border-radius: 8px;
        }
        QLabel#titleLabel {
            color: #2c3e50;
        }
        QLineEdit {
            padding: 8px;
            border: 1px solid #ddd;
            border-radius: 4px;
            font-size: 14px;
        }
        QLineEdit:focus {
            border-color: #3498db;
            outline: none;
        }
        QPushButton {
            padding: 8px 0;
            border-radius: 4px;
            font-size: 14px;
            border: none;
        }
        QPushButton#loginBtn {
            background-color: #3498db;
            color: white;
        }
        QPushButton#loginBtn:hover {
            background-color: #2980b9;
        }
        QPushButton#toRegisterBtn {
            background-color: #ecf0f1;
            color: #333;
        }
        QPushButton#toRegisterBtn:hover {
            background-color: #ddd;
        }
        QCheckBox {
            color: #666;
        }
    )");

    // 1. 创建UI控件
    QLabel *titleLabel = new QLabel("用户登录", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);

    QLabel *userLabel = new QLabel("账号：", this);
    usernameEdit = new QLineEdit(this);
    usernameEdit->setPlaceholderText("请输入账号");

    QLabel *pwdLabel = new QLabel("密码：", this);
    passwordEdit = new QLineEdit(this);
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setPlaceholderText("请输入密码");

    loginBtn = new QPushButton("登录", this);
    toRegisterBtn = new QPushButton("去注册", this);

    togglePwdBtn = new QCheckBox("显示", this);
    togglePwdBtn->setFixedWidth(50);

    // 2. 布局管理
    QHBoxLayout *userLayout = new QHBoxLayout();
    userLayout->addWidget(userLabel);
    userLayout->addWidget(usernameEdit);

    QHBoxLayout *pwdLayout = new QHBoxLayout();
    pwdLayout->addWidget(pwdLabel);
    pwdLayout->addWidget(passwordEdit);
    pwdLayout->addWidget(togglePwdBtn);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(loginBtn);
    btnLayout->addWidget(toRegisterBtn);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacing(15);
    mainLayout->addLayout(userLayout);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(pwdLayout);
    mainLayout->addSpacing(15);
    mainLayout->addLayout(btnLayout);
    mainLayout->addSpacing(10);

    setLayout(mainLayout);

    // 标题标签设置对象名（用于样式表定位）
    titleLabel->setObjectName("titleLabel");
    loginBtn->setObjectName("loginBtn");
    toRegisterBtn->setObjectName("toRegisterBtn");

    // 3. 信号绑定
    // 点击"去注册"：显示注册页，关闭后返回登录页
    connect(toRegisterBtn, &QPushButton::clicked, this, [=]() {
        RegisterPage registerPage;
        registerPage.exec();
    });
    // 点击"登录"触发QDialog::accepted信号（登录成功）
    connect(loginBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(togglePwdBtn, &QCheckBox::stateChanged, this, [=](int state) {
        if (state == Qt::Checked) {
            // 选中状态，显示密码
            passwordEdit->setEchoMode(QLineEdit::Normal);
        } else {
            // 未选中状态，隐藏密码
            passwordEdit->setEchoMode(QLineEdit::Password);
        }
    });
}
