/**
 * @file DesktopPet.cpp
 * @brief 桌面宠物控件实现文件
 * 
 * 所属分层：Pages（界面层）
 * 功能：提供桌面宠物动画显示、交互反馈（拖拽、双击、气泡提示）
 * 
 * @author 开发团队
 * @date 2026-06-08
 */

#include "DesktopPet.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QMenu>
#include <QTimer>
#include <QApplication>
#include <QScreen>
#include <QDebug>
#include <QCoreApplication>
#include <QRandomGenerator> // 用于生成随机气泡

/**
 * @brief 构造函数：初始化桌面宠物控件
 * 
 * @param parent 父窗口指针
 * 
 * 初始化流程：
 * 1. 设置无边框、置顶、工具窗口属性
 * 2. 设置透明背景
 * 3. 创建UI组件和右键菜单
 * 4. 初始化动画定时器
 * 5. 将窗口移动到屏幕右下角
 * 6. 启动空闲状态动画
 */
DesktopPet::DesktopPet(QWidget *parent) 
    : QWidget(parent), isDragging(false), currentFrameIndex(0)
{
    // 设置窗口属性：无边框、置顶、工具窗口
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    // 设置透明背景，支持不规则形状
    setAttribute(Qt::WA_TranslucentBackground);

    setupUI();      // 创建UI组件
    setupMenu();    // 创建右键菜单

    // 初始化动画定时器，连接帧更新槽函数
    animTimer = new QTimer(this);
    connect(animTimer, &QTimer::timeout, this, &DesktopPet::updateFrame);

    // 获取主屏幕可用区域，将窗口移动到右下角
    QRect screenRect = QApplication::primaryScreen()->availableGeometry();
    move(screenRect.width() - 250, screenRect.height() - 250);

    // 启动空闲状态动画
    changeState("idle"); 
}

/**
 * @brief 设置UI组件：创建气泡标签和宠物标签
 * 
 * 布局结构：
 * - 垂直布局
 *   - 气泡标签（居中，底部对齐）
 *   - 宠物标签（居中，底部对齐）
 */
void DesktopPet::setupUI()
{
    // 创建垂直布局，设置无外边距
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    // 创建气泡标签：用于显示提示文字
    bubbleLabel = new QLabel(this);
    bubbleLabel->setStyleSheet(R"(
        QLabel {
            background-color: rgba(255, 255, 255, 240);
            border: 2px solid #ffb19e;
            border-radius: 15px;
            padding: 12px;
            font-family: "Microsoft YaHei";
            font-size: 15px;
            font-weight: bold;
            color: #333333;
        }
    )");
    bubbleLabel->setAlignment(Qt::AlignCenter);     // 文字居中
    bubbleLabel->setWordWrap(true);                 // 自动换行
    bubbleLabel->hide();                            // 初始隐藏

    // 创建宠物标签：用于显示动画帧
    petLabel = new QLabel(this);
    petLabel->setFixedSize(160, 160);               // 固定大小
    petLabel->setAttribute(Qt::WA_TransparentForMouseEvents); // 鼠标事件穿透
    
    // 将组件添加到布局
    layout->addWidget(bubbleLabel, 0, Qt::AlignHCenter | Qt::AlignBottom);
    layout->addWidget(petLabel, 0, Qt::AlignHCenter | Qt::AlignBottom);
}

/**
 * @brief 设置右键菜单：包含隐藏和退出功能
 * 
 * 菜单项：
 * - 暂时隐藏：隐藏宠物窗口
 * - 退出程序：关闭应用程序
 */
void DesktopPet::setupMenu()
{
    // 创建右键菜单，设置样式
    rightClickMenu = new QMenu(this);
    rightClickMenu->setStyleSheet(
        "QMenu { background-color: white; border-radius: 8px; border: 1px solid #ccc; font-family: 'Microsoft YaHei'; font-size: 14px; padding: 5px; }"
        "QMenu::item { padding: 8px 20px; }"
        "QMenu::item:selected { background-color: #f0f0f0; border-radius: 5px; }"
    );
    
    // 创建菜单动作
    QAction *actHide = new QAction("🙈 暂时隐藏", this);  // 隐藏窗口
    QAction *actQuit = new QAction("❌ 退出程序", this);  // 退出程序

    // 绑定动作信号
    connect(actHide, &QAction::triggered, this, &QWidget::hide);
    connect(actQuit, &QAction::triggered, qApp, &QApplication::quit);

    // 将动作添加到菜单
    rightClickMenu->addAction(actHide);
    rightClickMenu->addSeparator();  // 添加分隔线
    rightClickMenu->addAction(actQuit);
}

/**
 * @brief 播放指定动画序列
 * 
 * @param prefix 动画帧文件前缀（如 "src/res/role/ChrisKitty/action/stand_"）
 * @param maxFrames 动画总帧数
 * @param intervalMs 帧切换间隔（毫秒）
 * 
 * 逻辑说明：
 * 1. 停止当前动画，清空帧列表
 * 2. 加载指定路径下的所有帧图片
 * 3. 如果加载成功，启动定时器开始播放
 * 
 * @note 图片路径使用 ":/" 前缀，表示从 Qt 资源系统加载
 */
