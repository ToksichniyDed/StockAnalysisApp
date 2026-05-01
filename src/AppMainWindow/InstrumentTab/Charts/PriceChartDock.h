//
// Created by DED on 28.02.2026.
//

#ifndef COOLAPPNAME_PRICECHARTDOCK_H
#define COOLAPPNAME_PRICECHARTDOCK_H

#include <QDockWidget>
#include <Market/Market.h>

class InstrumentContext;
class InstrumentSession;

class PriceChartWidget : public QWidget {

    Q_OBJECT

public:
    explicit PriceChartWidget(QWidget* parent = nullptr);
    ~PriceChartWidget() override = default;

    void setCandles(const std::vector<Market::Candle>& candles);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    [[nodiscard]] QRect chartRect() const noexcept;

    void drawBackground(QPainter& painter) const;
    void drawCandles(QPainter& painter) const;
    void drawGrid(QPainter& painter) const;
    void drawAxisY(QPainter& painter) const;
    void drawAxisX(QPainter& painter) const;

    void recalculateView();
    [[nodiscard]] int visibleCandleCount() const noexcept;

private:
    std::vector<Market::Candle> _candles;

    double _candleWidth = 0.0; //ширина одной свечи исходя из ширины chartRect и количества свечей
    double _priceMin = 0.0; // только для видимых свечей
    double _priceMax = 0.0; // только для видимых свечей
    int _viewOffset = 0; // индекс первой видимой свечи

    static constexpr int Y_AXIS_WIDTH  = 60;
    static constexpr int X_AXIS_HEIGHT = 24;

    static constexpr int CANDLE_GAP = 2; // зазор между свечами
    static constexpr double MIN_CANDLE_W = 3.0; // минимальная ширина свечи
    static constexpr double MAX_CANDLE_W = 60.0; // максимальная ширина свечи
};

class PriceChartDock : public QDockWidget {

    Q_OBJECT

public:
    explicit PriceChartDock(InstrumentContext* context,
                            InstrumentSession* session,
                            QWidget* parent = nullptr);

    ~PriceChartDock() override = default;

    PriceChartWidget* widget();

private:
    PriceChartWidget* _priceChartWidget = nullptr;
};


#endif //COOLAPPNAME_PRICECHARTDOCK_H
