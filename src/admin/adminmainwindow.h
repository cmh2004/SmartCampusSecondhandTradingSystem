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
#include <QGroupBox>
#include <QWidget>

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

    // 用户管理相关
    void onBlockUser(const QString &userId, bool block);
    void onAdjustCreditScore(int userId, int score);
    void onSearchUsers();

    // 纠纷处理相关
    void onViewDisputeDetail(int disputeId);
    void onProcessDispute(int disputeId, const QString& decision);
    void onFilterDisputes();

private:
    void setupUI();

    // 创建各个页面
    QWidget* createGoodsReviewPage();
    QWidget* createUserManagementPage();
    QWidget* createDisputeManagementPage();

    // 加载数据
    void loadGoodsReviewData(const QString& keyword, const QString& status,
                             const QString& startDate, const QString& endDate,
                             int page = 1, int pageSize = 20);
    void loadUserManagementData(const QString& keyword = "", const QString& status = "", int page = 1, int pageSize = 20);
    void loadDisputeData(const QString& status = "", int page = 1, int pageSize = 20);

    void onOffShelfGoods(int goodsId);      // 下架商品
    void onReapplyGoods(int goodsId);       // 已拒绝商品重新申请审核
    void onApplyShelfGoods(int goodsId);    // 已下架商品申请上架

    QString getDisputeStatusParam();
    QString getCategoryName(int categoryId);

private:
    // 主标签页
    QTabWidget *mainTabWidget;

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

    // 纠纷处理页面组件
    QTableWidget *disputeTable;
    QComboBox *disputeStatusCombo;

    QString m_currentKeyword;
    QString m_currentStatus;
    QString m_currentStartDate;
    QString m_currentEndDate;

    int m_goodsCurrentPage;
    int m_goodsTotalPages;
    QPushButton *goodsPrevBtn;
    QPushButton *goodsNextBtn;
    QLabel *goodsPageInfoLabel;

    // 用户管理分页
    int m_userCurrentPage;
    int m_userPageSize = 20;
    QPushButton *userPrevBtn;
    QPushButton *userNextBtn;
    QLabel *userPageInfoLabel;

    // 纠纷处理分页
    int m_disputeCurrentPage;
    int m_disputePageSize = 20;
    QPushButton *disputePrevBtn;
    QPushButton *disputeNextBtn;
    QLabel *disputePageInfoLabel;
};

#endif // ADMINMAINWINDOW_H
