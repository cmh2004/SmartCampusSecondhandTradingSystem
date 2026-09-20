#ifndef DISPUTEDETAILDIALOG_H
#define DISPUTEDETAILDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QJsonObject>

class DisputeDetailDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DisputeDetailDialog(const QJsonObject &dispute, QWidget *parent = nullptr);

private:
    void setupUI(const QJsonObject &dispute);
    void loadImages(const QString &evidenceUrls);

    QTabWidget *m_tabWidget;
    QLabel *m_typeLabel;
    QLabel *m_statusLabel;
    QLabel *m_timeLabel;
    QTextEdit *m_descEdit;
    QTextEdit *m_resultEdit;
};

#endif // DISPUTEDETAILDIALOG_H
