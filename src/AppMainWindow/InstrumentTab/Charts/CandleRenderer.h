//
// Created by DED on 21.03.2026.
//

#ifndef COOLAPPNAME_CANDLERENDERER_H
#define COOLAPPNAME_CANDLERENDERER_H

#include <QPainter>

#include <Market.h>

class CandleRenderer {

public:
    CandleRenderer() = delete;

    struct CandleGeometry {
        int wickX;      // X центра фитиля
        int wickTop;    // Y верха фитиля
        int wickBottom; // Y низа фитиля

        int bodyX;      // X левого края тела
        int bodyTop;    // Y верха тела
        unsigned int bodyWidth;  // ширина тела
        unsigned int bodyHeight; // высота тела (минимум 1)

        bool isBullish; // бычья или медвежья
    };

    static void draw(QPainter& painter, const CandleGeometry& geometry);

    // вычисляет геометрию из данных свечи и координат
    [[nodiscard]] static CandleGeometry calculateGeometry(
        const Market::Candle& candle,
        double x, // левый край свечи в пикселях
        double width, // ширина свечи в пикселях
        double priceMin,
        double priceMax,
        int chartHeight
    );
};


#endif //COOLAPPNAME_CANDLERENDERER_H
