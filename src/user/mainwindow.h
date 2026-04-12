#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QPushButton>
#include <QLabel>
#include <QMouseEvent>
#include <QStatusBar>

// 前向声明
class HomePage;
class PublishPage;
class UserCenterPage;
class MessagesPage;
class OrdersPage;
class GoodsDetailDialog;
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
    void onTabChanged(int index);
    void onShowPayment(int orderId, double amount);
    void onShowReview(int orderId, const QString &sellerName);
    void onShowProfileEdit();
    void onShowCreditScore();  // 显示信用分详情

    void onReportGoods(int goodsId);      // 举报商品
    void onReportUser(const QString &userId);    // 举报用户
    void onReportOrder(int orderId);      // 举报订单
    void onReportSubmitted(int targetId, QString targetType); // 举报提交后的处理

    void onShowDisputeSubmit(int orderId);  // 显示售后纠纷对话框

    void onLogout();

private:
    void setupUI();
    void setupCustomTitleBar();
    void setActiveTabButton(int index);  // 设置活动标签按钮

private:
    QWidget *mainWidget;
    QTabWidget *mainTabWidget;

    // 页面实例
    HomePage *homePage;
    PublishPage *publishPage;
    MessagesPage *messagesPage;
    OrdersPage *ordersPage;
    UserCenterPage *userCenterPage;

    // 自定义标题栏控件
    QWidget *customTitleBar;
    QLabel *titleLabel;
    QPushButton *minimizeBtn;
    QPushButton *maximizeBtn;
    QPushButton *closeBtn;

    // 标签按钮列表
    QList<QPushButton*> tabButtons;

    // 用于窗口拖拽
    bool isDragging;
    QPoint dragStartPosition;

    CreditScoreDialog *creditScoreDialog = nullptr;  // 信用分对话框
    ReportSubmitDialog *reportDialog = nullptr;
    DisputeSubmitDialog *disputeDialog = nullptr;  // 纠纷对话框指针
};

#endif // MAINWINDOW_H