void DesktopPet::playAnimation(const QString& prefix, int maxFrames, int intervalMs)
{
    animTimer->stop();       // 停止当前动画
    currentFrames.clear();   // 清空帧列表

    // 加载所有帧图片
    for (int i = 0; i < maxFrames; ++i) {
        // 拼接完整资源路径：使用 ":/" 前缀从 Qt 资源系统加载
        QString imgPath = ":/" + prefix + QString::number(i) + ".png"; 
        
        QPixmap pix(imgPath);  // 加载图片
        
        if (!pix.isNull()) {
            // 缩放图片以适应宠物标签大小，保持宽高比，使用平滑缩放
            pix = pix.scaled(petLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            currentFrames.append(pix);  // 添加到帧列表
        } else {
            // 图片加载失败时输出调试信息
            qDebug() << "图片加载失败，请检查 qrc 路径拼写：" << imgPath;
        }
    }

    // 如果成功加载了帧，则启动动画
    if (!currentFrames.isEmpty()) {
        currentFrameIndex = 0;          // 重置帧索引
        animTimer->start(intervalMs);   // 启动定时器
        updateFrame();                  // 立即显示第一帧
    }
}

/**
 * @brief 更新动画帧：切换到下一帧显示
 * 
 * 循环播放：当播放到最后一帧时，回到第一帧
 */
void DesktopPet::updateFrame()
{
    if (currentFrames.isEmpty()) return;  // 无帧数据时直接返回
    
    // 设置当前帧图片
    petLabel->setPixmap(currentFrames[currentFrameIndex]);
    // 更新帧索引，实现循环播放
    currentFrameIndex = (currentFrameIndex + 1) % currentFrames.size();
}

/**
 * @brief 切换宠物状态，播放对应动画
 * 
 * @param state 状态名称
 *   - "idle": 空闲状态，播放站立动画（5帧，200ms间隔）
 *   - "focus": 专注状态，播放地面动画（8帧，150ms间隔）
 *   - "rest": 休息状态，播放站立动画（5帧，500ms间隔）
 *   - "finish": 完成状态，播放拖拽动画（1帧，100ms间隔）
 */
void DesktopPet::changeState(const QString& state)
{
    if (state == "idle") {
        playAnimation("src/res/role/ChrisKitty/action/stand_", 5, 200);
    } 
    else if (state == "focus") {
        playAnimation("src/res/role/ChrisKitty/action/onfloor_", 8, 150);
    } 
    else if (state == "rest") {
        playAnimation("src/res/role/ChrisKitty/action/stand_", 5, 500); 
    }
    else if (state == "finish") { 
        playAnimation("src/res/role/ChrisKitty/action/drag_", 1, 100); 
    }
}

/**
 * @brief 显示气泡提示文字
 * 
 * @param text 要显示的文本内容
 * @param durationMs 显示持续时间（毫秒）
 * 
 * 功能：在宠物上方显示一个带样式的气泡，指定时间后自动隐藏
 */
void DesktopPet::popBubble(const QString& text, int durationMs)
{
    bubbleLabel->setText(text);         // 设置文本
    bubbleLabel->adjustSize();          // 根据内容调整大小
    bubbleLabel->show();                // 显示气泡
    // 定时隐藏气泡
    QTimer::singleShot(durationMs, bubbleLabel, &QLabel::hide);
}

// =====================
// 交互核心
// =====================

/**
 * @brief 鼠标按下事件处理
 * 
 * @param event 鼠标事件对象
 * 
 * 功能：
 * - 左键按下：记录拖拽偏移，设置拖拽状态，显示随机鼓励语录
 * - 右键按下：弹出右键菜单
 */
void DesktopPet::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        // 计算拖拽偏移量：鼠标位置与窗口左上角的差值
        dragOffset = event->globalPosition().toPoint() - frameGeometry().topLeft();
        isDragging = true;              // 设置拖拽状态
        setCursor(Qt::ClosedHandCursor); // 设置拖拽光标
        
        // 随机显示鼓励语录
        QStringList quotes = {
            "喵呜~ 要专心哦！",
            "本喵在看着你呢！👀",
            "加油加油！你是最棒的！",
            "不要摸鱼啦，快去学习！",
            "完成任务就给你摸肚皮~"
        };
        int randIdx = QRandomGenerator::global()->bounded(quotes.size());
        popBubble(quotes[randIdx], 2000);  // 显示2秒

    } else if (event->button() == Qt::RightButton) {
        // 右键：在鼠标位置弹出菜单
        rightClickMenu->exec(event->globalPosition().toPoint());
    }
}

/**
 * @brief 鼠标移动事件处理
 * 
 * @param event 鼠标事件对象
 * 
 * 功能：在拖拽状态下，根据鼠标移动更新窗口位置
 */
void DesktopPet::mouseMoveEvent(QMouseEvent *event) {
    if (isDragging && (event->buttons() & Qt::LeftButton)) {
        // 计算新位置：鼠标当前位置减去拖拽偏移
        move(event->globalPosition().toPoint() - dragOffset);
    }
}

/**
 * @brief 鼠标释放事件处理
 * 
 * @param event 鼠标事件对象
 * 
 * 功能：结束拖拽，恢复默认光标
 */
void DesktopPet::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isDragging = false;             // 结束拖拽
        setCursor(Qt::ArrowCursor);     // 恢复默认光标
    }
}

/**
 * @brief 鼠标双击事件处理
 * 
 * @param event 鼠标事件对象
 * 
 * 功能：
 * 1. 发射 petDoubleClicked 信号，通知主界面显示
 * 2. 显示提示气泡
 */
void DesktopPet::mouseDoubleClickEvent(QMouseEvent *event) {
    // 发射双击信号，通知主界面弹出
    emit petDoubleClicked();
    // 显示提示气泡
    popBubble("主人呼唤，面板展开！✨", 2000);
}