#ifndef USERCENTERPAGE_H
#define USERCENTERPAGE_H

#include <QWidget>
#include <QLabel>
#include <QTabWidget>
#include <QTableWidget>
#include <QListWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

class UserCenterPage : public QWidget {
    Q_OBJECT

public:
    explicit UserCenterPage(QWidget *parent = nullptr);
    void updateUserInfo(const QString &name, int creditScore, const QString &joinDate);
    void loadUserInfo();
    void refreshFavorites();

signals:
    void editProfileRequested();
    void creditScoreRequested();
    void logoutRequested();
    void goodsDetailRequested(int goodsId);

private slots:
    void onEditProfile();
    void onShowCreditScore();
    void onLogout();  // 退出登录槽函数
    void onOffShelf();           // 下架商品
    void onEditGoods();          // 编辑商品
    void onApplyShelve();

private:
    void setupUI();
    void createMyPublishTab();
    void createMyCollectionTab();
    void createReviewTab();
    void createHistoryTab();
    void setupMenuTabs();
    void addReviewItem(const QString &date, const QString &orderId,
                       const QString &item, int rating, const QString &comment);
    void loadMyGoods(int page=1, int pageSize=20);
    void loadFavorites(int page = 1, int pageSize = 10);
    void loadMyReviews(int page = 1, int pageSize = 10);
    void loadBrowseHistory(int page = 1, int pageSize = 10);
    QWidget* createHistoryItem(const QJsonObject &goods);
    QWidget* createEmptyHistoryWidget();   // 创建空历史记录控件

    QLabel *userAvatarLabel;
    QLabel *userNameLabel;
    QLabel *userLevelLabel;
    QLabel *userJoinLabel;
    QTabWidget *userSubTabs;

    // 菜单按钮
    QList<QPushButton*> menuButtons;

    // 子页面
    QWidget *myPublishWidget;
    QWidget *myCollectionWidget;
    QWidget *myReviewWidget;
    QWidget *myHistoryWidget;

    QTableWidget *myGoodsTable;
    QListWidget *collectionList;
    QListWidget *reviewList;
    QWidget *m_historyContainer;          // 浏览历史容器
    QVBoxLayout *m_historyContainerLayout;

    // 我的发布分页
    int m_publishCurrentPage=1;
    int m_publishPageSize = 10;
    QPushButton *m_publishPrevBtn;
    QPushButton *m_publishNextBtn;
    QLabel *m_publishPageLabel;

    // 我的收藏分页
    int m_favCurrentPage=1;
    int m_favPageSize = 10;
    QPushButton *m_favPrevBtn;
    QPushButton *m_favNextBtn;
    QLabel *m_favPageLabel;

    // 评价记录分页
    int m_reviewCurrentPage=1;
    int m_reviewPageSize = 10;
    QPushButton *m_reviewPrevBtn;
    QPushButton *m_reviewNextBtn;
    QLabel *m_reviewPageLabel;

    // 浏览历史分页
    int m_historyCurrentPage=1;
    int m_historyPageSize = 10;
    QPushButton *m_historyPrevBtn;
    QPushButton *m_historyNextBtn;
    QLabel *m_historyPageLabel;
};

#endif // USERCENTERPAGE_H
