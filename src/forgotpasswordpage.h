#ifndef FORGOTPASSWORDPAGE_H
#define FORGOTPASSWORDPAGE_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QMouseEvent>
#include <QTimer>
#include <QStackedWidget>

// 前向声明自定义类
class StepIndicator;

class ForgotPasswordPage : public QDialog {
    Q_OBJECT

public:
    explicit ForgotPasswordPage(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void updateCountdown();
    void onResetPasswordClicked();
    void onEnterPressed();

private:
    void setupUI();
    void setupStyles();
    void startCountdown();

    // 创建各个步骤的UI
    QWidget* createStep1Widget();
    QWidget* createStep2Widget();
    QWidget* createStep3Widget();

private:
    // UI控件
    QLineEdit *emailEdit;
    QLineEdit *codeEdit;
    QLineEdit *newPasswordEdit;
    QLineEdit *confirmPasswordEdit;

    QPushButton *sendCodeBtn;
    QPushButton *resetBtn;
    QPushButton *closeBtn;
    QPushButton *nextBtn1;
    QPushButton *nextBtn2;

    QCheckBox *togglePwdBtn;
    QLabel *countdownLabel;
    StepIndicator *stepIndicator;
    QStackedWidget *contentStack;

    QTimer *countdownTimer;
    int countdownSeconds;
    int currentStep;

    bool isDragging;
    QPoint dragStartPosition;
};

#endif // FORGOTPASSWORDPAGE_H
