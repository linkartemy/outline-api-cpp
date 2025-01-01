#ifndef OUTLINE_EXCEPTIONS_H
#define OUTLINE_EXCEPTIONS_H

#include <stdexcept>
#include <string>

namespace outline {

/**
 * @brief Базовый класс-исключение для всей библиотеки Outline.
 */
class OutlineException : public std::runtime_error {
 public:
  explicit OutlineException(const std::string& message)
      : std::runtime_error(message) {}
};

/**
 * @brief Исключение, возникающее при сетевых проблемах (разрыв соединения, отказ хоста и т.д.).
 */
class OutlineNetworkException : public OutlineException {
 public:
  explicit OutlineNetworkException(const std::string& message)
      : OutlineException("Network Error: " + message) {}
};

/**
 * @brief Исключение, которое говорит о превышении времени ожидания (таймаут).
 */
class OutlineTimeoutException : public OutlineException {
 public:
  explicit OutlineTimeoutException(const std::string& message)
      : OutlineException("Timeout Error: " + message) {}
};

/**
 * @brief Исключение, возникающее при ошибках парсинга входных данных (JSON, XML, и т.д.).
 */
class OutlineParseException : public OutlineException {
 public:
  explicit OutlineParseException(const std::string& message)
      : OutlineException("Parse Error: " + message) {}
};

/**
 * @brief Исключение, сигнализирующее о том, что сервер вернул недопустимый ответ
 *        или необходимые поля отсутствуют.
 */
class OutlineServerErrorException : public OutlineException {
 public:
  explicit OutlineServerErrorException(const std::string& message)
      : OutlineException("Server Error: " + message) {}
};

}  // namespace outline

#endif  // OUTLINE_EXCEPTIONS_H
