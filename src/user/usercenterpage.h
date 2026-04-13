#ifndef USERCENTERPAGE_H
#define USERCENTERPAGE_H

#include <QWidget>
#include <QLabel>
#include <QTabWidget>
#include <QTableWidget>
#include <QListWidget>
#include <QTextEdit>
#include <QPushButton>

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
    void loadFavorites();
    void addReviewItem(const QString &date, const QString &orderId,
                       const QString &item, int rating, const QString &comment);
    void loadMyGoods();
    void loadMyReviews();
    void loadBrowseHistory();
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
};

#endif // USERCENTERPAGE_H
