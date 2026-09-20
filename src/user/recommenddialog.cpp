#include "RecommendDialog.h"
#include "../apiservice.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPixmap>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>

RecommendDialog::RecommendDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("🤖 为你推荐 · 猜你喜欢");
    setMinimumSize(800, 600);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setupUI();
    loadRecommendations();
}

void RecommendDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(10);

    // 标题区域
    QWidget *titleWidget = new QWidget();
    QHBoxLayout *titleLayout = new QHBoxLayout(titleWidget);
    QLabel *iconLabel = new QLabel("🔥");
    iconLabel->setStyleSheet("font-size: 28px;");
    QLabel *titleLabel = new QLabel("为你推荐 · 猜你喜欢");
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #e67e22;");
    m_refreshBtn = new QPushButton("刷新");
    m_refreshBtn->setFixedSize(80, 32);
    m_refreshBtn->setStyleSheet("background-color: #f39c12; color: white; border-radius: 6px;");
    titleLayout->addWidget(iconLabel);
    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();
    titleLayout->addWidget(m_refreshBtn);
    mainLayout->addWidget(titleWidget);

    // 滚动区域 + 网格容器
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("border: none; background-color: #f8fafc;");
    m_gridContainer = new QWidget();
    m_gridLayout = new QGridLayout(m_gridContainer);
    m_gridLayout->setContentsMargins(10, 10, 10, 10);
    m_gridLayout->setSpacing(15);
    m_gridLayout->setAlignment(Qt::AlignTop);
    scrollArea->setWidget(m_gridContainer);
    mainLayout->addWidget(scrollArea, 1);

    // 底部提示
    m_statusLabel = new QLabel("正在为您推荐...");
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setStyleSheet("color: #94a3b8; padding: 10px;");
    mainLayout->addWidget(m_statusLabel);

    // 连接刷新按钮
    connect(m_refreshBtn, &QPushButton::clicked, this, &RecommendDialog::loadRecommendations);
}

void RecommendDialog::loadRecommendations()
{
    clearGrid();
    m_statusLabel->setText("正在加载推荐...");
    m_refreshBtn->setEnabled(false);

    QJsonArray recList = ApiService::instance()->getCollaborativeRecommendations(12);
    if (recList.isEmpty()) {
        m_statusLabel->setText("暂无推荐，快去浏览或收藏商品吧~");
        m_refreshBtn->setEnabled(true);
        return;
    }

    int columns = 3;  // 每行3个卡片
    for (int i = 0; i < recList.size(); ++i) {
        QJsonObject goods = recList[i].toObject();
        int goodsId = goods.value("id").toInt();
        QString name = goods.value("name").toString();
        double price = goods.value("price").toDouble();
        QString imageUrl = goods.value("image_url").toString();

        QWidget *card = createRecommendCard(goodsId, name, price, imageUrl);
        int row = i / columns;
        int col = i % columns;
        m_gridLayout->addWidget(card, row, col);
    }

    m_statusLabel->setText(QString("找到 %1 件推荐商品").arg(recList.size()));
    m_refreshBtn->setEnabled(true);
}

void RecommendDialog::clearGrid()
{
    QLayoutItem *child;
    while ((child = m_gridLayout->takeAt(0)) != nullptr) {
        if (child->widget()) delete child->widget();
        delete child;
    }
}

QWidget* RecommendDialog::createRecommendCard(int goodsId, const QString &name, double price, const QString &imageUrl)
{
    QWidget *card = new QWidget();
    card->setFixedSize(220, 280);
    card->setCursor(Qt::PointingHandCursor);
    card->setProperty("goodsId", goodsId);
    card->setStyleSheet(R"(
        QWidget {
            background-color: white;
            border-radius: 12px;
            border: 1px solid #e2e8f0;
        }
        QWidget:hover {
            border-color: #f39c12;
            box-shadow: 0 4px 12px rgba(0,0,0,0.1);
        }
    )");

    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(8);

    // 图片
    QLabel *imageLabel = new QLabel();
    imageLabel->setFixedSize(196, 150);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setStyleSheet("background-color: #f8fafc; border-radius: 8px;");
    if (!imageUrl.isEmpty()) {
        QString fullUrl = "http://127.0.0.1:8080" + imageUrl;
        QNetworkAccessManager *nam = new QNetworkAccessManager(this);
        connect(nam, &QNetworkAccessManager::finished, [imageLabel, nam](QNetworkReply *reply) {
            if (reply->error() == QNetworkReply::NoError) {
                QPixmap pixmap;
                pixmap.loadFromData(reply->readAll());
                if (!pixmap.isNull()) {
                    imageLabel->setPixmap(pixmap.scaled(196, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                }
            }
            reply->deleteLater();
            nam->deleteLater();
        });
        nam->get(QNetworkRequest(QUrl(fullUrl)));
    } else {
        imageLabel->setPixmap(QPixmap(":/icons/img/buy.png").scaled(100, 100, Qt::KeepAspectRatio));
    }

    // 名称
    QLabel *nameLabel = new QLabel(name);
    nameLabel->setWordWrap(true);
    nameLabel->setMaximumHeight(50);
    nameLabel->setStyleSheet("font-size: 14px; font-weight: 500; color: #1e293b;");

    // 价格
    QLabel *priceLabel = new QLabel(QString("¥%1").arg(price));
    priceLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #e67e22;");

    // 查看按钮
    QPushButton *viewBtn = new QPushButton("查看详情");
    viewBtn->setFixedHeight(32);
    viewBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #f39c12;
            color: white;
            border-radius: 6px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #e67e22;
        }
    )");

    layout->addWidget(imageLabel);
    layout->addWidget(nameLabel);
    layout->addWidget(priceLabel);
    layout->addWidget(viewBtn);

    // 点击按钮或卡片都触发查看详情
    connect(viewBtn, &QPushButton::clicked, [this, goodsId]() {
        emit goodsDetailRequested(goodsId);
    });
    card->installEventFilter(this);

    return card;
}
