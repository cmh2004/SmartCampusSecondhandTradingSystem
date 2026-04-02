#ifndef PUBLISHPAGE_H
#define PUBLISHPAGE_H

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QPushButton>
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
    void onUploadImage();
    void onAIPriceEstimate();

private:
    void setupUI();

    QLineEdit *goodsNameEdit;
    QComboBox *goodsCategoryCombo;
    QLineEdit *goodsPriceEdit;
    QTextEdit *goodsDescEdit;
    QPushButton *uploadImageBtn;
    QLabel *imagePreview;
};

#endif // PUBLISHPAGE_H
