/**
 * @file TimerPage.cpp
 * @brief 番茄钟计时器页面实现文件
 * 
 * 所属分层：Pages（界面层）
 * 功能描述：实现番茄钟计时器的完整功能，包括专注/休息计时、
 *           UI交互、音频播放、桌面宠物联动和数据持久化
 * 
 * @author tys
 * @date 2026-06-08
 */

#include "TimerPage.h"
#include "DesktopPet.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QLineEdit>
#include <QSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QCoreApplication>
#include <QUrl>
#include <QDebug>
#include "../data/DatabaseManager.h"
#include <QDateTime>

/**
 * @brief 构造函数：初始化番茄钟页面
 * 
 * 创建桌面宠物实例、初始化定时器、设置UI布局和音频播放器
 * 
 * @param parent 父窗口指针，用于Qt对象树管理
 */
TimerPage::TimerPage(QWidget *parent) : QWidget(parent), isFocusing(false)
{
    // 创建并显示桌面宠物实例
    myPet = new DesktopPet(); 
    myPet->show(); 
    
    // 创建倒计时定时器，每秒触发一次更新
    countdownTimer = new QTimer(this);
    connect(countdownTimer, &QTimer::timeout, this, &TimerPage::updateTimer);

    // 初始化UI布局和音频播放器
    setupUI();
    setupAudio();
}

/**
 * @brief 析构函数：清理资源
 * 
 * 关闭并删除桌面宠物实例，防止内存泄漏
 */
TimerPage::~TimerPage() {
    if (myPet) {
        myPet->close();
        delete myPet;
    }
}

/**
 * @brief 设置UI布局
 * 
 * 创建完整的用户界面，包括：
 * - 设置面板（任务名称、专注时长、休息时长）
 * - 倒计时显示区
 * - 控制按钮区（开始专注、开始休息、放弃）
 * 
 * @note 所有样式使用硬编码字符串，后续可考虑提取为常量
 */
void TimerPage::setupUI()
{
    // 创建主垂直布局，设置对齐方式和边距
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignTop);
    mainLayout->setContentsMargins(40, 40, 40, 40);

    // ==========================================
    // 1. 设置面板 (任务名、专注时长、休息时长)
    // ==========================================
    QGroupBox *settingsGroup = new QGroupBox("番茄钟设置", this);
    settingsGroup->setStyleSheet("QGroupBox { font-size: 16px; font-weight: bold; border: 2px solid #bdc3c7; border-radius: 8px; margin-top: 20px; } QGroupBox::title { subcontrol-origin: margin; left: 20px; padding: 0 5px; }");
    
    // 创建表单布局，用于排列设置项
    QFormLayout *formLayout = new QFormLayout(settingsGroup);
    formLayout->setContentsMargins(20, 30, 20, 20);
    formLayout->setSpacing(15);

    // 任务名称输入框
    taskNameInput = new QLineEdit(this);
    taskNameInput->setPlaceholderText("例如：复习高等数学...");
    taskNameInput->setStyleSheet("padding: 8px; font-size: 14px; border-radius: 4px; border: 1px solid #ccc;");

    // 专注时长选择器（1-120分钟，默认25分钟）
    focusTimeSpin = new QSpinBox(this);
    focusTimeSpin->setRange(1, 120);
    focusTimeSpin->setValue(25); // 默认25分钟
    focusTimeSpin->setSuffix(" 分钟");
    focusTimeSpin->setStyleSheet("padding: 8px; font-size: 14px;");

    // 休息时长选择器（1-60分钟，默认5分钟）
    restTimeSpin = new QSpinBox(this);
    restTimeSpin->setRange(1, 60);
    restTimeSpin->setValue(5); // 默认5分钟
    restTimeSpin->setSuffix(" 分钟");
    restTimeSpin->setStyleSheet("padding: 8px; font-size: 14px;");

    // 将设置项添加到表单布局
    formLayout->addRow("🎯 任务名称:", taskNameInput);
    formLayout->addRow("⏱️ 专注时长:", focusTimeSpin);
    formLayout->addRow("☕ 休息时长:", restTimeSpin);

    // ==========================================
    // 2. 倒计时显示区
    // ==========================================
    // 状态标签，显示当前状态信息
    statusLabel = new QLabel("准备就绪", this);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet("font-size: 22px; font-weight: bold; color: #34495E; margin-top: 30px;");

    // 时间显示标签，大字体显示倒计时
    timeDisplayLabel = new QLabel("25:00", this);
    timeDisplayLabel->setAlignment(Qt::AlignCenter);
    timeDisplayLabel->setStyleSheet("font-size: 100px; font-weight: bold; color: #E74C3C; font-family: 'Arial';");

    // ==========================================
    // 3. 控制按钮区
    // ==========================================
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(20);
    
    // 创建三个控制按钮
    btnStartPomo = new QPushButton("🚀 开始专注", this);
    btnStartRest = new QPushButton("🌿 开始休息", this);
    btnStop = new QPushButton("⏹️ 放弃", this);

    // 设置按钮样式，不同功能使用不同颜色
    QString btnStyle = "QPushButton { font-size: 18px; font-weight: bold; color: white; border-radius: 8px; padding: 12px 0px; }";
    btnStartPomo->setStyleSheet(btnStyle + "background-color: #E74C3C;");
    btnStartRest->setStyleSheet(btnStyle + "background-color: #2ECC71;");
    btnStop->setStyleSheet(btnStyle + "background-color: #95A5A6;");
    
    // 设置鼠标悬停样式
    btnStartPomo->setCursor(Qt::PointingHandCursor);
    btnStartRest->setCursor(Qt::PointingHandCursor);
    btnStop->setCursor(Qt::PointingHandCursor);

    // 将按钮添加到水平布局
    btnLayout->addWidget(btnStartPomo);
    btnLayout->addWidget(btnStartRest);
    btnLayout->addWidget(btnStop);

    // 组装所有模块到主布局
    mainLayout->addWidget(settingsGroup);
    mainLayout->addWidget(statusLabel);
    mainLayout->addWidget(timeDisplayLabel);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(btnLayout);
    mainLayout->addStretch();

    // 连接按钮信号到对应的槽函数
    connect(btnStartPomo, &QPushButton::clicked, this, &TimerPage::startPomodoro);
    connect(btnStartRest, &QPushButton::clicked, this, &TimerPage::startRest);
    connect(btnStop, &QPushButton::clicked, this, &TimerPage::stopTimer);
}

