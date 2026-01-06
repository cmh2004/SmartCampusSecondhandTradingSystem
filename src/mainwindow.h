// mainwindow.h - 添加以下内容

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QListWidget>
#include <QTableWidget>
#include <QMenuBar>
#include <QToolBar>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QStackedWidget>
#include <QLabel>
#include <QTreeWidget>
#include <QTextEdit>
#include <QFormLayout>
#include <QStatusBar>

// 前向声明
class GoodsDetailDialog;
class ChatDialog;
class DisputeSubmitDialog;
class PaymentDialog;
class ReviewDialog;
class ProfileEditDialog;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // 已有槽函数
    void onCategoryClicked(QListWidgetItem* item);
    void onSearchClicked();
    void onPublishGoods();
    void onShowGoodsDetail(int row, int column);
    void onTabChanged(int index);

    // 新添加的槽函数
    void onShowPayment();
    void onShowReview();
    void onShowProfileEdit();
    void onReviewSubmitted(int orderId, int rating, QString comment);
    void onProfileUpdated();

private:
    void setupUI();
    void loadMockData();

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
    QPushButton *editProfileBtn; // 添加编辑资料按钮引用

    // 消息页面
    QWidget *messagesPage;
    QListWidget *chatList;
    QTextEdit *chatArea;
    QLineEdit *messageEdit;

    // 订单页面
    QWidget *ordersPage;
    QTableWidget *ordersTable;
    QPushButton *paymentBtn;     // 支付按钮
    QPushButton *reviewBtn;      // 评价按钮

    // 菜单栏
    QMenu *userMenu;
    QMenu *helpMenu;

    // 工具栏
    QToolBar *mainToolBar;
};

#endif // MAINWINDOW_H
