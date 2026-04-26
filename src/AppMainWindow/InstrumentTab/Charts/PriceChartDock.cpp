//
// Created by DED on 28.02.2026.
//
#include <QPainter>
#include <QWheelEvent>

#include <time.h>

#include "PriceChartDock.h"
#include "AppTheme.h"
#include "CandleRenderer.h"

#include <InstrumentTab/InstrumentContext.h>
#include <Logger/Logger.h>

PriceChartWidget::PriceChartWidget(QWidget* parent) : QWidget(parent) {
    setMouseTracking(true);
    setAttribute(Qt::WA_OpaquePaintEvent);
}

void PriceChartWidget::setCandles(std::vector<Market::Candle>& candles) {
    _candles = std::move(candles);

    Logger::log<Logger::LogLevel::Debug>("Подгружены свечи {} штук !", _candles.size());

    _candleWidth = 0.0; // сбрасываем чтобы recalcView пересчитал под новые данные
    _viewOffset = 0;
    recalculateView();

    update();
}

void PriceChartWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    drawBackground(painter);
    drawGrid(painter);
    drawCandles(painter);
    drawAxisY(painter);
    drawAxisX(painter);
}

void PriceChartWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    recalculateView();
    update();
}

void PriceChartWidget::wheelEvent(QWheelEvent* event) {
    const int delta = event->angleDelta().y();

    if (event->modifiers() & Qt::ControlModifier) {
        // Ctrl + колесо — зум
        const double factor = (delta > 0) ? 1.15 : 0.87;
        _candleWidth = std::clamp(
            _candleWidth * factor,
            MIN_CANDLE_W,
            MAX_CANDLE_W
        );

        const int maxOffset = std::max(0, static_cast<int>(_candles.size()) - visibleCandleCount());

        _viewOffset = std::clamp(_viewOffset, 0, maxOffset);
    } else {
        // просто колесо — скролл
        const int step = (delta > 0) ? -3 : 3;
        const int maxOffset = std::max(0, static_cast<int>(_candles.size()) - visibleCandleCount());
        _viewOffset = std::clamp(_viewOffset + step, 0, maxOffset);
    }

    recalculateView();
    update();
    event->accept();
}

void PriceChartWidget::drawCandles(QPainter& painter) const {
    if (_candles.empty())
        return;

    auto const rect = chartRect();
    const double bodyWidth = std::max(MAX_CANDLE_W, _candleWidth - CANDLE_GAP);

    const int first = _viewOffset;
    const int last = std::min(
        _viewOffset + visibleCandleCount(),
        static_cast<int>(_candles.size())
    );

    for (int i = first; i < last; ++i) {
        const double x = (i - _viewOffset) * _candleWidth;

        // не рисуем если за пределами chartRect
        if (x + _candleWidth < 0 || x > rect.width())
            continue;

        const auto geom = CandleRenderer::calculateGeometry(
            _candles.at(i),
            x,
            bodyWidth,
            _priceMin,
            _priceMax,
            rect.height()
        );

        CandleRenderer::draw(painter, geom);
    }
}

void PriceChartWidget::drawGrid(QPainter& painter) const {
    if (_priceMin >= _priceMax)
        return;

    const auto rect = chartRect();
    static constexpr int GRID_LINES = 6;

    painter.setPen(QPen(palette().color(QPalette::Mid), 1, Qt::SolidLine));

    for (int i = 0; i <= GRID_LINES; ++i) {
        const double ratio = static_cast<double>(i) / GRID_LINES;
        const double price = _priceMax - (_priceMax - _priceMin) * ratio;
        const int y = static_cast<int>(
            (_priceMax - price) / (_priceMax - _priceMin) * rect.height()
        );

        if (y < 0 || y > rect.height())
            continue;
        painter.drawLine(0, y, rect.width(), y);
    }

    Logger::log<Logger::LogLevel::Debug>("Отрисованна сетка графика!");
}

void PriceChartWidget::drawAxisY(QPainter& painter) const {
    if (_priceMin >= _priceMax)
        return;

    const auto rect = chartRect();
    static constexpr int GRID_LINES = 6;

    for (int i = 0; i <= GRID_LINES; ++i) {
        const double ratio = static_cast<double>(i) / GRID_LINES;
        const double price = _priceMax - (_priceMax - _priceMin) * ratio;
        const int y = static_cast<int>(ratio * rect.height());

        if (y < 0 || y > rect.height())
            continue;

        // засечка — маленький штрих от границы вправо
        painter.setPen(palette().color(QPalette::Mid));
        painter.drawLine(rect.width(), y, rect.width() + 4, y);

        // текст цены
        painter.setPen(palette().color(QPalette::PlaceholderText));
        const QString label = QString::number(price, 'f', 2);
        painter.drawText(
            QRect(rect.width() + 6, y - 10, Y_AXIS_WIDTH - 8, 20),
            Qt::AlignVCenter | Qt::AlignLeft,
            label
        );
    }

    Logger::log<Logger::LogLevel::Debug>("Отрисованна ось Y!");
}


