const express = require('express');
const app = express();
const PORT = 8080;

app.use((req, res, next) => {
    console.log(`[${new Date().toISOString()}] ${req.method} ${req.url}`);
    next();
});

// Успешные ответы

app.get('/success', (req, res) => {
    res.status(200).json({
        status: 'ok',
        message: 'Success response',
        timestamp: new Date().toISOString()
    });
});

app.get('/text', (req, res) => {
    res.status(200).send('Plain text response');
});

app.get('/empty', (req, res) => {
    res.status(200).send('');
});

app.get('/large', (req, res) => {
    const largeData = 'x'.repeat(1024 * 100); // 100KB
    res.status(200).send(largeData);
});

// HTTP ошибки

app.get('/notfound', (req, res) => {
    res.status(404).json({
        error: 'Not Found',
        message: 'The requested resource was not found'
    });
});

app.get('/badrequest', (req, res) => {
    res.status(400).json({
        error: 'Bad Request',
        message: 'Invalid request parameters'
    });
});

app.get('/unauthorized', (req, res) => {
    res.status(401).json({
        error: 'Unauthorized',
        message: 'Authentication required'
    });
});

app.get('/forbidden', (req, res) => {
    res.status(403).json({
        error: 'Forbidden',
        message: 'Access denied'
    });
});

app.get('/servererror', (req, res) => {
    res.status(500).json({
        error: 'Internal Server Error',
        message: 'Something went wrong on the server'
    });
});

app.get('/badgateway', (req, res) => {
    res.status(502).json({
        error: 'Bad Gateway',
        message: 'Invalid response from upstream server'
    });
});

app.get('/unavailable', (req, res) => {
    res.status(503).json({
        error: 'Service Unavailable',
        message: 'Service is temporarily unavailable'
    });
});

// Специальные сценарии

app.get('/timeout', (req, res) => {
    // Никогда не отвечаем - для теста timeout
    console.log('Timeout endpoint called - will not respond');
});

app.get('/slow', (req, res) => {
    // Медленный ответ (5 секунд)
    setTimeout(() => {
        res.status(200).json({
            status: 'ok',
            message: 'Slow response after 5 seconds'
        });
    }, 5000);
});

app.get('/redirect', (req, res) => {
    res.redirect(301, '/success');
});

app.get('/redirect-loop', (req, res) => {
    res.redirect(302, '/redirect-loop');
});

// Проверка заголовков

app.get('/headers', (req, res) => {
    res.status(200).json({
        receivedHeaders: req.headers
    });
});

app.get('/custom-headers', (req, res) => {
    res.set('X-Custom-Header', 'CustomValue');
    res.set('X-Server-Version', '1.0.0');
    res.status(200).json({
        status: 'ok',
        message: 'Response with custom headers'
    });
});

// Query параметры

app.get('/echo-query', (req, res) => {
    res.status(200).json({
        query: req.query
    });
});

// Имитация MOEX API

app.get('/moex/candles', (req, res) => {
    const {ticker, from, till, interval} = req.query;

    if (!ticker) {
        return res.status(400).json({
            error: 'Bad Request',
            message: 'Missing ticker parameter'
        });
    }

    // Симулируем ответ MOEX
    res.status(200).json({
        candles: {
            columns: ['open', 'close', 'high', 'low', 'value', 'volume', 'begin', 'end'],
            data: []
        },
        data: [
            [100.5, 101.0, 102.0, 100.0, 500000.0, 5000.0, '2024-01-01 10:00:00', '2024-01-01 18:50:00'],
            [101.0, 100.5, 101.5, 100.0, 450000.0, 4500.0, '2024-01-02 10:00:00', '2024-01-02 18:50:00']
        ]
    });
});

// Health check

app.get('/health', (req, res) => {
    res.status(200).json({
        status: 'healthy',
        uptime: process.uptime(),
        timestamp: new Date().toISOString()
    });
});

// Запуск сервера

const server = app.listen(PORT, '0.0.0.0', () => {
    console.log(`Mock HTTP server listening on port ${PORT}`);
    console.log('Available endpoints:');
    console.log('  GET /success - 200 OK with JSON');
    console.log('  GET /text - 200 OK with plain text');
    console.log('  GET /notfound - 404 Not Found');
    console.log('  GET /servererror - 500 Internal Server Error');
    console.log('  GET /timeout - Never responds (timeout test)');
    console.log('  GET /slow - Responds after 5 seconds');
    console.log('  GET /moex/candles - Simulates MOEX API');
    console.log('  GET /health - Health check');
});

// Graceful shutdown
process.on('SIGTERM', () => {
    console.log('SIGTERM signal received: closing HTTP server');
    server.close(() => {
        console.log('HTTP server closed');
        process.exit(0);
    });
});