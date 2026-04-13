#ifndef GOODSEDITDIALOG_H
#define GOODSEDITDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QPushButton>

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

private:
    void setupUI();
    void loadGoodsData(const QJsonObject &goodsData);

    int m_goodsId;
    QLineEdit *m_nameEdit;
    QLineEdit *m_priceEdit;
    QComboBox *m_categoryCombo;
    QTextEdit *m_descEdit;
    QPushButton *m_saveBtn;
    QPushButton *m_cancelBtn;
};

#endif // GOODSEDITDIALOG_H
