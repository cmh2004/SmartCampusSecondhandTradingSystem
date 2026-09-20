#include "imageviewerdialog.h"
#include <QScrollArea>
#include <QVBoxLayout>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPushButton>
#include <QScreen>
#include <QGuiApplication>

ImageViewerDialog::ImageViewerDialog(const QPixmap &pixmap, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("查看图片");
    setMinimumSize(400, 400);
    setMaximumSize(1400, 850);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QVBoxLayout *layout = new QVBoxLayout(this);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);

    QLabel *imageLabel = new QLabel();
    imageLabel->setPixmap(pixmap.scaled(pixmap.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setScaledContents(true);
    scrollArea->setWidget(imageLabel);

    layout->addWidget(scrollArea);

    QPushButton *closeBtn = new QPushButton("关闭");
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    layout->addWidget(closeBtn, 0, Qt::AlignCenter);

    // 根据图片大小调整窗口
    int newWidth = qMin(pixmap.width() + 50, maximumWidth());
    int newHeight = qMin(pixmap.height() + 100, maximumHeight());
    resize(newWidth, newHeight);

    // 移动到屏幕中央（在最终大小确定后）
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenRect = screen->geometry();
        move(screenRect.center() - rect().center());
    }
}

ImageViewerDialog::ImageViewerDialog(const QString &imageUrl, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("加载图片...");
    setMinimumSize(400, 400);
    setMaximumSize(1400, 850);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QVBoxLayout *layout = new QVBoxLayout(this);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);

    QLabel *imageLabel = new QLabel("加载中...");
    imageLabel->setAlignment(Qt::AlignCenter);
    scrollArea->setWidget(imageLabel);
    layout->addWidget(scrollArea);

    QPushButton *closeBtn = new QPushButton("关闭");
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    layout->addWidget(closeBtn, 0, Qt::AlignCenter);

    // 异步加载图片
    QNetworkAccessManager *nam = new QNetworkAccessManager(this);
    connect(nam, &QNetworkAccessManager::finished, [this, imageLabel, nam](QNetworkReply *reply) {
        if (reply->error() == QNetworkReply::NoError) {
            QPixmap pixmap;
            pixmap.loadFromData(reply->readAll());
            if (!pixmap.isNull()) {
                imageLabel->setPixmap(pixmap.scaled(pixmap.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
                imageLabel->setScaledContents(true);
                setWindowTitle("查看图片");

                // 调整窗口大小
                int newWidth = qMin(pixmap.width() + 50, maximumWidth());
                int newHeight = qMin(pixmap.height() + 100, maximumHeight());
                resize(newWidth, newHeight);

                // 重新居中
                QScreen *screen = QGuiApplication::primaryScreen();
                if (screen) {
                    QRect screenRect = screen->geometry();
                    move(screenRect.center() - rect().center());
                }
            } else {
                imageLabel->setText("图片无法显示");
            }
        } else {
            imageLabel->setText("图片加载失败");
        }
        reply->deleteLater();
        nam->deleteLater();
    });
    nam->get(QNetworkRequest(QUrl(imageUrl)));

    // 初始默认大小
    resize(500, 500);
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenRect = screen->geometry();
        move(screenRect.center() - rect().center());
    }
}
