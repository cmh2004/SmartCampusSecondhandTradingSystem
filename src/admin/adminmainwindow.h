#ifndef ADMINMAINWINDOW_H
#define ADMINMAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QListWidget>
#include <QDateEdit>
#include <QProgressBar>
#include <QChartView>
#include <QGroupBox>
#include <QWidget>
#include <QtCharts/QChart>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QPieSeries>

class AdminMainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit AdminMainWindow(QWidget *parent = nullptr);
    ~AdminMainWindow();

private slots:
    void onLogoutClicked();
    void onTabChanged(int index);

    // 商品审核相关
    void onReviewGoods(int goodsId, bool approve);
    void onFilterGoods();
    void onExportGoodsData();

    // 用户管理相关
    void onBlockUser(const QString &userId, bool block);
    void onAdjustCreditScore(const QString &userId, int score);
    void onSearchUsers();

    // 纠纷处理相关
    void onViewDisputeDetail(int disputeId);
    void onProcessDispute(int disputeId, QString decision);
    void onFilterDisputes();

private:
    void setupUI();

    // 创建各个页面
    QWidget* createDashboardPage();
    QWidget* createGoodsReviewPage();
    QWidget* createUserManagementPage();
    QWidget* createDisputeManagementPage();

    // 加载数据
    void loadDashboardData();
    void loadGoodsReviewData();
    void loadUserManagementData();
    void loadDisputeData();

private:
    // 主标签页
    QTabWidget *mainTabWidget;

    // 仪表盘页面组件
    QLabel *totalUsersLabel;
    QLabel *totalGoodsLabel;
    QLabel *totalOrdersLabel;
    QLabel *pendingReviewsLabel;
    QLabel *pendingDisputesLabel;
    QProgressBar *platformGrowthBar;
    QChartView *revenueChartView;
    QChartView *categoryChartView;

    // 商品审核页面组件
    QTableWidget *goodsReviewTable;
    QLineEdit *goodsSearchEdit;
    QComboBox *goodsStatusCombo;
    QDateEdit *goodsDateFromEdit;
    QDateEdit *goodsDateToEdit;

    // 用户管理页面组件
    QTableWidget *userTable;
    QLineEdit *userSearchEdit;
    QComboBox *userStatusCombo;
    QComboBox *userCreditLevelCombo;

    // 纠纷处理页面组件
    QTableWidget *disputeTable;
    QComboBox *disputeTypeCombo;
    QComboBox *disputeStatusCombo;
    QDateEdit *disputeDateEdit;
};

#endif // ADMINMAINWINDOW_H
