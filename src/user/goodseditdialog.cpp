#include "goodseditdialog.h"
#include "..\apiservice.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QJsonObject>

GoodsEditDialog::GoodsEditDialog(int goodsId, const QJsonObject &goodsData, QWidget *parent)
    : QDialog(parent), m_goodsId(goodsId)
{
    setupUI();
    loadGoodsData(goodsData);
}

void GoodsEditDialog::setupUI()
{
    setWindowTitle("编辑商品");
    setMinimumSize(500, 450);
    setModal(true);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(15);

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
    m_descEdit->setMaximumHeight(150);
    formLayout->addRow("描述:", m_descEdit);

    mainLayout->addLayout(formLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_saveBtn = new QPushButton("保存");
    m_saveBtn->setObjectName("primaryBtn");
    m_cancelBtn = new QPushButton("取消");
    m_cancelBtn->setObjectName("secondaryBtn");
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_cancelBtn);
    buttonLayout->addWidget(m_saveBtn);
    mainLayout->addLayout(buttonLayout);

    connect(m_saveBtn, &QPushButton::clicked, this, &GoodsEditDialog::onSave);
    connect(m_cancelBtn, &QPushButton::clicked, this, &GoodsEditDialog::onCancel);

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
            padding: 8px 20px;
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
    // 根据分类名称设置下拉框索引
    QString category = goodsData.value("category_name").toString();
    int index = m_categoryCombo->findText(category);
    if (index >= 0) m_categoryCombo->setCurrentIndex(index);
    m_descEdit->setText(goodsData.value("description").toString());
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