/**
 * @brief 设置音频播放器
 * 
 * 初始化白噪音和闹铃播放器，音频文件位于可执行文件所在目录的res/audio/文件夹下
 * 
 * @note 音频文件路径硬编码，建议后续使用Qt资源系统(.qrc)管理
 * @warning 如果音频文件不存在，播放操作将静默失败
 */
void TimerPage::setupAudio()
{
    // 获取 exe 所在路径，拼接音频文件路径
    QString appPath = QCoreApplication::applicationDirPath();
    
    // 1. 白噪音（专注时播放）
    bgmPlayer = new QMediaPlayer(this);
    bgmOutput = new QAudioOutput(this);
    bgmPlayer->setAudioOutput(bgmOutput);
    bgmPlayer->setSource(QUrl::fromLocalFile(appPath + "/res/audio/white_noise.mp3"));
    bgmPlayer->setLoops(QMediaPlayer::Infinite); // 无限循环播放
    bgmOutput->setVolume(0.5); // 音量 50%

    // 2. 闹铃（时间到时播放）
    alarmPlayer = new QMediaPlayer(this);
    alarmOutput = new QAudioOutput(this);
    alarmPlayer->setAudioOutput(alarmOutput);
    alarmPlayer->setSource(QUrl::fromLocalFile(appPath + "/res/audio/alarm.wav"));
    alarmOutput->setVolume(0.8); 
}

/**
 * @brief 开始专注计时
 * 
 * 启动专注模式，设置倒计时、更新UI状态、联动桌面宠物、播放白噪音
 * 
 * @note 专注期间禁用设置面板，防止用户修改参数
 */
void TimerPage::startPomodoro() {
    isFocusing = true;
    // 如果任务名称为空，使用默认名称
    currentTaskName = taskNameInput->text().isEmpty() ? "未命名任务" : taskNameInput->text();
    focusStartTime = QDateTime::currentDateTime();
    // 读取自定义时间，转换为秒
    remainingSeconds = focusTimeSpin->value() * 60;
    setTimeDisplay(remainingSeconds);
    
    // 更新状态显示
    statusLabel->setText(QString("正在专注：%1").arg(currentTaskName));
    timeDisplayLabel->setStyleSheet("font-size: 100px; font-weight: bold; color: #E74C3C; font-family: 'Arial';");
    
    // 禁用设置，防止专注中途修改
    taskNameInput->setEnabled(false);
    focusTimeSpin->setEnabled(false);
    restTimeSpin->setEnabled(false);

    // 桌宠联动：切换到专注状态并显示提示气泡
    myPet->changeState("focus");            
    myPet->popBubble(QString("开始专注：%1！加油鸭！").arg(currentTaskName)); 

    // 停止闹铃，播放白噪音
    alarmPlayer->stop();
    bgmPlayer->play();

    // 启动倒计时定时器，每秒触发一次
    countdownTimer->start(1000);
}

