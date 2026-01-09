#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QFileDialog>
#include "profileeditdialog.h"

ProfileEditDialog::ProfileEditDialog(QWidget *parent)
    : QDialog(parent) {
    setWindowTitle("编辑个人资料");
    setFixedSize(500, 700);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    setupUI();
    loadCurrentProfile();
}

void ProfileEditDialog::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 头像区域
    QGroupBox *avatarGroup = new QGroupBox("头像");
    QHBoxLayout *avatarLayout = new QHBoxLayout();

    avatarLabel = new QLabel();
    avatarLabel->setPixmap(QPixmap(":/icons/img/user.png").scaled(80, 80));
    avatarLabel->setStyleSheet("border-radius: 40px; border: 3px solid #3498db;");
    avatarLabel->setFixedSize(80, 80);
    avatarLabel->setScaledContents(true);

    uploadAvatarBtn = new QPushButton("更换头像");
    uploadAvatarBtn->setObjectName("primaryBtn");

    avatarLayout->addWidget(avatarLabel);
    avatarLayout->addWidget(uploadAvatarBtn);
    avatarLayout->addStretch();
    avatarGroup->setLayout(avatarLayout);
    mainLayout->addWidget(avatarGroup);

    // 基本信息
    QGroupBox *basicGroup = new QGroupBox("基本信息");
    QFormLayout *basicLayout = new QFormLayout();

    nicknameEdit = new QLineEdit();
    realNameEdit = new QLineEdit();
    studentIdEdit = new QLineEdit();
    phoneEdit = new QLineEdit();
    emailEdit = new QLineEdit();
    campusCombo = new QComboBox();
    collegeCombo = new QComboBox();

    campusCombo->addItems({"主校区", "东校区", "西校区", "新校区"});
    collegeCombo->addItems({"计算机学院", "电子信息学院", "经济管理学院",
                            "外国语学院", "理学院", "其他学院"});

    basicLayout->addRow("昵称:", nicknameEdit);
    basicLayout->addRow("真实姓名:", realNameEdit);
    basicLayout->addRow("学号:", studentIdEdit);
    basicLayout->addRow("手机号:", phoneEdit);
    basicLayout->addRow("邮箱:", emailEdit);
    basicLayout->addRow("所在校区:", campusCombo);
    basicLayout->addRow("所属学院:", collegeCombo);

    basicGroup->setLayout(basicLayout);
    mainLayout->addWidget(basicGroup);

    // 安全设置
    QGroupBox *securityGroup = new QGroupBox("安全设置");
    QVBoxLayout *securityLayout = new QVBoxLayout();

    changePwdBtn = new QPushButton("修改密码");
    bindPhoneBtn = new QPushButton("绑定手机号");

    changePwdBtn->setObjectName("primaryBtn");
    bindPhoneBtn->setObjectName("primaryBtn");

    securityLayout->addWidget(changePwdBtn);
    securityLayout->addWidget(bindPhoneBtn);
    securityGroup->setLayout(securityLayout);
    mainLayout->addWidget(securityGroup);

    // 按钮区域
    QWidget *buttonWidget = new QWidget();
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);

    saveBtn = new QPushButton("保存修改");
    cancelBtn = new QPushButton("取消");

    saveBtn->setObjectName("primaryBtn");
    cancelBtn->setObjectName("secondaryBtn");

    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelBtn);
    buttonLayout->addWidget(saveBtn);

    mainLayout->addWidget(buttonWidget);

    // 连接信号槽
    connect(saveBtn, &QPushButton::clicked, this, &ProfileEditDialog::onSaveProfile);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(uploadAvatarBtn, &QPushButton::clicked, this, &ProfileEditDialog::onUploadAvatar);
    connect(changePwdBtn, &QPushButton::clicked, []() {
        QMessageBox::information(nullptr, "提示", "修改密码功能开发中");
    });
    connect(bindPhoneBtn, &QPushButton::clicked, []() {
        QMessageBox::information(nullptr, "提示", "绑定手机号功能开发中");
    });

    // 样式
    setStyleSheet(R"(
        QGroupBox {
            font-weight: bold;
            border: 1px solid #ddd;
            border-radius: 6px;
            margin-top: 10px;
            padding-top: 10px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
        }
        QLineEdit, QComboBox {
            padding: 6px;
            border: 1px solid #ddd;
            border-radius: 4px;
        }
        #primaryBtn {
            background-color: #3498db;
            color: white;
            border-radius: 4px;
            padding: 10px 20px;
        }
        #secondaryBtn {
            background-color: #ecf0f1;
            color: #34495e;
            border-radius: 4px;
            padding: 8px 16px;
        }
    )");
}

void ProfileEditDialog::loadCurrentProfile() {
    // 模拟加载当前用户资料
    nicknameEdit->setText("张三同学");
    realNameEdit->setText("张三");
    studentIdEdit->setText("2024012345");
    phoneEdit->setText("138****1234");
    emailEdit->setText("zhangsan@student.edu.cn");
    campusCombo->setCurrentText("主校区");
    collegeCombo->setCurrentText("计算机学院");
}

void ProfileEditDialog::onSaveProfile() {
    QString nickname = nicknameEdit->text().trimmed();
    QString realName = realNameEdit->text().trimmed();
    QString studentId = studentIdEdit->text().trimmed();
    QString phone = phoneEdit->text().trimmed();
    QString email = emailEdit->text().trimmed();

    if (nickname.isEmpty() || realName.isEmpty()) {
        QMessageBox::warning(this, "提示", "昵称和真实姓名不能为空");
        return;
    }

    // 验证手机号格式
    if (!phone.isEmpty()) {
        QRegularExpression phoneRegex("^1[3-9]\\d{9}$");
        if (!phoneRegex.match(phone).hasMatch()) {
            QMessageBox::warning(this, "提示", "请输入正确的手机号格式");
            return;
        }
    }

    // 验证邮箱格式
    if (!email.isEmpty()) {
        QRegularExpression emailRegex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
        if (!emailRegex.match(email).hasMatch()) {
            QMessageBox::warning(this, "提示", "请输入正确的邮箱格式");
            return;
        }
    }

    QMessageBox::information(this, "保存成功", "个人资料已更新");
    emit profileUpdated();
    accept();
}

void ProfileEditDialog::onUploadAvatar() {
    QString fileName = QFileDialog::getOpenFileName(this, "选择头像",
                                                    "", "Images (*.png *.jpg *.jpeg)");
    if (!fileName.isEmpty()) {
        QPixmap pixmap(fileName);
        avatarLabel->setPixmap(pixmap.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        avatarLabel->setScaledContents(true);
        QMessageBox::information(this, "提示", "头像已更新，保存后生效");
    }
}
