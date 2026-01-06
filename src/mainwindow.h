// MainWindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QListWidget>
#include <QTableWidget>
#include <QMenuBar>
#include <QToolBar>
#include <QLineEdit>
#include <QDialog>
#include <QPushButton>
#include <QComboBox>
#include <QStackedWidget>
#include <QLabel>
#include <QTreeWidget>
#include <QTextEdit>
#include <QFormLayout>
#include <QStatusBar>

// 前向声明，避免循环依赖
class GoodsDetailDialog;
class ChatDialog;
class DisputeSubmitDialog;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onCategoryClicked(QListWidgetItem* item);
    void onSearchClicked();
    void onPublishGoods();
    void onShowGoodsDetail(int row, int column);
    void onTabChanged(int index);
    void onLogout();

private:
    void setupUI();
    void loadMockData();

    // 添加页面创建函数的声明
    QWidget* createHomePage();
    QWidget* createPublishPage();
    QWidget* createUserCenterPage();
    QWidget* createMessagesPage();
    QWidget* createOrdersPage();

private:
    QWidget *mainWidget;
    QTabWidget *mainTabWidget;

    // 首页相关
    QWidget *homePage;
    QListWidget *categoryList;
    QTableWidget *goodsTable;
    QLineEdit *searchEdit;
    QPushButton *searchBtn;
    QComboBox *sortCombo;
    QLabel *welcomeLabel;

    // 发布商品页面
    QWidget *publishPage;
    QLineEdit *goodsNameEdit;
    QComboBox *goodsCategoryCombo;
    QLineEdit *goodsPriceEdit;
    QTextEdit *goodsDescEdit;
    QPushButton *uploadImageBtn;
    QLabel *imagePreview;

    // 个人中心页面
    QWidget *userCenterPage;
    QLabel *userAvatarLabel;
    QLabel *userNameLabel;
    QTreeWidget *orderTree;
    QTabWidget *userSubTabs;

    // 消息页面
    QWidget *messagesPage;
    QListWidget *chatList;
    QTextEdit *chatArea;
    QLineEdit *messageEdit;

    // 订单页面
    QWidget *ordersPage;
    QTableWidget *ordersTable;

    // 菜单栏
    QMenu *userMenu;
    QMenu *helpMenu;

    // 工具栏
    QToolBar *mainToolBar;
};

#endif // MAINWINDOW_H
