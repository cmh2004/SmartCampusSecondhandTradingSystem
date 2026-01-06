#ifndef FORGOTPASSWORDPAGE_H
#define FORGOTPASSWORDPAGE_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QMouseEvent>

class ForgotPasswordPage : public QDialog {
    Q_OBJECT

public:
    explicit ForgotPasswordPage(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void onSendCodeClicked();
    void onResetPasswordClicked();
    void onBackToLoginClicked();

private:
    void setupUI();
    void setupStyles();
    void startCountdown();
    void updateCountdown();

private:
    // 输入控件
    QLineEdit *emailEdit;
    QLineEdit *codeEdit;
    QLineEdit *newPasswordEdit;
    QLineEdit *confirmPasswordEdit;

    // 按钮控件
    QPushButton *sendCodeBtn;
    QPushButton *resetBtn;
    QPushButton *backBtn;
    QPushButton *closeBtn;

    // 其他控件
    QCheckBox *togglePwdBtn;
    QLabel *countdownLabel;
    QLabel *stepIndicator;

    // 验证码倒计时
    QTimer *countdownTimer;
    int countdownSeconds;

    // 窗口拖动相关
    bool isDragging;
    QPoint dragStartPosition;

    // 当前步骤
    int currentStep; // 1: 输入邮箱, 2: 输入验证码, 3: 设置新密码
};

#endif // FORGOTPASSWORDPAGE_H
