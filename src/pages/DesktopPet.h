/**
 * @file DesktopPet.h
 * @brief 桌面宠物组件 - 界面层
 * 
 * 所属分层：Pages（界面层）
 * 功能描述：实现一个可拖拽、可动画、可显示气泡提示的桌面宠物
 * 
 * @author tys
 * @date 2026-06-08
 */

#ifndef DESKTOPPET_H
#define DESKTOPPET_H

#include <QWidget>
#include <QPoint>
#include <QVector>
#include <QPixmap>

class QLabel;
class QTimer;
class QMenu;

/**
 * @brief 桌面宠物组件
 * 
 * 设计目的：提供桌面宠物功能，支持动画播放、拖拽移动、气泡提示和交互事件
 * 职责：
 * - 管理宠物动画帧的加载和播放
 * - 处理鼠标拖拽移动
 * - 显示气泡提示信息
 * - 提供右键菜单
 * - 触发双击事件信号
 */
class DesktopPet : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父窗口指针，默认为nullptr
     * 
     * 初始化宠物界面、动画定时器和右键菜单
     */
    explicit DesktopPet(QWidget *parent = nullptr);
    
    /**
     * @brief 切换宠物状态
     * @param state 状态名称（如 "idle", "happy", "sad"）
     * 
     * 根据状态名称加载对应的动画帧序列并开始播放
     * 注意：状态名称应与资源文件中的动画帧前缀对应
     */
    void changeState(const QString& state); 
    
    /**
     * @brief 显示气泡提示
     * @param text 提示文本内容
     * @param durationMs 显示持续时间（毫秒），默认3000ms
     * 
     * 在宠物上方显示气泡样式的提示信息
     * 到达指定时间后自动隐藏
     */
    void popBubble(const QString& text, int durationMs = 3000); 

// 【新增信号】：用于通知主界面
signals:
    /**
     * @brief 宠物被双击信号
     * 
     * 当用户双击宠物时触发，用于通知主界面执行相应操作
     */
    void petDoubleClicked(); 

protected:
    /**
     * @brief 鼠标按下事件处理
     * @param event 鼠标事件对象
     * 
     * 记录拖拽起始位置和偏移量，开始拖拽模式
     */
    void mousePressEvent(QMouseEvent *event) override;
    
    /**
     * @brief 鼠标移动事件处理
     * @param event 鼠标事件对象
     * 
     * 在拖拽模式下，根据鼠标移动更新宠物位置
     */
    void mouseMoveEvent(QMouseEvent *event) override;
    
    /**
     * @brief 鼠标释放事件处理
     * @param event 鼠标事件对象
     * 
     * 结束拖拽模式
     */
    void mouseReleaseEvent(QMouseEvent *event) override;
    
    /**
     * @brief 鼠标双击事件处理
     * @param event 鼠标事件对象
     * 
     * 触发 petDoubleClicked 信号
     */
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private slots:
    /**
     * @brief 更新动画帧
     * 
     * 定时器槽函数，每次调用切换到下一帧
     * 到达最后一帧后循环到第一帧
     */
    void updateFrame(); 

private:
    QLabel *petLabel;           /**< 宠物图像显示标签 */
    QLabel *bubbleLabel;        /**< 气泡提示显示标签 */
    QMenu *rightClickMenu;      /**< 右键菜单 */
    
    bool isDragging;            /**< 是否正在拖拽 */
    QPoint dragOffset;          /**< 拖拽偏移量（鼠标相对于窗口左上角的位置） */

    QTimer *animTimer;          /**< 动画播放定时器 */
    int currentFrameIndex;      /**< 当前动画帧索引 */
    QVector<QPixmap> currentFrames; /**< 当前状态的动画帧序列 */

    /**
     * @brief 初始化用户界面
     * 
     * 创建并设置宠物标签和气泡标签的位置、大小和样式
     */
    void setupUI();
    
    /**
     * @brief 初始化右键菜单
     * 
     * 创建右键菜单项并连接相应的槽函数
     */
    void setupMenu();
    
    /**
     * @brief 播放动画
     * @param prefix 动画帧文件名前缀
     * @param maxFrames 最大帧数
     * @param intervalMs 帧间隔时间（毫秒）
     * 
     * 加载指定前缀的动画帧序列，设置定时器间隔并开始播放
     */
    void playAnimation(const QString& prefix, int maxFrames, int intervalMs);
};

#endif // DESKTOPPET_H