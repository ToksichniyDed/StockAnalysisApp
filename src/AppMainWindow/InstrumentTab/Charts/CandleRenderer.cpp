//
// Created by DED on 21.03.2026.
//

#include "CandleRenderer.h"

#include <Logger/Logger.h>

#include <AppTheme.h>

void CandleRenderer::draw(QPainter& painter, const CandleGeometry& geometry) {
    const QColor color = geometry.isBullish
                         ? QColor(AppTheme::COLOR_BUY)
                         : QColor(AppTheme::COLOR_SELL);

    // фитиль
    painter.setPen(QPen(color,1));
    painter.drawLine(geometry.wickX, geometry.wickTop, geometry.wickX, geometry.wickBottom);

    // тело
    painter.fillRect(QRect(
        geometry.bodyX,
        geometry.bodyTop,
        static_cast<int>(geometry.bodyWidth),
        static_cast<int>(geometry.bodyHeight)
    ), color);

    Logger::log<Logger::LogLevel::Debug>("Рендеринг свечи!");
}

CandleRenderer::CandleGeometry CandleRenderer::calculateGeometry(const Market::Candle& candle, double x, double width,
    double priceMin, double priceMax, int chartHeight) {

    const double range = priceMax - priceMin;

    auto priceToY = [&](double price) {
        if (range <= 0.0)
            return chartHeight / 2;

        return static_cast<int>((priceMax - price) / range * chartHeight);
    };

    const int wickTop = priceToY(candle.highPrice); // вверх свечи
    const int wickBottom = priceToY(candle.lowPrice); // низ свечи
    const int bodyTop = priceToY(std::max(candle.openPrice, candle.closePrice)); // верх тела
    const int bodyBottom = priceToY(std::min(candle.openPrice, candle.closePrice));// низ тела

    Logger::log<Logger::LogLevel::Debug>("Расчет геометрии свечи!");

    return {
        .wickX = static_cast<int>(x + width / 2.0),
        .wickTop = wickTop,
        .wickBottom = wickBottom,
        .bodyX = static_cast<int>(x),
        .bodyTop = bodyTop,
        .bodyWidth = static_cast<unsigned int>(std::max(1.0, width)),
        .bodyHeight = static_cast<unsigned int>(std::max(1, bodyBottom - bodyTop)),
        .isBullish = candle.isBullish()
    };
}
