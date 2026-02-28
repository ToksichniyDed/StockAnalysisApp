//
// Created by DED on 27.02.2026.
//

#ifndef COOLAPPNAME_INSTRUMENTTABBAR_H
#define COOLAPPNAME_INSTRUMENTTABBAR_H

#include <QWidget>
#include <QLineEdit>
#include <QHBoxLayout>

#include <InstrumentContext.h>
#include <InstrumentTabButton.h>

class InstrumentTabBar : public QWidget {

    Q_OBJECT

public:
    explicit InstrumentTabBar(QWidget* parent = nullptr);
    ~InstrumentTabBar() override;

    void addInstrumentTab(InstrumentContext* instrumentContext);
    void removeInstrumentTab(int index);
    void setActiveInstrumentTab(int index);

protected:
    bool eventFilter(QObject* sender, QEvent* event) override;

signals:
    void signal_InstrumentTabClicked(int index);
    void signal_InstrumentTabCloseRequested(int index);
    void signal_AddInstrumentTabRequested(const QString& ticker);

private slots:
    void slot_AddInstrumentButtonClicked();
    void slot_InstrumentTickerEntered();
    void slot_TickerEditingCancelled();

private:
    QHBoxLayout* _tabLayout = nullptr;
    QList<InstrumentTabButton*> _instrumentTabs;
    QToolButton* _addInstrumentTabButton = nullptr;
    QLineEdit* _tickerEdit = nullptr;
};


#endif //COOLAPPNAME_INSTRUMENTTABBAR_H
