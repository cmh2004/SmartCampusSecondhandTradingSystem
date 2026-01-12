#ifndef FORGOTPASSWORDPAGE_H
#define FORGOTPASSWORDPAGE_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QTimer>
#include <QMessageBox>

class ForgotPasswordPage : public QDialog {
    Q_OBJECT

public:
    explicit ForgotPasswordPage(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) ;
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

private slots:
    void onSendCodeClicked();
    void onNextClicked();
    void onBackClicked();
    void onResetClicked();
    void onShowPasswordChanged(int state);
    void updateCountdown();
    void onCloseClicked();

private:
    void setupUI();
    void showMessageBox(QWidget *parent, const QString &title, const QString &text, QMessageBox::Icon icon);

private:
    // UI控件
    QLabel *stepLabel;
    QLineEdit *emailEdit;
    QLineEdit *codeEdit;
    QLineEdit *newPasswordEdit;
    QLineEdit *confirmPasswordEdit;
    QPushButton *sendCodeBtn;
    QPushButton *nextBtn;
    QPushButton *backBtn;
    QPushButton *resetBtn;
    QCheckBox *showPasswordCheck;
    QPushButton *closeBtn;

    // 定时器
    QTimer *countdownTimer;
    int countdownSeconds;

    int step; // 0:输入邮箱, 1:输入验证码, 2:设置密码
};

#endif // FORGOTPASSWORDPAGE_H