void PriceChartWidget::drawAxisX(QPainter& painter) const {
    if (_candles.empty())
        return;

    const auto rect = chartRect();

    // шаг меток зависит от масштаба — чем мельче свечи тем реже метки
    const int labelStep = std::max(1, static_cast<int>(MAX_CANDLE_W / _candleWidth));

    painter.setFont(AppTheme::monoFont(9));
    painter.setPen(palette().color(QPalette::PlaceholderText));

    const int first = _viewOffset;
    const int last = std::min(
        _viewOffset + visibleCandleCount(),
        static_cast<int>(_candles.size())
    );

    for (int i = first; i < last; i += labelStep) {
        const int x = static_cast<int>((i - _viewOffset) * _candleWidth);

        // не рисуем у самого правого края — метка не влезет
        if (x < 0 || x > rect.width() - 30)
            continue;

        // засечка
        painter.setPen(palette().color(QPalette::Mid));
        painter.drawLine(x, rect.height(), x, rect.height() + 4);

        // дата из startPoint
        const std::time_t tt = std::chrono::system_clock::to_time_t(
            _candles[i].startPoint
        );

        std::tm tm{};
        localtime_s(&tm, &tt);

        const QString label = QString("%1.%2")
                              .arg(tm.tm_mday, 2, 10, QChar('0'))
                              .arg(tm.tm_mon + 1, 2, 10, QChar('0'));

        painter.setPen(palette().color(QPalette::PlaceholderText));
        painter.drawText(
            QRect(x - 20, rect.height() + 4, 42, X_AXIS_HEIGHT - 4),
            Qt::AlignCenter,
            label
        );
    }

    Logger::log<Logger::LogLevel::Debug>("Отрисованна ось X!");
}

QRect PriceChartWidget::chartRect() const noexcept {
    return {0, 0, width() - Y_AXIS_WIDTH, height() - X_AXIS_HEIGHT};
}

void PriceChartWidget::drawBackground(QPainter& painter) const {

    painter.fillRect(rect(), palette().color(QPalette::Base));

    // Y ось
    painter.fillRect(QRect(chartRect().width(), 0, Y_AXIS_WIDTH, height()), palette().color(QPalette::Window));


    // X ось
    painter.fillRect(QRect(0, chartRect().height(), chartRect().width(), X_AXIS_HEIGHT),
                     palette().color(QPalette::Window));

    painter.setPen(palette().color(QPalette::Mid));

    // граница между свечами и осью Y
    painter.drawLine(chartRect().width(), 0, chartRect().width(), height());

    // граница между свечами и осью X
    painter.drawLine(0, chartRect().height(), chartRect().width(), chartRect().height());

    Logger::log<Logger::LogLevel::Debug>("Отрисован задний план!");
}

void PriceChartWidget::recalculateView() {
    if (_candles.empty())
        return;

    // при первой загрузке масштабируем под ширину виджета
    if (_candleWidth <= 0.0) {
        _candleWidth = std::clamp(static_cast<double>(chartRect().width()) / _candles.size(), MIN_CANDLE_W,
                                  MAX_CANDLE_W);

        // стартуем с конца — показываем последние свечи
        _viewOffset = std::max(0,
                               static_cast<int>(_candles.size()) - visibleCandleCount()
        );
    }

    const int first = _viewOffset;
    const int last = std::min(
        _viewOffset + visibleCandleCount(),
        static_cast<int>(_candles.size())
    );

    _priceMin = std::numeric_limits<double>::max();
    _priceMax = std::numeric_limits<double>::lowest();

    for (int i = first; i < last; ++i) {
        _priceMin = std::min(_priceMin, _candles[i].lowPrice);
        _priceMax = std::max(_priceMax, _candles[i].highPrice);
    }

    // 5% отступ чтобы свечи не упирались в края
    const double padding = (_priceMax - _priceMin) * 0.05;
    _priceMin -= padding;
    _priceMax += padding;

    Logger::log<Logger::LogLevel::Debug>("Пересчет отображения!");
}

int PriceChartWidget::visibleCandleCount() const noexcept {
    if (_candleWidth <= 0)
        return 0;

    return static_cast<int>(std::ceil(chartRect().width() / _candleWidth)) + 1;
}

PriceChartDock::PriceChartDock(InstrumentContext* context, InstrumentSession* session, QWidget* parent) : QDockWidget(
    context->tickerName(), parent) {
    setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Expanding
    );
    setObjectName(QString("PriceChartDock_%1").arg(context->tickerName()));
    setAllowedAreas(Qt::AllDockWidgetAreas);

    _priceChartWidget = new PriceChartWidget(this);
    setWidget(_priceChartWidget);
}

PriceChartWidget* PriceChartDock::widget() {
    return _priceChartWidget;
}
