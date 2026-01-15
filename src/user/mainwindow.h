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
#include <QMouseEvent>
#include <QStatusBar>
#include <QApplication>

// 前向声明
class GoodsDetailDialog;
class ChatDialog;
class DisputeSubmitDialog;
class PaymentDialog;
class ReviewDialog;
class ProfileEditDialog;
class CreditScoreDialog;
class ReportSubmitDialog;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private slots:
    void onCategoryClicked(QListWidgetItem* item);
    void onSearchClicked();
    void onPublishGoods();
    void onShowGoodsDetail(int row, int column);
    void onTabChanged(int index);

    void onShowPayment();
    void onShowReview();
    void onShowProfileEdit();
    void onReviewSubmitted(int orderId, int rating, QString comment);
    void onProfileUpdated();
    void onShowCreditScore();  // 显示信用分详情

    void onReportGoods(int goodsId);      // 举报商品
    void onReportUser(QString userId);    // 举报用户
    void onReportOrder(int orderId);      // 举报订单
    void onReportSubmitted(int targetId, QString targetType); // 举报提交后的处理

    void onShowDisputeSubmit(int orderId);  // 显示售后纠纷对话框
    void onDisputeSubmitted(int orderId);   // 纠纷提交后的处理
    void loadOrderExamples();  // 加载订单示例数据
    void filterOrders(const QString &status, const QString &keyword); // 筛选订单
    void onCancelOrder(int orderId);  // 取消订单
    void onConfirmReceipt(int orderId);  // 确认收货
    void onViewDisputeDetail(int orderId);  // 查看纠纷详情

private:
    void setupUI();
    void loadMockData();

    void setupCustomTitleBar();
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
    QPushButton *editProfileBtn;

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

    // 自定义标题栏控件
    QWidget *customTitleBar;
    QLabel *titleLabel;
    QPushButton *minimizeBtn;
    QPushButton *maximizeBtn;
    QPushButton *closeBtn;

    // 用于窗口拖拽
    bool isDragging;
    QPoint dragStartPosition;

    CreditScoreDialog *creditScoreDialog = nullptr;  // 信用分对话框
    ReportSubmitDialog *reportDialog = nullptr;
    DisputeSubmitDialog *disputeDialog = nullptr;  // 纠纷对话框指针
};

#endif // MAINWINDOW_H
