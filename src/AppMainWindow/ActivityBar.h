//
// Created by DED on 14.03.2026.
//

#ifndef COOLAPPNAME_ACTIVITYBAR_H
#define COOLAPPNAME_ACTIVITYBAR_H

#include <QToolButton>
#include <QVBoxLayout>

#include <InstrumentTab/InstrumentDockSet.h>

class ActivityBar : public QWidget {

    Q_OBJECT

public:
    explicit ActivityBar(QWidget* parent = nullptr);
    ~ActivityBar() override;

    void setActiveInstrumentDockSet(InstrumentDockSet* instrumentDockSet);

signals :
    void signal_ResetDocksLayout();

private:
    QToolButton* createToolButton(const QString& icon, const QString& tooltip);
    void synchronizeDocksVisibilityWithButtons() const;

private:
    QVBoxLayout* _layout = nullptr;
    InstrumentDockSet* _activeInstrumentDockSet = nullptr;

    QToolButton* _priceDockBtn = nullptr;
    QToolButton* _cvdDockBtn = nullptr;
    QToolButton* _vdDockBtn = nullptr;
    QToolButton* _resetBtn = nullptr;

    static constexpr int BUTTON_SIZE = 32;
    static constexpr int BAR_WIDTH = 40;
};


#endif //COOLAPPNAME_ACTIVITYBAR_H
