#ifndef REGISTERPAGE_H
#define REGISTERPAGE_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QMouseEvent>

class RegisterPage : public QDialog {
    Q_OBJECT

public:
    explicit RegisterPage(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void onTogglePassword();

private:
    void setupUI();
    void setupStyles();

private:
    // 输入控件
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QLineEdit *confirmPasswordEdit;
    QLineEdit *nicknameEdit;

    // 按钮控件
    QPushButton *registerBtn;
    QPushButton *toLoginBtn;
    QPushButton *closeBtn;

    // 其他控件
    QCheckBox *togglePwdBtn;

    // 窗口拖动相关
    bool isDragging;
    QPoint dragStartPosition;
};

#endif // REGISTERPAGE_H
