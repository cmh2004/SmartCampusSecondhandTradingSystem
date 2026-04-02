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

signals:
    void goodsDetailRequested(int goodsId);
    void searchRequested(const QString &keyword);
    void categoryChanged(const QString &category);
    void reportGoodsRequested(int goodsId);
    void contactSellerRequested(int goodsId);
    void buyNowRequested(int goodsId);
    void makeOfferRequested(int goodsId);

private:
    void setupUI();
    QWidget* createGoodsCard(int goodsId, const QString& name,
                             const QString& price, const QString& category,
                             const QString& status);
    void clearGoodsGrid();
    QString getSortByValue() const;
    QString getCurrentCategory() const;

    QListWidget *categoryList;
    QLineEdit *searchEdit;
    QPushButton *searchBtn;
    QComboBox *sortCombo;
    QLabel *welcomeLabel;

    // 网格布局相关
    QWidget *goodsGridContainer;
    QGridLayout *goodsGridLayout;
};

#endif // HOMEPAGE_H
