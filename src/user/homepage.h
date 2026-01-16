#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>

class HomePage : public QWidget {
    Q_OBJECT

public:
    explicit HomePage(QWidget *parent = nullptr);
    void loadMockData();

private slots:
    void onCategoryClicked(QListWidgetItem* item);
    void onSearchClicked();
    void onShowGoodsDetail(int row, int column);

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

    QListWidget *categoryList;
    QTableWidget *goodsTable;
    QLineEdit *searchEdit;
    QPushButton *searchBtn;
    QComboBox *sortCombo;
    QLabel *welcomeLabel;
};

#endif // HOMEPAGE_H
