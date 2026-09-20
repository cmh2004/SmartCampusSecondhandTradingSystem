#include "GoodsEditDialog.h"
#include "../apiservice.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QJsonObject>
#include <QFileDialog>
#include <QFileInfo>
#include <QScrollArea>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPixmap>

GoodsEditDialog::GoodsEditDialog(int goodsId, const QJsonObject &goodsData, QWidget *parent)
    : QDialog(parent), m_goodsId(goodsId)
{
    setupUI();
    loadGoodsData(goodsData);
}

void GoodsEditDialog::setupUI()
{
    setWindowTitle("编辑商品");
    setMinimumSize(650, 550);
    setModal(true);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);

    // 基本表单
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(12);
    m_nameEdit = new QLineEdit();
    m_nameEdit->setPlaceholderText("商品名称");
    formLayout->addRow("商品名称:", m_nameEdit);

    m_priceEdit = new QLineEdit();
    m_priceEdit->setPlaceholderText("价格");
    formLayout->addRow("价格:", m_priceEdit);

    m_categoryCombo = new QComboBox();
    m_categoryCombo->addItems({"书籍教材", "电子产品", "服饰鞋包", "生活用品",
                               "体育器材", "学习工具", "美妆个护", "其他"});
    formLayout->addRow("分类:", m_categoryCombo);

    m_descEdit = new QTextEdit();
    m_descEdit->setPlaceholderText("商品描述...");
    m_descEdit->setMaximumHeight(120);
    formLayout->addRow("描述:", m_descEdit);

    mainLayout->addLayout(formLayout);

    // 图片管理区域
    QWidget *imageWidget = new QWidget();
    QVBoxLayout *imageWidgetLayout = new QVBoxLayout(imageWidget);
    imageWidgetLayout->setSpacing(8);

    QLabel *imageTitle = new QLabel("商品图片");
    imageTitle->setStyleSheet("font-weight: bold;");
    imageWidgetLayout->addWidget(imageTitle);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setFixedHeight(140);
    scrollArea->setStyleSheet("border: none;");

    m_imageContainer = new QWidget();
    m_imageLayout = new QHBoxLayout(m_imageContainer);
    m_imageLayout->setContentsMargins(0, 0, 0, 0);
    m_imageLayout->setSpacing(10);
    m_imageLayout->setAlignment(Qt::AlignLeft);

    scrollArea->setWidget(m_imageContainer);
    imageWidgetLayout->addWidget(scrollArea);

    // 添加图片按钮
    m_addImageBtn = new QPushButton("+ 添加图片");
    m_addImageBtn->setFixedHeight(32);
    m_addImageBtn->setObjectName("primaryBtn");
    imageWidgetLayout->addWidget(m_addImageBtn);

    mainLayout->addWidget(imageWidget);

    // 按钮区域
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_saveBtn = new QPushButton("保存");
    m_cancelBtn = new QPushButton("取消");
    m_saveBtn->setObjectName("primaryBtn");
    m_cancelBtn->setObjectName("secondaryBtn");
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_cancelBtn);
    buttonLayout->addWidget(m_saveBtn);
    mainLayout->addLayout(buttonLayout);

    connect(m_saveBtn, &QPushButton::clicked, this, &GoodsEditDialog::onSave);
    connect(m_cancelBtn, &QPushButton::clicked, this, &GoodsEditDialog::onCancel);
    connect(m_addImageBtn, &QPushButton::clicked, this, &GoodsEditDialog::onAddImage);

    setStyleSheet(R"(
        QDialog { background-color: white; }
        QFormLayout QLabel { font-weight: bold; color: #2c3e50; }
        QLineEdit, QTextEdit, QComboBox {
            border: 1px solid #ddd;
            border-radius: 4px;
            padding: 6px;
        }
        #primaryBtn {
            background-color: #3498db;
            color: white;
            border-radius: 4px;
            padding: 6px 20px;
        }
        #secondaryBtn {
            background-color: #ecf0f1;
            color: #34495e;
            border-radius: 4px;
            padding: 6px 18px;
        }
    )");
}

void GoodsEditDialog::loadGoodsData(const QJsonObject &goodsData)
{
    m_nameEdit->setText(goodsData.value("name").toString());
    m_priceEdit->setText(QString::number(goodsData.value("price").toDouble()));
    QString category = goodsData.value("category_name").toString();
    int index = m_categoryCombo->findText(category);
    if (index >= 0) m_categoryCombo->setCurrentIndex(index);
    m_descEdit->setText(goodsData.value("description").toString());

    // 加载已有图片
    QJsonArray images = goodsData.value("images").toArray();
    qDebug() << "Images array:" << images;
    for (const QJsonValue &val : images) {
        qDebug() << "Image value:" << val;
        QJsonObject imgObj = val.toObject();
        QString imageUrl = imgObj.value("image_url").toString();
        if (!imageUrl.isEmpty()) {
            addImagePreview(imageUrl, true);
            m_existingImageUrls.append(imageUrl);  // 存储相对路径
        }
    }
}

