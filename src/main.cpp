/**
 * @file main.cpp
 * @brief 应用程序入口点
 *
 * 本文件属于应用层（App），负责初始化全局资源（如数据库）并启动主窗口。
 * 依赖关系：
 *   - Qt 框架层：QApplication、QMessageBox
 *   - 应用层：MainWindow
 *   - 数据层：DatabaseManager
 *
 * 分层规范：应用层可以依赖数据层和应用层自身，无逆向依赖。
 *
 * @author 项目团队
 * @date 2026-06-08
 */

#include <QApplication>
#include <QMessageBox>
#include "app/MainWindow.h"
#include "data/DatabaseManager.h"

/**
 * @brief 程序主函数
 *
 * 负责初始化 Qt 应用程序、数据库连接和表结构，然后启动主窗口。
 * 如果数据库初始化失败，程序将弹出错误对话框并退出。
 *
 * @param argc 命令行参数个数
 * @param argv 命令行参数数组
 * @return int 程序退出码：
 *   - 0：正常退出
 *   - -1：数据库初始化失败
 *
 * @note 数据库文件（lifemate.db）将在当前运行目录下自动生成。
 * @note 本函数不涉及业务逻辑校验，因此未使用 Validator 或 OperationResult。
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 1. 初始化数据库 (在当前运行目录下生成 lifemate.db)
    if (!DatabaseManager::instance().openDatabase("lifemate.db")) {
        QMessageBox::critical(nullptr, "致命错误", "无法打开数据库，程序即将退出！");
        return -1;
    }

    // 2. 初始化数据表结构
    if (!DatabaseManager::instance().initTables()) {
        QMessageBox::critical(nullptr, "致命错误", "数据库表初始化失败！");
        return -1;
    }

    // 3. 启动主窗口
    MainWindow w;
    w.show();

    return a.exec();
}