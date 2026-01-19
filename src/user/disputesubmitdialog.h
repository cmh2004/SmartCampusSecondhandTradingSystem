#ifndef DISPUTESUBMITDIALOG_H
#define DISPUTESUBMITDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QComboBox>
#include <QTextEdit>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>

class DisputeSubmitDialog : public QDialog {
    Q_OBJECT

public:
    explicit DisputeSubmitDialog(QWidget *parent = nullptr, int orderId = -1);

private slots:
    void onUploadEvidence();
    void onSubmitDispute();
    void onEvidenceItemSelected();
    void onDescriptionTextChanged();

private:
    void setupUI();
    void loadOrderInfo(int orderId);
    void updateSubmitButtonState();

private:
    int orderId;

    // 订单信息
    QLabel *orderIdLabel;
    QLabel *goodsNameLabel;
    QLabel *orderAmountLabel;
    QLabel *sellerNameLabel;

    // 纠纷表单
    QComboBox *disputeTypeCombo;
    QTextEdit *descriptionEdit;
    QListWidget *evidenceList;
    QPushButton *uploadBtn;
    QPushButton *removeBtn;
    QPushButton *submitBtn;
};

#endif // DISPUTESUBMITDIALOG_H
