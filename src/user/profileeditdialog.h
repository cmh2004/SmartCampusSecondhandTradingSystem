#ifndef PROFILEEDITDIALOG_H
#define PROFILEEDITDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

class ProfileEditDialog : public QDialog {
    Q_OBJECT

public:
    explicit ProfileEditDialog(QWidget *parent = nullptr);

signals:
    void profileUpdated();

private slots:
    void onSaveProfile();
    void onUploadAvatar();

private:
    void setupUI();
    void loadCurrentProfile();

    // 基本信息
    QLineEdit *nicknameEdit;
    QLineEdit *realNameEdit;
    QLineEdit *studentIdEdit;
    QLineEdit *phoneEdit;
    QLineEdit *emailEdit;
    QComboBox *campusCombo;
    QComboBox *collegeCombo;

    // 头像
    QLabel *avatarLabel;
    QPushButton *uploadAvatarBtn;

    // 安全设置
    QPushButton *changePwdBtn;
    QPushButton *bindPhoneBtn;

    // 按钮
    QPushButton *saveBtn;
    QPushButton *cancelBtn;

    QString m_newAvatarUrl;
};

#endif // PROFILEEDITDIALOG_H
