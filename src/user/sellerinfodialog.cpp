#include "SellerInfoDialog.h"
#include "../apiservice.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QHeaderView>
#include <QFormLayout>
#include <QMessageBox>

SellerInfoDialog::SellerInfoDialog(int sellerId, const QString &sellerName, QWidget *parent)
    : QDialog(parent), m_sellerId(sellerId), m_sellerName(sellerName)
{
    setWindowTitle(QString("卖家信息 - %1").arg(sellerName));
    setMinimumSize(700, 600);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // ========== 基本信息区域 ==========
    QWidget *infoWidget = new QWidget();
    QFormLayout *infoLayout = new QFormLayout(infoWidget);
    infoLayout->setSpacing(8);

    m_nameLabel = new QLabel();
    m_creditLabel = new QLabel();
    m_schoolLabel = new QLabel();
    m_phoneLabel = new QLabel();

    infoLayout->addRow("昵称:", m_nameLabel);
    infoLayout->addRow("信用分:", m_creditLabel);
    infoLayout->addRow("学校:", m_schoolLabel);
    infoLayout->addRow("联系方式:", m_phoneLabel);

    mainLayout->addWidget(infoWidget);

    // ========== 评价区域 ==========
    QLabel *reviewTitle = new QLabel("买家评价");
    reviewTitle->setStyleSheet("font-weight: bold; margin-top: 10px;");
    mainLayout->addWidget(reviewTitle);

    m_reviewTable = new QTableWidget(0, 5);
    m_reviewTable->setHorizontalHeaderLabels({"评分", "评价内容", "评价人", "商品名称", "时间"});
    m_reviewTable->horizontalHeader()->setStretchLastSection(true);
    m_reviewTable->setEditTriggers(QTableWidget::NoEditTriggers);
    m_reviewTable->setSelectionBehavior(QTableWidget::SelectRows);
    m_reviewTable->setAlternatingRowColors(true);
    m_reviewTable->verticalHeader()->setVisible(false);
    m_reviewTable->setColumnWidth(0, 80);
    m_reviewTable->setColumnWidth(1, 250);

    mainLayout->addWidget(m_reviewTable);

    // 关闭按钮
    QPushButton *closeBtn = new QPushButton("关闭");
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    mainLayout->addWidget(closeBtn, 0, Qt::AlignCenter);

    // 加载数据
    loadSellerInfo();
    loadReviews();
}

void SellerInfoDialog::loadSellerInfo()
{
    QJsonObject result = ApiService::instance()->getUserProfile(QString::number(m_sellerId));
    if (result.value("success").toBool()) {
        QJsonObject data = result.value("data").toObject();
        m_nameLabel->setText(data.value("nickname").toString());
        m_creditLabel->setText(QString::number(data.value("credit_score").toInt()));
        m_schoolLabel->setText(data.value("school").toString());
        m_phoneLabel->setText(data.value("phone").toString());
    } else {
        m_nameLabel->setText(m_sellerName);
        m_creditLabel->setText("获取失败");
        m_schoolLabel->setText("未知");
        m_phoneLabel->setText("未知");
    }
}

void SellerInfoDialog::loadReviews()
{
    QJsonObject result = ApiService::instance()->getSellerReviews(QString::number(m_sellerId), 1, 20);
    if (!result.value("success").toBool()) {
        QMessageBox::warning(this, "提示", "获取评价失败：" + result.value("error").toString());
        return;
    }

    QJsonArray reviews = result.value("data").toArray();
    m_reviewTable->setRowCount(0);

    for (const QJsonValue &val : reviews) {
        QJsonObject review = val.toObject();
        int score = review.value("score").toString().toInt();
        QString content = review.value("content").toString();
        QString buyerName = review.value("buyer_nickname").toString();
        QString goodsName = review.value("goods_name").toString();
        QString time = review.value("create_time").toString().left(10);

        int row = m_reviewTable->rowCount();
        m_reviewTable->insertRow(row);

        m_reviewTable->setItem(row, 0, new QTableWidgetItem(QString::number(score) + "分"));
        m_reviewTable->setItem(row, 1, new QTableWidgetItem(content));
        m_reviewTable->setItem(row, 2, new QTableWidgetItem(buyerName));
        m_reviewTable->setItem(row, 3, new QTableWidgetItem(goodsName));
        m_reviewTable->setItem(row, 4, new QTableWidgetItem(time));
    }

    if (reviews.isEmpty()) {
        m_reviewTable->setRowCount(1);
        m_reviewTable->setSpan(0, 0, 1, 5);
        m_reviewTable->setItem(0, 0, new QTableWidgetItem("暂无评价"));
    }
}
