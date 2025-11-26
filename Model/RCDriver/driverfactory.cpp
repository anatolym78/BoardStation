#include <QDebug>

#include "driverfactory.h"
#include "serialdriver.h" // Для SerialDriver


namespace drv {

std::shared_ptr<IDriver> DriverFactory::createDriver() {
    return std::make_shared<SerialDriver>();
}

}
