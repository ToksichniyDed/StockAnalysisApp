//
// Created by DED on 28.02.2026.
//

#ifndef COOLAPPNAME_CVDCHARTDOCK_H
#define COOLAPPNAME_CVDCHARTDOCK_H

#include <QDockWidget>

#include <Market/Market.h>

class InstrumentContext;
class InstrumentSession;

class CvdChartWidget : public QWidget {

    Q_OBJECT

public:
    explicit CvdChartWidget(QWidget* parent = nullptr);
    ~CvdChartWidget() override = default;

    void setCandles(const std::vector<Market::Candle>& candles);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

protected:
    void recalculateCvd();
    void recalculateView();

    void drawBackground(QPainter& painter) const;
    void drawZeroLine(QPainter& painter) const;
    void drawGradient(QPainter& painter) const;
    void drawCvdLine(QPainter& painter) const;
    void drawAxisY(QPainter& painter) const;

    [[nodiscard]] QRect chartRect() const noexcept;
    [[nodiscard]] double cvdToY(double cvd) const noexcept;
    [[nodiscard]] double candleX(int index) const noexcept;
    [[nodiscard]] int visibleCandleCount() const noexcept;

private:
    std::vector<Market::Candle> _candles;
    std::vector<double> _cvdValues;

    double _cvdMin = 0.0;
    double _cvdMax = 0.0;
    double _candleWidth = 0.0;
    int _viewOffset = 0;

    static constexpr int Y_AXIS_WIDTH = 60;
    static constexpr int X_AXIS_HEIGHT = 0;
    static constexpr double MIN_CANDLE_W = 3.0;
    static constexpr double MAX_CANDLE_W = 60.0;
};

class CvdChartDock : public QDockWidget {

    Q_OBJECT

public:
    explicit CvdChartDock(InstrumentContext* context,
                          InstrumentSession* session,
                          QWidget* parent = nullptr);
    ~CvdChartDock() override = default;

    CvdChartWidget* widget();

protected:
    CvdChartWidget* _cvdChartWidget = nullptr;

};


#endif //COOLAPPNAME_CVDCHARTDOCK_H
