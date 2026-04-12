#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QMouseEvent>
#include <QComboBox>
#include <QApplication>

class LoginPage : public QDialog {
    Q_OBJECT

public:
    explicit LoginPage(QWidget *parent = nullptr);
    QString getSelectedRole() const { return selectedRole; }

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void onTogglePassword();
    void onLoginClicked();
    void onRegisterClicked();
    void onForgotPasswordClicked();
    void onRoleChanged(int index);

private:
    void setupUI();
    void setupStyles();
    QWidget* createLeftPanel();
    QWidget* createRightPanel();
    void saveLoginInfo(const QString& username, const QString& password, bool remember, bool autoLogin);
    void loadLoginInfo();
    void attemptAutoLogin();

private:
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginBtn;
    QPushButton *toRegisterBtn;
    QPushButton *togglePwdBtn;
    QPushButton *closeBtn;
    QPushButton *minimizeBtn;
    QPushButton *forgotBtn;
    QComboBox *roleCombo;

    // 用于窗口拖动
    bool isDragging;
    QPoint dragStartPosition;

    QString selectedRole;

    QCheckBox *rememberMeCheckBox;
    QCheckBox *autoLoginCheckBox;
};

#endif // LOGINPAGE_H
