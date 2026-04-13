#ifndef PUBLISHPAGE_H
#define PUBLISHPAGE_H

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>

class PublishPage : public QWidget {
    Q_OBJECT

public:
    explicit PublishPage(QWidget *parent = nullptr);

signals:
    void goodsPublished(const QString &name, const QString &category,
                        double price, const QString &description);

private slots:
    void onPublishGoods();
    void onAIPriceEstimate();

private:
    void setupUI();
    void onAddImage();
    void addImagePreview(const QString &filePath);

    QLineEdit *goodsNameEdit;
    QComboBox *goodsCategoryCombo;
    QLineEdit *goodsPriceEdit;
    QTextEdit *goodsDescEdit;

    QList<QString> m_uploadedImagePaths;      // 存储上传后的图片URL（服务端返回）
    QList<QLabel*> m_imagePreviewLabels;      // 预览控件
    QWidget *m_imageContainer;                // 放置预览图片的容器
    QHBoxLayout *m_imageLayout;               // 水平布局
    QPushButton *m_addImageBtn;               // 添加图片按钮
};

#endif // PUBLISHPAGE_H
