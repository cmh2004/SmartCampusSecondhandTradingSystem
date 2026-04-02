#ifndef REPORTSUBMITDIALOG_H
#define REPORTSUBMITDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QComboBox>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

class ReportSubmitDialog : public QDialog {
    Q_OBJECT

public:
    explicit ReportSubmitDialog(QWidget *parent = nullptr, int targetId = -1,
                                QString targetType = "goods", QString targetName = "");

signals:
    void reportSubmitted(int targetId, QString targetType);

private slots:
    void onSubmitReport();
    void onUploadEvidence();
    void onReportTypeChanged(int index);
    void onCancel();

private:
    void setupUI();
    void loadTargetInfo();
    int mapReasonType(const QString &reasonText) const;

private:
    int targetId;
    QString targetType;
    QString targetName;

    // 被举报对象信息
    QLabel *targetTitleLabel;
    QLabel *targetInfoLabel;

    // 举报表单
    QComboBox *reportTypeCombo;
    QTextEdit *descriptionEdit;
    QListWidget *evidenceList;

    // 联系方式
    QLineEdit *contactEdit;

    // 按钮
    QPushButton *uploadBtn;
    QPushButton *submitBtn;
    QPushButton *cancelBtn;
};

#endif // REPORTSUBMITDIALOG_H
