//
// Created by DED on 28.02.2026.
//

#ifndef COOLAPPNAME_INSTRUMENTDOCKSET_H
#define COOLAPPNAME_INSTRUMENTDOCKSET_H

#include <QObject>
#include <QMainWindow>

#include "Charts/CvdChartDock.h"
#include "Charts/VdChartDock.h"
#include "Charts/PriceChartDock.h"
#include "InstrumentSession.h"
#include "InstrumentContext.h"

class InstrumentDockSet : public QObject {

    Q_OBJECT

public:
    explicit InstrumentDockSet(InstrumentContext* context,
                               const std::shared_ptr<Exchange::IDataFetcher>& fetcher,
                               QMainWindow* mainWindow,
                               QObject* parent = nullptr);

    ~InstrumentDockSet() override;

    void show();
    void hide();

    [[nodiscard]] InstrumentContext* context() const noexcept;
    [[nodiscard]] InstrumentSession* session() const noexcept;

    [[nodiscard]] bool priceChartDockVisible() const noexcept;
    [[nodiscard]] bool cvdChartDockVisible() const noexcept;
    [[nodiscard]] bool vdChartDockVisible() const noexcept;

signals:
    void signal_PriceChartDockChangedVisible(bool);
    void signal_CvdChartDockChangedVisible(bool);
    void signal_VdChartDockChangedVisible(bool);

public slots:
    void slot_PriceChartDockVisible(bool visible);
    void slot_CvdChartDockVisible(bool visible);
    void slot_VdChartDockVisible(bool visible);
    void slot_ResetDocksLayout();

    void slot_PriceChartSetCandles() const;
    void slot_CvdChartSetCandles();
    void slot_VdChartSetCandles();

private:
    void setupDocks();
    void applyDefaultLayout();

private:
    QMainWindow* _mainWindow = nullptr;
    InstrumentContext* _context = nullptr;
    InstrumentSession* _session = nullptr;

    PriceChartDock* _priceDock = nullptr;
    CvdChartDock* _cvdDock = nullptr;
    VdChartDock* _vdDock = nullptr;

    QByteArray _savedState;
    bool _firstShow = true;
};


#endif //COOLAPPNAME_INSTRUMENTDOCKSET_H
