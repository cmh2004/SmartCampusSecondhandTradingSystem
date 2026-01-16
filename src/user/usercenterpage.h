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

signals:
    void editProfileRequested();
    void creditScoreRequested();
    void menuTabChanged(int index);

private slots:
    void onEditProfile();
    void onShowCreditScore();

private:
    void setupUI();
    void createMyPublishTab();
    void createMyCollectionTab();
    void createReviewTab();
    void createHistoryTab();
    void setupMenuTabs() ;

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
    QTextEdit *reviewEdit;
};

#endif // USERCENTERPAGE_H
