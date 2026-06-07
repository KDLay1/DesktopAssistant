# 1. 基础配置（适配你的 Qt 6.11）
cmake_minimum_required(VERSION 3.21)
project(DesktopAssistant LANGUAGES CXX)

# 2. 开启C++17（你的项目用到了C++17特性）
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 3. Qt自动工具（必须开启，处理MOC/UIC/RCC）
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 4. 找到Qt 6 核心模块（你的项目用到的组件）
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql Multimedia)
# 5. 包含所有层级目录（和你的实际结构完全对齐）
include_directories(
    ${PROJECT_SOURCE_DIR}          # 根目录，方便写#include "models/xxx.h"
    ${PROJECT_SOURCE_DIR}/base
    ${PROJECT_SOURCE_DIR}/models
    ${PROJECT_SOURCE_DIR}/services
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/app
    ${PROJECT_SOURCE_DIR}/src/data
    ${PROJECT_SOURCE_DIR}/src/pages
)

# 6. 列出所有源文件（已修正base目录文件名，和你的实际文件完全对齐）
add_executable(DesktopAssistant
    # 程序入口
    src/main.cpp

    # 基础工具层（base/，已修正文件名，补全所有文件）
    base/base_operationresult.h
    base/base_validator.h
    base/date_extension.h
    base/enumerable_extension.h
    base/limited_queue.h

    # 数据模型层（models/）
    models/AccountBalance.h
    models/AccountID.h
    models/AccountItem.h
    models/AccountStatusOptions.h
    models/bill.h
    models/bill.cpp
    models/CategoryID.h
    models/CategoryItem.h
    models/categorymanager.h
    models/categorymanager.cpp
    models/eventsummary.h
    models/financialsummary.h
    models/InsideAccountItem.h
    models/MoneyRecord.h

    # 业务服务层（services/）
    services/accountservice.h
    services/accountservice.cpp
    services/billmanager.h
    services/billmanager.cpp
    services/calculateservice.h
    services/calculateservice.cpp
    services/smartfillservice.h
    services/smartfillservice.cpp
  

    # 主窗口（src/app/）
    src/app/MainWindow.h
    src/app/MainWindow.cpp

    # 数据库层（src/data/）
    src/data/DatabaseManager.h
    src/data/DatabaseManager.cpp

    # 界面层（src/pages/）
    src/pages/FinancePage.h
    src/pages/FinancePage.cpp
    src/pages/addbilldialog.h
    src/pages/addbilldialog.cpp
    src/pages/ReportPage.h
    src/pages/ReportPage.cpp
    src/pages/TimerPage.h
    src/pages/TimerPage.cpp
    src/pages/DesktopPet.h
    src/pages/DesktopPet.cpp
    src/pages/PomodoroStatsPage.h     
    src/pages/PomodoroStatsPage.cpp 
    src/data/res.qrc
    res.qrc
)


# 7. 链接Qt库（必须，否则会出现undefined reference错误）
target_link_libraries(DesktopAssistant
    PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Sql
    Qt6::Multimedia
)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})