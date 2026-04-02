#include "user/mainwindow.h"
#include "admin/adminmainwindow.h"
#include "loginpage.h"
#include "apiservice.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QFont font("Microsoft YaHei", 9); // 字体：微软雅黑，默认字号9
    a.setFont(font);
    a.setWindowIcon(QIcon(":/icons/img/app.png"));

    ApiService::instance()->initialize("http://127.0.0.1:8080", "ws://127.0.0.1:8081");

    // 显示登录页
    LoginPage loginPage;
    if (loginPage.exec() == QDialog::Accepted) {
        // 获取选择的角色
        QString selectedRole = loginPage.getSelectedRole();

        if (selectedRole == "admin") {
            // 进入管理员系统
            AdminMainWindow adminWindow;
            adminWindow.show();
            return a.exec();
        } else {
            // 进入普通用户系统
            MainWindow userWindow;
            userWindow.show();
            return a.exec();
        }
    }

    return 0;
}
