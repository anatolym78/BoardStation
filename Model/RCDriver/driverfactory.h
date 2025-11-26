#pragma once

#include <memory>

#include "driverinterface.h" // Для drv::IDriver

namespace drv {

class DriverFactory {
public:
    // Статический метод, который создает нужный драйвер по типу
    static std::shared_ptr<IDriver> createDriver();
};

}
