#include "mainwindow.h"
#include "LoginPage.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 第一步：显示登录页（模态窗口）
    LoginPage loginPage;
    int loginResult = loginPage.exec(); // 等待登录页关闭，获取结果

    // 第二步：根据登录结果决定程序走向
    if (loginResult == QDialog::Accepted) {
        // 登录成功：创建并显示主窗口，进入事件循环
        MainWindow w;
        w.show();
        return a.exec();
    } else {
        // 登录失败/关闭登录页：直接退出程序
        return 0;
    }
}
