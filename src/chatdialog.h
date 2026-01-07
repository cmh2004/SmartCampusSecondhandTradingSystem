// ChatDialog.h
#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QTextEdit>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QJsonObject>
#include <QLabel>

class ChatDialog : public QDialog {
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr, int goodsId = -1, QString sellerId = "");

public slots:
    void analyzeChatForRisk();
    void sendMessage();
    void makePriceOffer();
    void onAIChatAssistant();

private slots:
    void onRiskDetected(const QString &riskType, const QString &suggestion);

private:
    void setupUI();
    void loadChatHistory();
    void saveChatMessage(const QString &sender, const QString &message, bool isPriceOffer = false);
    void addRiskWarning(const QString &warning);

signals:
    void reportUserRequested(QString userId);

private:
    int goodsId;
    QString sellerId;

    // 聊天组件
    QListWidget *chatList;
    QTextEdit *messageEdit;
    QLineEdit *priceOfferEdit;
    QPushButton *sendBtn;
    QPushButton *priceOfferBtn;
    QPushButton *aiAssistantBtn;
    QPushButton *riskAnalysisBtn;

    // 风险提醒区域
    QWidget *riskWidget;
    QLabel *riskLabel;

    // 定时器用于实时风险分析
    QTimer *riskAnalysisTimer;
};

#endif // CHATDIALOG_H
