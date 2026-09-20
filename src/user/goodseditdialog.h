#ifndef GOODSEDITDIALOG_H
#define GOODSEDITDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QScrollArea>
#include <QJsonArray>

class GoodsEditDialog : public QDialog
{
    Q_OBJECT
public:
    explicit GoodsEditDialog(int goodsId, const QJsonObject &goodsData, QWidget *parent = nullptr);

signals:
    void goodsUpdated();

private slots:
    void onSave();
    void onCancel();
    void onAddImage();
    void onRemoveImage();

private:
    void setupUI();
    void loadGoodsData(const QJsonObject &goodsData);
    void addImagePreview(const QString &imageUrl, bool isExisting = true);
    void collectImageUrlsForSave(QStringList &existingUrls, QStringList &newUploadedUrls);

    int m_goodsId;
    QLineEdit *m_nameEdit;
    QLineEdit *m_priceEdit;
    QComboBox *m_categoryCombo;
    QTextEdit *m_descEdit;
    QPushButton *m_saveBtn;
    QPushButton *m_cancelBtn;
    QPushButton *m_addImageBtn;

    QWidget *m_imageContainer;
    QHBoxLayout *m_imageLayout;
    QList<QWidget*> m_imageWidgets;          // 存储图片控件（用于删除）
    QStringList m_existingImageUrls;         // 原始图片URL（从服务器获取）
    QStringList m_newUploadedImageUrls;      // 本次新上传的图片URL
    QStringList m_removedImageUrls;          // 本次删除的图片URL
};

#endif // GOODSEDITDIALOG_H
