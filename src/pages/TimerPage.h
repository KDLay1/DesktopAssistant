/**
 * @file TimerPage.h
 * @brief 番茄钟计时器页面
 *
 * 所属分层：Pages（界面层）
 * 功能：提供番茄钟计时功能，包括专注计时、休息计时、任务名称输入、倒计时显示、
 *       计时结束闹钟提醒以及与桌宠模块的交互。
 * 依赖：Qt Widgets, Qt Multimedia, DesktopPet
 *
 * @author tys
 * @date 2026-06-08
 */
#ifndef TIMERPAGE_H
#define TIMERPAGE_H

#include <QWidget>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QDateTime>

// 前向声明，减少头文件依赖
class QLabel;
class QPushButton;
class QTimer;
class QLineEdit;
class QSpinBox;
class DesktopPet;

/**
 * @brief 番茄钟计时器页面类
 *
 * 设计目的：为用户提供一个完整的番茄钟工作法界面。
 * 职责：
 *  - 管理专注/休息时长的设置与启动
 *  - 显示倒计时
 *  - 播放背景音乐和计时结束闹钟
 *  - 与桌宠模块交互（通过 myPet 指针）
 *
 * @note 该类直接持有 DesktopPet 指针，存在潜在的逆向依赖风险。
 *       建议后续重构为通过信号/槽与桌宠模块解耦。
 */
class TimerPage : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口部件，默认为 nullptr
     */
    explicit TimerPage(QWidget *parent = nullptr);

    /**
     * @brief 析构函数
     *
     * 负责清理资源。Qt 的父子对象机制会自动删除大部分子控件，
     * 但 myPet 指针指向的对象需要外部管理或在此处手动清理。
     */
    ~TimerPage();

    /**
     * @brief 桌宠指针
     *
     * 用于与桌宠模块进行交互，例如计时结束时通知桌宠做出反应。
     * @warning 该指针指向的对象生命周期由外部管理，本类不负责其内存释放。
     */
    DesktopPet *myPet;

private slots:
    /**
     * @brief 更新倒计时显示
     *
     * 由 countdownTimer 的 timeout() 信号触发。
     * 每次触发减少 remainingSeconds，并更新 timeDisplayLabel 的显示。
     * 当 remainingSeconds 归零时，停止定时器，播放闹钟，
     * 并根据 isFocusing 状态决定是结束专注还是结束休息。
     */
    void updateTimer();

    /**
     * @brief 开始专注计时
     *
     * 由 btnStartPomo 的 clicked() 信号触发。
     * 从 focusTimeSpin 获取专注时长（分钟），转换为秒并设置 remainingSeconds。
     * 启动 countdownTimer，设置 isFocusing = true，更新状态提示。
     * @note 当前未检查是否已在计时状态，可能导致定时器重复启动。
     *       也未使用 Validator 和 OperationResult，不符合项目规范。
     */
    void startPomodoro();

    /**
     * @brief 开始休息计时
     *
     * 由 btnStartRest 的 clicked() 信号触发。
     * 逻辑与 startPomodoro() 类似，但使用 restTimeSpin 的值，并设置 isFocusing = false。
     * @note 同样存在未检查状态和未使用全局约束的问题。
     */
    void startRest();

    /**
     * @brief 停止当前计时
     *
     * 由 btnStop 的 clicked() 信号触发。
     * 停止 countdownTimer，重置相关状态。
     */
    void stopTimer();

private:
    /**
     * @brief 初始化用户界面
     *
     * 创建并布局所有 UI 控件，包括输入框、标签、按钮等。
     * 连接按钮的 clicked() 信号到对应的私有槽函数。
     */
    void setupUI();

    /**
     * @brief 初始化音频播放器
     *
     * 创建 QMediaPlayer 和 QAudioOutput 实例，用于播放背景音乐和闹钟。
     * 设置音频源文件路径。
     */
    void setupAudio();

    /**
     * @brief 设置倒计时显示
     * @param totalSeconds 总秒数
     *
     * 将 totalSeconds 格式化为 "MM:SS" 格式的字符串，并显示在 timeDisplayLabel 上。
     */
    void setTimeDisplay(int totalSeconds);

    /** @brief 专注开始时间，用于记录本次专注的起始时刻 */
    QDateTime focusStartTime;

    // --- UI 控件 ---
    QLineEdit *taskNameInput;   /**< 任务名称输入框 */
    QSpinBox *focusTimeSpin;    /**< 专注时长设置（分钟） */
    QSpinBox *restTimeSpin;     /**< 休息时长设置（分钟） */

    QLabel *timeDisplayLabel;   /**< 巨大的倒计时显示标签 */
    QLabel *statusLabel;        /**< 状态提示标签（如“正在专注...”、“正在休息...”） */

    QPushButton *btnStartPomo;  /**< 开始专注按钮 */
    QPushButton *btnStartRest;  /**< 开始休息按钮 */
    QPushButton *btnStop;       /**< 停止按钮 */

    // --- 核心逻辑 ---
    QTimer *countdownTimer;     /**< 驱动倒计时的 Qt 定时器 */
    int remainingSeconds;       /**< 当前剩余秒数 */
    QString currentTaskName;    /**< 当前任务名称 */
    bool isFocusing;            /**< 标记当前是否在专注状态（true=专注，false=休息） */

    // --- 音频系统 ---
    QMediaPlayer *bgmPlayer;    /**< 背景音乐播放器 */
    QAudioOutput *bgmOutput;    /**< 背景音乐音频输出 */
    QMediaPlayer *alarmPlayer;  /**< 闹钟播放器 */
    QAudioOutput *alarmOutput;  /**< 闹钟音频输出 */
};

#endif // TIMERPAGE_H