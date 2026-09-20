#include "DisputeDetailDialog.h"
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPixmap>
#include <QFileInfo>

DisputeDetailDialog::DisputeDetailDialog(const QJsonObject &dispute, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("纠纷详情");
    setMinimumSize(600, 550);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setupUI(dispute);
}

void DisputeDetailDialog::setupUI(const QJsonObject &dispute)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);

    // 基本信息组
    QGroupBox *infoGroup = new QGroupBox("基本信息");
    QGridLayout *infoLayout = new QGridLayout(infoGroup);

    QString type = dispute.value("type").toString();
    int status = dispute.value("status").toString().toInt();
    QString statusText;
    if (status == 0) statusText = "待处理";
    else if (status == 1) statusText = "处理中";
    else if (status == 2) statusText = "已解决";
    else statusText = "已关闭";

    QString createTime = dispute.value("create_time").toString();

    infoLayout->addWidget(new QLabel("纠纷类型:"), 0, 0);
    infoLayout->addWidget(new QLabel(type), 0, 1);
    infoLayout->addWidget(new QLabel("处理状态:"), 1, 0);
    infoLayout->addWidget(new QLabel(statusText), 1, 1);
    infoLayout->addWidget(new QLabel("提交时间:"), 2, 0);
    infoLayout->addWidget(new QLabel(createTime), 2, 1);
    infoLayout->setColumnStretch(1, 1);
    mainLayout->addWidget(infoGroup);

    // 纠纷描述组
    QGroupBox *descGroup = new QGroupBox("纠纷描述");
    QVBoxLayout *descLayout = new QVBoxLayout(descGroup);
    QTextEdit *descEdit = new QTextEdit();
    descEdit->setPlainText(dispute.value("description").toString());
    descEdit->setReadOnly(true);
    descLayout->addWidget(descEdit);
    mainLayout->addWidget(descGroup);

    // 证据图片组
    QString evidenceUrls = dispute.value("evidence_urls").toString();
    if (!evidenceUrls.isEmpty()) {
        QGroupBox *evidenceGroup = new QGroupBox("证据材料");
        QVBoxLayout *evidenceLayout = new QVBoxLayout(evidenceGroup);

        // 使用 QTabWidget 显示多张图片
        QTabWidget *tabWidget = new QTabWidget();
        QStringList urls = evidenceUrls.split(',', Qt::SkipEmptyParts);
        for (const QString &url : urls) {
            QLabel *imageLabel = new QLabel();
            imageLabel->setAlignment(Qt::AlignCenter);
            imageLabel->setMinimumSize(400, 300);
            imageLabel->setStyleSheet("border: 1px solid #ddd; background-color: #f8f9fa;");

            QString fullUrl = url.startsWith("http") ? url : "http://127.0.0.1:8080" + url;
            QNetworkAccessManager *nam = new QNetworkAccessManager(this);
            connect(nam, &QNetworkAccessManager::finished, [imageLabel, nam](QNetworkReply *reply) {
                if (reply->error() == QNetworkReply::NoError) {
                    QPixmap pixmap;
                    pixmap.loadFromData(reply->readAll());
                    if (!pixmap.isNull()) {
                        imageLabel->setPixmap(pixmap.scaled(400, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    } else {
                        imageLabel->setText("图片加载失败");
                    }
                } else {
                    imageLabel->setText("图片加载失败");
                }
                reply->deleteLater();
                nam->deleteLater();
            });
            nam->get(QNetworkRequest(QUrl(fullUrl)));

            QString tabName = QFileInfo(url).fileName();
            if (tabName.isEmpty()) tabName = QString("图片 %1").arg(tabWidget->count() + 1);
            tabWidget->addTab(imageLabel, tabName);
        }
        evidenceLayout->addWidget(tabWidget);
        mainLayout->addWidget(evidenceGroup);
    }

    // 处理结果组（如果已处理）
    QString handleResult = dispute.value("handle_result").toString();
    if (!handleResult.isEmpty()) {
        QGroupBox *resultGroup = new QGroupBox("处理结果");
        QVBoxLayout *resultLayout = new QVBoxLayout(resultGroup);
        QTextEdit *resultEdit = new QTextEdit();
        resultEdit->setPlainText(handleResult);
        resultEdit->setReadOnly(true);
        resultLayout->addWidget(resultEdit);
        mainLayout->addWidget(resultGroup);
    }

    // 关闭按钮
    QPushButton *closeBtn = new QPushButton("关闭");
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    mainLayout->addWidget(closeBtn, 0, Qt::AlignCenter);

    // 样式
    setStyleSheet(R"(
        QGroupBox {
            font-weight: bold;
            border: 1px solid #ddd;
            border-radius: 6px;
            margin-top: 10px;
            padding-top: 10px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
        }
        QTextEdit {
            border: 1px solid #ddd;
            border-radius: 4px;
            padding: 6px;
            background-color: #fafafa;
        }
    )");
}
