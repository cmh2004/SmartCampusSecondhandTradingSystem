#include "RegisterPage.h"

RegisterPage::RegisterPage(QWidget *parent) : QDialog(parent) {
    setWindowTitle("注册");
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
        QPushButton#registerBtn {
            background-color: #3498db;
            color: white;
        }
        QPushButton#registerBtn:hover {
            background-color: #2980b9;
        }
        QPushButton#toLoginBtn {
            background-color: #ecf0f1;
            color: #333;
        }
        QPushButton#toLoginBtn:hover {
            background-color: #ddd;
        }
        QCheckBox {
            color: #666;
        }
    )");

    // 1. 创建UI控件
    QLabel *titleLabel = new QLabel("用户注册", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);

    QLabel *userLabel = new QLabel("账号：", this);
    usernameEdit = new QLineEdit(this);
    usernameEdit->setPlaceholderText("请设置账号");

    QLabel *pwdLabel = new QLabel("密码：", this);
    passwordEdit = new QLineEdit(this);
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setPlaceholderText("请设置密码");

    registerBtn = new QPushButton("注册", this);
    toLoginBtn = new QPushButton("去登录", this);

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
    btnLayout->addWidget(registerBtn);
    btnLayout->addWidget(toLoginBtn);

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
    registerBtn->setObjectName("registerBtn");
    toLoginBtn->setObjectName("toLoginBtn");

    // 3. 信号绑定
    // 点击"去登录"关闭注册页（返回登录页）
    connect(toLoginBtn, &QPushButton::clicked, this, &QDialog::close);
    // 点击"注册"按钮关闭注册页（可在此处添加注册逻辑验证）
    connect(registerBtn, &QPushButton::clicked, this, &QDialog::close);
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
