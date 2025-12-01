#ifndef REGISTERPAGE_H
#define REGISTERPAGE_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>

class RegisterPage : public QDialog {
    Q_OBJECT

public:
    explicit RegisterPage(QWidget *parent = nullptr);

signals:
    void goToLogin();  // 切换到登录页的信号

private:
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QPushButton *registerBtn;
    QPushButton *toLoginBtn;
    QCheckBox *togglePwdBtn;  // 密码显示切换按钮
};

#endif // REGISTERPAGE_H