/**
 * @brief 开始休息计时
 * 
 * 启动休息模式，设置倒计时、更新UI状态、联动桌面宠物、停止白噪音
 * 
 * @note 休息期间不保存记录，仅作为放松时段
 */
void TimerPage::startRest() {
    isFocusing = false;
    // 读取休息时长，转换为秒
    remainingSeconds = restTimeSpin->value() * 60;
    setTimeDisplay(remainingSeconds);
    
    // 更新状态显示，使用绿色主题
    statusLabel->setText("休息中，喝口水放松一下吧~");
    timeDisplayLabel->setStyleSheet("font-size: 100px; font-weight: bold; color: #2ECC71; font-family: 'Arial';");

    // 桌宠联动：切换到休息状态并显示提示气泡
    myPet->changeState("rest");             
    myPet->popBubble("辛苦啦，放松一下吧~ Zzz...");

    // 停止所有音频播放
    bgmPlayer->stop();
    alarmPlayer->stop();

    // 启动倒计时定时器，每秒触发一次
    countdownTimer->start(1000);
}

/**
 * @brief 停止计时器
 * 
 * 停止所有计时和音频播放，恢复设置面板，重置显示状态
 * 
 * @note 停止后不会保存任何记录
 */
void TimerPage::stopTimer() {
    // 停止所有计时和音频
    countdownTimer->stop();
    bgmPlayer->stop();
    alarmPlayer->stop();

    // 重置显示为初始状态
    setTimeDisplay(focusTimeSpin->value() * 60);
    statusLabel->setText("已停止");
    timeDisplayLabel->setStyleSheet("font-size: 100px; font-weight: bold; color: #95A5A6; font-family: 'Arial';");

    // 恢复设置面板，允许用户修改参数
    taskNameInput->setEnabled(true);
    focusTimeSpin->setEnabled(true);
    restTimeSpin->setEnabled(true);

    // 桌宠联动：切换到空闲状态并显示提示气泡
    myPet->changeState("idle");             
    myPet->popBubble("怎么停下了？要摸鱼了吗？");
}

/**
 * @brief 更新倒计时显示
 * 
 * 每秒调用一次，减少剩余秒数并更新显示
 * 当倒计时归零时，停止计时、播放闹铃、保存专注记录
 * 
 * @note 专注记录通过DatabaseManager保存到SQLite数据库
 */
void TimerPage::updateTimer() {
    remainingSeconds--;
    setTimeDisplay(remainingSeconds);

    // 检查倒计时是否结束
    if (remainingSeconds <= 0) {
        // 1. 立即停止当前计时，避免重复触发，并播放闹铃
        countdownTimer->stop();
        bgmPlayer->stop();
        alarmPlayer->play(); 

        // ==========================================
        // 2. 状态分流处理
        // ==========================================
        if (isFocusing) {
            // A. 刚才在专注：保存数据并自动进入休息
            QDateTime focusEndTime = QDateTime::currentDateTime();
            int duration = focusTimeSpin->value(); // 获取设定的专注分钟数
            
            // 写入数据库
            bool success = DatabaseManager::instance().addPomodoroRecord(
                currentTaskName, duration, focusStartTime, focusEndTime
            );
            
            if (success) {
                qDebug() << "🎉 成功存入数据库！任务：" << currentTaskName << "，时长：" << duration << "分钟";
                myPet->popBubble("专注记录已保存！主人真棒！📝", 3000);
            } else {
                qDebug() << "❌ 数据库保存失败，请检查数据库连接";
            }

            // 为了用户体验，延迟 1.5 秒再自动进入休息（让用户能看清专注成功的提示）
            statusLabel->setText("专注结束！即将自动进入休息...");
            QTimer::singleShot(1500, this, [=]() {
                startRest(); 
            });

        } else {
            // B. 刚才在休息：休息结束，恢复初始状态
            stopTimer(); // 恢复设置面板
            statusLabel->setText("休息结束，准备开始下一轮吧！");
            timeDisplayLabel->setStyleSheet("font-size: 100px; font-weight: bold; color: #34495E; font-family: 'Arial';");
            
            myPet->changeState("idle");
            myPet->popBubble("休息结束啦！快开始新一轮的专注吧~ ✨", 3000);
        }
    }
}

void TimerPage::setTimeDisplay(int totalSeconds) {
    int m = totalSeconds / 60;
    int s = totalSeconds % 60;
    timeDisplayLabel->setText(QString("%1:%2").arg(m, 2, 10, QChar('0')).arg(s, 2, 10, QChar('0')));
}