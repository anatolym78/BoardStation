# Инструкции по сборке BoardStation с QML интерфейсом

## Предварительные требования

1. **Qt 5.12 или выше** с установленными модулями:
   - Qt Core
   - Qt GUI
   - Qt Widgets
   - Qt Quick
   - Qt Charts

2. **Компилятор C++17** (GCC, Clang, MSVC)

## Структура проекта

После добавления QML интерфейса проект содержит:

```
BoardStation/
├── main.cpp                    # Главный файл с макросом QML_GUI
├── mainwindow.h/cpp/ui        # Старый Qt Widgets интерфейс
├── QmlMainWindow.h/cpp        # Новый QML интерфейс
├── main.qml                   # Полный QML интерфейс с графиками
├── main_simple.qml            # Упрощенный QML интерфейс
├── qml.qrc                    # Ресурсы Qt для QML
├── BoardStation.pro           # Файл проекта Qt
└── README_QML_Interface.md    # Документация по QML
```

## Сборка проекта

### 1. Использование qmake

```bash
# Перейти в директорию проекта
cd BoardStation

# Создать Makefile
qmake BoardStation.pro

# Собрать проект
make
```

### 2. Использование Qt Creator

1. Открыть `BoardStation.pro` в Qt Creator
2. Выбрать комплект сборки (kit)
3. Нажать Ctrl+B для сборки

## Переключение между интерфейсами

### Включение QML интерфейса

В файле `main.cpp` раскомментировать:

```cpp
#define QML_GUI
```

### Включение старого интерфейса

В файле `main.cpp` закомментировать:

```cpp
// #define QML_GUI
```

## Возможные проблемы и решения

### 1. Ошибка "Qt Quick module not found"

**Решение**: Установить Qt Quick модуль:
```bash
# Ubuntu/Debian
sudo apt-get install qml-module-qtquick2

# Windows: Переустановить Qt с модулем Quick
```

### 2. Ошибка "QML file not found"

**Решение**: Проверить, что файл `qml.qrc` включен в проект и содержит правильные пути.

### 3. Ошибки компиляции QML

**Решение**: Использовать `main_simple.qml` вместо `main.qml` для лучшей совместимости.

### 4. Проблемы с версиями Qt

**Решение**: Убедиться, что все модули Qt имеют одинаковую версию.

## Тестирование

### Тест QML интерфейса

```bash
# Компиляция теста
g++ -std=c++17 test_qml.cpp \
    -I$(pkg-config --cflags Qt5Core Qt5Gui Qt5Quick Qt5Qml) \
    -L$(pkg-config --libs Qt5Core Qt5Gui Qt5Quick Qt5Qml) \
    -o test_qml

# Запуск
./test_qml
```

### Тест основного приложения

```bash
# Сборка
make

# Запуск
./BoardStation
```

## Отладка

### Включение отладочной информации

В `QmlMainWindow.cpp` добавлены `qDebug()` сообщения для отслеживания:

- Загрузки QML файла
- Установки приложения
- Настройки моделей
- Обновления параметров

### Просмотр QML ошибок

QML ошибки выводятся в консоль при запуске приложения.

## Производительность

- QML интерфейс может быть медленнее при первом запуске
- Для лучшей производительности используйте Release сборку
- При необходимости отключите антиалиасинг в QML

## Совместимость

- Qt 5.12+ (рекомендуется Qt 5.15+)
- Windows 7+, macOS 10.12+, Linux (Ubuntu 16.04+)
- OpenGL 2.1+ для QML рендеринга
