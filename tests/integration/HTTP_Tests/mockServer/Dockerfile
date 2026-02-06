FROM node:20-alpine

WORKDIR /app

# Копируем package.json и устанавливаем зависимости
COPY package.json .
RUN npm install --production

# Копируем сервер
COPY mock_server.js .

# Открываем порт
EXPOSE 8080

# Запускаем сервер
CMD ["npm", "start"]