#ifndef FORGOTPASSWORDPAGE_H
#define FORGOTPASSWORDPAGE_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QTimer>

class ForgotPasswordPage : public QDialog {
    Q_OBJECT

public:
    explicit ForgotPasswordPage(QWidget *parent = nullptr);

private slots:
    void onSendCodeClicked();
    void onNextClicked();
    void onBackClicked();
    void onResetClicked();
    void onShowPasswordChanged(int state);
    void updateCountdown();

private:
    void setupUI();

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

    // 定时器
    QTimer *countdownTimer;
    int countdownSeconds;
};

#endif // FORGOTPASSWORDPAGE_H
