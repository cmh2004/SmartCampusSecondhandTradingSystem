// disputesubmitdialog.h 修复版
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
    void onDisputeTypeChanged(int index);

private:
    void setupUI();
    void loadOrderInfo(int orderId);

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

    // AI纠纷分析
    QTextEdit *aiAnalysisText;
    QPushButton *analyzeBtn;
};

#endif // DISPUTESUBMITDIALOG_H
