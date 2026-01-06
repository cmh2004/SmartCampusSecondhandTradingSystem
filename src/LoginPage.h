#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QMouseEvent>
#include "ForgotPasswordPage.h"

class LoginPage : public QDialog {
    Q_OBJECT

public:
    explicit LoginPage(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void onTogglePassword();
    void onLoginClicked();
    void onRegisterClicked();
    void onForgotPasswordClicked();

private:
    void setupUI();
    void setupStyles();
    QWidget* createTitleBar();
    QWidget* createLeftPanel();
    QWidget* createRightPanel();

private:
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginBtn;
    QPushButton *toRegisterBtn;
    QPushButton *togglePwdBtn;
    QPushButton *closeBtn;
    QPushButton *minimizeBtn;
    QPushButton *forgotBtn;

    // 用于窗口拖动
    bool isDragging;
    QPoint dragStartPosition;
};

#endif // LOGINPAGE_H
