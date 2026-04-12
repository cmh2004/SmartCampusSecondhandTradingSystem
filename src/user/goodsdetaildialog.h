#ifndef GOODSDETAILDIALOG_H
#define GOODSDETAILDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTabWidget>

class GoodsDetailDialog : public QDialog {
    Q_OBJECT

public:
    explicit GoodsDetailDialog(QWidget *parent = nullptr, int goodsId = -1);

    int getGoodsId() const { return goodsId; }

signals:
    void contactSellerRequested(int goodsId, const QString &sellerName);
    void buyNowRequested(int goodsId);
    void reportGoodsRequested(int goodsId);

private slots:
    void onCollectGoods();
    void onAIAssessment();
    void onShowRiskAssessment();

private:
    void setupUI();
    void loadGoodsData(int goodsId);
    void loadAIAssessment(int goodsId);
    QString getCategoryName(int categoryId);

private:
    int goodsId;

    // 商品信息
    QLabel *goodsImageLabel;
    QLabel *goodsTitleLabel;
    QLabel *priceLabel;
    QLabel *sellerLabel;
    QLabel *contactLabel;
    QLabel *locationLabel;
    QLabel *publishTimeLabel;
    QLabel *conditionLabel;
    QLabel *categoryLabel;
    QTextEdit *descriptionText;

    // AI评估信息
    QLabel *aiPriceRangeLabel;
    QLabel *aiConditionLabel;
    QLabel *aiBrandLabel;
    QLabel *aiRiskLevelLabel;
    QLabel *aiRecommendationLabel;

    // 按钮
    QPushButton *contactBtn;
    QPushButton *buyBtn;
    QPushButton *collectBtn;
    QPushButton *aiAssessmentBtn;
    QPushButton *riskBtn;
    QPushButton *reportBtn;

    // 标签页
    QTabWidget *detailTabs;

    QList<QLabel*> m_thumbnailLabels;   // 存储缩略图控件

    bool m_isFavorited;  // 当前商品是否已收藏
};

#endif // GOODSDETAILDIALOG_H