void GoodsEditDialog::addImagePreview(const QString &imageUrl, bool isExisting)
{
    QString fullUrl = imageUrl;
    if (!fullUrl.startsWith("http")) {
        fullUrl = "http://127.0.0.1:8080" + imageUrl;
    }

    // 创建容器
    QWidget *container = new QWidget();
    container->setFixedSize(100, 100);
    container->setStyleSheet("border: 1px solid #ddd; border-radius: 6px; background-color: white;");

    // 图片标签
    QLabel *imageLabel = new QLabel(container);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setGeometry(0, 0, 100, 100);
    imageLabel->setStyleSheet("border: none;");

    // 删除按钮
    QPushButton *removeBtn = new QPushButton("×", container);
    removeBtn->setFixedSize(24, 24);
    removeBtn->setStyleSheet("background-color: rgba(231, 76, 60, 0.9); color: white; border-radius: 12px; border: none;");
    removeBtn->move(76, 0);
    removeBtn->setCursor(Qt::PointingHandCursor);
    removeBtn->setProperty("imageUrl", imageUrl);
    removeBtn->setProperty("isExisting", isExisting);
    connect(removeBtn, &QPushButton::clicked, this, &GoodsEditDialog::onRemoveImage);

    // 异步加载图片
    QNetworkAccessManager *nam = new QNetworkAccessManager(this);
    connect(nam, &QNetworkAccessManager::finished, [imageLabel, nam](QNetworkReply *reply) {
        if (reply->error() == QNetworkReply::NoError) {
            QPixmap pixmap;
            pixmap.loadFromData(reply->readAll());
            if (!pixmap.isNull()) {
                imageLabel->setPixmap(pixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            } else {
                qDebug() << "Failed to decode image from data";
            }
        } else {
            qDebug() << "Image load error:" << reply->errorString();
            qDebug() << "URL:" << reply->url().toString();
        }
        reply->deleteLater();
        nam->deleteLater();
    });
    nam->get(QNetworkRequest(QUrl(fullUrl)));

    // 存储图片信息
    container->setProperty("imageUrl", imageUrl);
    container->setProperty("isExisting", isExisting);

    m_imageLayout->insertWidget(m_imageLayout->count(), container);
    m_imageWidgets.append(container);
}

void GoodsEditDialog::onAddImage()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(
        this, "选择商品图片", "", "Images (*.png *.jpg *.jpeg)");
    if (fileNames.isEmpty()) return;

    // 限制最多5张图片
    int totalImages = m_existingImageUrls.size() + m_newUploadedImageUrls.size() + fileNames.size();
    if (totalImages > 5) {
        QMessageBox::warning(this, "提示", "最多只能添加5张图片");
        return;
    }

    // 先上传图片
    for (const QString &fileName : fileNames) {
        QJsonObject uploadResult = ApiService::instance()->uploadImage(fileName);
        if (uploadResult.value("success").toBool()) {
            QString uploadedUrl = uploadResult.value("data").toObject().value("file_url").toString();
            if (!uploadedUrl.isEmpty()) {
                m_newUploadedImageUrls.append(uploadedUrl);
                QString fullUrl = "http://127.0.0.1:8080" + uploadedUrl;
                addImagePreview(fullUrl, false);
            }
        } else {
            QMessageBox::warning(this, "图片上传失败", uploadResult.value("error").toString());
        }
    }
}

void GoodsEditDialog::onRemoveImage()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    QWidget *container = qobject_cast<QWidget*>(btn->parent());
    if (!container) return;

    QString imageUrl = btn->property("imageUrl").toString();
    bool isExisting = btn->property("isExisting").toBool();

    if (isExisting) {
        // 从已有列表中移除，并记录到删除列表
        m_existingImageUrls.removeOne(imageUrl);
        m_removedImageUrls.append(imageUrl);
    } else {
        // 新上传的图片，从新上传列表中移除
        m_newUploadedImageUrls.removeOne(imageUrl);
    }

    m_imageLayout->removeWidget(container);
    m_imageWidgets.removeOne(container);
    container->deleteLater();
}

void GoodsEditDialog::collectImageUrlsForSave(QStringList &existingUrls, QStringList &newUploadedUrls)
{
    // 保留未被删除的已有图片
    existingUrls = m_existingImageUrls;
    newUploadedUrls = m_newUploadedImageUrls;
}

void GoodsEditDialog::onSave()
{
    QString name = m_nameEdit->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "提示", "商品名称不能为空");
        return;
    }
    bool ok;
    double price = m_priceEdit->text().trimmed().toDouble(&ok);
    if (!ok || price <= 0) {
        QMessageBox::warning(this, "提示", "请输入有效的价格");
        return;
    }

    QString category = m_categoryCombo->currentText();
    QString description = m_descEdit->toPlainText().trimmed();

    QJsonObject updates;
    updates["name"] = name;
    updates["price"] = price;
    updates["category"] = category;
    updates["description"] = description;

    // 组合最终图片URL列表（已有保留的 + 新上传的）
    QStringList finalImageUrls = m_existingImageUrls;
    finalImageUrls.append(m_newUploadedImageUrls);
    QJsonArray imagesArray;
    for (const QString &url : finalImageUrls) {
        imagesArray.append(url);
    }
    updates["images"] = imagesArray;

    QJsonObject result = ApiService::instance()->updateGoods(m_goodsId, updates);
    if (result.value("success").toBool()) {
        QMessageBox::information(this, "成功", "商品信息已更新");
        emit goodsUpdated();
        accept();
    } else {
        QMessageBox::warning(this, "失败", result.value("error").toString());
    }
}

void GoodsEditDialog::onCancel()
{
    reject();
}
