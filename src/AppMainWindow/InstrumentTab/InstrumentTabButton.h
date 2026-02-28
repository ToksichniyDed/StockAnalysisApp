//
// Created by DED on 27.02.2026.
//

#ifndef COOLAPPNAME_INSTRUMENTTABBUTTON_H
#define COOLAPPNAME_INSTRUMENTTABBUTTON_H

#include <QWidget>

#include "InstrumentContext.h"


class InstrumentTabButton : public QWidget{

    Q_OBJECT

public:
    explicit InstrumentTabButton(InstrumentContext* instrumentContext, QWidget* parent);
    ~InstrumentTabButton() override;

    void setActive(bool isActive);
    [[nodiscard]] bool isActive() const noexcept;
    void setMarkerColor(const QColor& color) noexcept;

signals:
    void clicked();
    void closeRequested();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    [[nodiscard]] QRect closeButtonRect() const noexcept;
    void recalcWidth() noexcept;

private:
    InstrumentContext* _instrumentContext = nullptr;
    bool _active  = false;
    bool _hovered = false;
    QColor _markerColor{ "#00d4ff" };
};


#endif //COOLAPPNAME_INSTRUMENTTABBUTTON_H