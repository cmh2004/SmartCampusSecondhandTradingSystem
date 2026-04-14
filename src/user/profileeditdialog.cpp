#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QJsonObject>
#include <QFileDialog>
#include "..\apiservice.h"
#include "profileeditdialog.h"

ProfileEditDialog::ProfileEditDialog(QWidget *parent)
    : QDialog(parent) {
    setWindowTitle("编辑个人资料");
    setFixedSize(500, 650);
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
    emailEdit = new QLineEdit();
    phoneEdit = new QLineEdit();

    basicLayout->addRow("昵称:", nicknameEdit);
    basicLayout->addRow("邮箱:", emailEdit);
    basicLayout->addRow("手机号:", phoneEdit);

    basicGroup->setLayout(basicLayout);
    mainLayout->addWidget(basicGroup);

    // 安全设置
    QGroupBox *securityGroup = new QGroupBox("安全设置");
    QVBoxLayout *securityLayout = new QVBoxLayout();

    changePwdBtn = new QPushButton("修改密码");

    changePwdBtn->setObjectName("primaryBtn");
    securityLayout->addWidget(changePwdBtn);
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
    connect(changePwdBtn, &QPushButton::clicked, this, &ProfileEditDialog::onChangePassword);

    // 样式
    setStyleSheet(R"(
        QDialog {
            background-color: white;
        }
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
            border: none;
            font-weight: bold;
        }
        #primaryBtn:hover {
            background-color: #2980b9;
        }
        #primaryBtn:pressed {
            background-color: #1c6ca1;
            padding-top: 11px;
            padding-bottom: 9px;
        }
        #primaryBtn:disabled {
            background-color: #bdc3c7;
            color: #95a5a6;
        }

        #secondaryBtn {
            background-color: #ecf0f1;
            color: #34495e;
            border-radius: 4px;
            padding: 8px 16px;
            border: 1px solid #d5dbdb;
            font-weight: bold;
        }
        #secondaryBtn:hover {
            background-color: #d5dbdb;
            border-color: #bdc3c7;
            color: #2c3e50;
        }
        #secondaryBtn:pressed {
            background-color: #c2c9cc;
            border-color: #aeb6bf;
            padding-top: 9px;
            padding-bottom: 7px;
        }
        #secondaryBtn:disabled {
            background-color: #f8f9fa;
            color: #bdc3c7;
            border-color: #ecf0f1;
        }
    )");
}

