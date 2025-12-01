#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>

class LoginPage : public QDialog {
    Q_OBJECT

public:
    explicit LoginPage(QWidget *parent = nullptr);

signals:
    void goToRegister();  // 触发切换到注册页

private:
    QLineEdit *usernameEdit;  // 用户名输入框
    QLineEdit *passwordEdit;  // 密码输入框
    QPushButton *loginBtn;    // 登录按钮
    QPushButton *toRegisterBtn; // 去注册按钮
    QCheckBox *togglePwdBtn; // 密码显示切换按钮
};

#endif // LOGINPAGE_H
