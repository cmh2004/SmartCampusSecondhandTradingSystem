#ifndef REGISTERPAGE_H
#define REGISTERPAGE_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QMessageBox>
#include <QMouseEvent>

class RegisterPage : public QDialog {
    Q_OBJECT

public:
    explicit RegisterPage(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void setupUI();
    void setupStyles();
    void onRegisterClicked();

private slots:
    void onTogglePassword();    // 密码显示/隐藏切换槽函数

private:
    // 输入控件
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QLineEdit *emailEdit;
    QLineEdit *nicknameEdit;

    // 按钮控件
    QPushButton *registerBtn;
    QPushButton *toLoginBtn;
    QPushButton *closeBtn;

    // 窗口拖动相关
    bool isDragging;
    QPoint dragStartPosition;

    QPushButton *togglePwdBtn;  // 密码显示/隐藏按钮
    bool isPasswordVisible;     // 密码可见状态标记
};

#endif // REGISTERPAGE_H