void ProfileEditDialog::loadCurrentProfile() {
    // 从服务端获取当前用户信息
    QJsonObject result = ApiService::instance()->getUserProfile();
    if (result.value("success").toBool()) {
        QJsonObject data = result.value("data").toObject();
        nicknameEdit->setText(data.value("nickname").toString());
        emailEdit->setText(data.value("email").toString());
        phoneEdit->setText(data.value("phone").toString());

        // 加载头像（如果有）
        QString avatarUrl = data.value("avatar_url").toString();
        if (!avatarUrl.isEmpty()) {
            // 异步加载头像图片
            QNetworkAccessManager *nam = new QNetworkAccessManager(this);
            if (!avatarUrl.startsWith("http")) {
                avatarUrl = "http://127.0.0.1:8080" + avatarUrl;
            }
            connect(nam, &QNetworkAccessManager::finished, [this, nam](QNetworkReply *reply) {
                if (reply->error() == QNetworkReply::NoError) {
                    QPixmap pixmap;
                    pixmap.loadFromData(reply->readAll());
                    if (!pixmap.isNull()) {
                        avatarLabel->setPixmap(pixmap.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    }
                }
                reply->deleteLater();
                nam->deleteLater();
            });
            nam->get(QNetworkRequest(QUrl(avatarUrl)));
        }
    } else {
        QMessageBox::warning(this, "提示", "加载用户信息失败，将使用默认数据");
        // 可回退到默认值或留空
    }
}

void ProfileEditDialog::onSaveProfile() {
    QString nickname = nicknameEdit->text().trimmed();
    QString email = emailEdit->text().trimmed();
    QString phone=phoneEdit->text().trimmed();

    if (nickname.isEmpty()) {
        QMessageBox::warning(this, "提示", "昵称不能为空");
        return;
    }

    // 验证邮箱格式
    if (!email.isEmpty()) {
        QRegularExpression emailRegex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
        if (!emailRegex.match(email).hasMatch()) {
            QMessageBox::warning(this, "提示", "请输入正确的邮箱格式");
            return;
        }
    }

    // 验证手机号格式（如果填写）
    if (!phone.isEmpty()) {
        // 中国大陆手机号：11位数字，以1开头
        QRegularExpression phoneRegex("^1[3-9]\\d{9}$");
        if (!phoneRegex.match(phone).hasMatch()) {
            QMessageBox::warning(this, "提示", "请输入正确的11位手机号码");
            return;
        }
    }

    QJsonObject updates;
    updates["nickname"] = nickname;
    updates["email"] = email;
    updates["phone"]=phone;
    if (!m_newAvatarUrl.isEmpty()) {
        updates["avatar_url"] = m_newAvatarUrl;
    }

    QJsonObject result = ApiService::instance()->updateUserProfile(updates);
    if (result.value("success").toBool()) {
        QMessageBox::information(this, "成功", "资料更新成功");
        emit profileUpdated(); // 让 UserCenterPage 刷新
        accept();
    } else {
        QMessageBox::warning(this, "失败", result.value("error").toString());
    }
}

void ProfileEditDialog::onUploadAvatar() {
    QString fileName = QFileDialog::getOpenFileName(this, "选择头像", "", "Images (*.png *.jpg *.jpeg)");
    if (fileName.isEmpty()) return;

    // 调用上传头像 API
    QJsonObject result = ApiService::instance()->uploadAvatar(fileName);
    if (result.value("success").toBool()) {
        QJsonObject data = result.value("data").toObject();
        QString avatarUrl = data.value("avatar_url").toString();

        // 更新界面上的头像
        QPixmap pixmap(fileName);
        avatarLabel->setPixmap(pixmap.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        avatarLabel->setScaledContents(true);

        // 可选：立即保存到用户资料（但实际应在用户点击“保存修改”时一并提交）
        // 这里可以缓存新头像 URL，在保存时一并提交
        m_newAvatarUrl = avatarUrl;  // 需要在类中添加成员变量
        QMessageBox::information(this, "提示", "头像已上传，保存资料后生效");
    } else {
        QMessageBox::warning(this, "上传失败", result.value("error").toString());
    }
}

void ProfileEditDialog::onChangePassword() {
    QDialog dialog(this);
    dialog.setWindowTitle("修改密码");
    dialog.setMinimumSize(300, 200);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    QLineEdit *oldPwdEdit = new QLineEdit(&dialog);
    oldPwdEdit->setPlaceholderText("原密码");
    oldPwdEdit->setEchoMode(QLineEdit::Password);
    QLineEdit *newPwdEdit = new QLineEdit(&dialog);
    newPwdEdit->setPlaceholderText("新密码（6-20位，含字母+数字）");
    newPwdEdit->setEchoMode(QLineEdit::Password);
    QLineEdit *confirmPwdEdit = new QLineEdit(&dialog);
    confirmPwdEdit->setPlaceholderText("确认新密码");
    confirmPwdEdit->setEchoMode(QLineEdit::Password);

    QPushButton *okBtn = new QPushButton("确定", &dialog);
    QPushButton *cancelBtn = new QPushButton("取消", &dialog);
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(okBtn);

    layout->addWidget(oldPwdEdit);
    layout->addWidget(newPwdEdit);
    layout->addWidget(confirmPwdEdit);
    layout->addLayout(btnLayout);

    connect(okBtn, &QPushButton::clicked, [&](){
        QString oldPwd = oldPwdEdit->text().trimmed();
        QString newPwd = newPwdEdit->text().trimmed();
        QString confirmPwd = confirmPwdEdit->text().trimmed();

        if (oldPwd.isEmpty() || newPwd.isEmpty()) {
            QMessageBox::warning(&dialog, "提示", "请填写完整信息");
            return;
        }
        if (newPwd != confirmPwd) {
            QMessageBox::warning(&dialog, "提示", "两次输入的新密码不一致");
            return;
        }
        if (newPwd.length() < 6 || newPwd.length() > 20) {
            QMessageBox::warning(&dialog, "提示", "密码长度应为6-20位");
            return;
        }
        bool hasLetter = false, hasDigit = false;
        for (QChar ch : newPwd) {
            if (ch.isLetter()) hasLetter = true;
            if (ch.isDigit()) hasDigit = true;
        }
        if (!hasLetter || !hasDigit) {
            QMessageBox::warning(&dialog, "提示", "密码需同时包含字母和数字");
            return;
        }

        QJsonObject result = ApiService::instance()->changePassword(oldPwd, newPwd);
        if (result.value("success").toBool()) {
            QMessageBox::information(&dialog, "成功", "密码修改成功，请重新登录");
            dialog.accept();
        } else {
            QMessageBox::warning(&dialog, "失败", result.value("error").toString());
        }
    });
    connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);

    dialog.exec();
}
