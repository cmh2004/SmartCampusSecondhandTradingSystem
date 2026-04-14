#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QGridLayout>

class HomePage : public QWidget {
    Q_OBJECT

public:
    explicit HomePage(QWidget *parent = nullptr);
    void loadGoodsFromServer(const QString &keyword, const QString &category,
                                   double minPrice, double maxPrice, const QString &sortBy,
                             int page, int pageSize);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void onCategoryClicked(QListWidgetItem* item);
    void onSearchClicked();
    void onAISearchClicked();

signals:
    void goodsDetailRequested(int goodsId);
    void reportGoodsRequested(int goodsId);
    void contactSellerRequested(int goodsId);
    void buyNowRequested(int goodsId);
    void makeOfferRequested(int goodsId);

private:
    void setupUI();
    QWidget* createGoodsCard(int goodsId, const QString& name,
                             const QString& price, const QString& category,
                             const QString& status, const QString& imageUrl="");
    void clearGoodsGrid();
    QString getSortByValue() const;
    QString getCurrentCategory() const;
    void updatePageButtons();
    void goToPrevPage();
    void goToNextPage();

    QListWidget *categoryList;
    QLineEdit *searchEdit;
    QPushButton *searchBtn;
    QComboBox *sortCombo;
    QLabel *welcomeLabel;

    int m_currentPage;
    int m_totalPages;  // 总页数（可选，可从服务端返回）
    QPushButton *prevPageBtn;
    QPushButton *nextPageBtn;
    QLabel *pageInfoLabel;

    // 网格布局相关
    QWidget *goodsGridContainer;
    QGridLayout *goodsGridLayout;
};

#endif // HOMEPAGE_H
