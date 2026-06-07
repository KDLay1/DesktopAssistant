/**
 * @file addbilldialog.h
 * @brief 添加账单对话框界面层
 *
 * 所属分层：Pages（界面层）
 * 依赖关系：依赖 Models 层的 Bill 结构体，依赖 Services 层的 SmartFillService
 * 功能描述：提供添加账单的对话框界面，支持智能填充功能
 *
 * @author tys
 * @date 2026-06-08
 */

#ifndef ADDBILLDIALOG_H
#define ADDBILLDIALOG_H

#include <QDialog>
#include "models/bill.h"                // 账单数据模型
#include "services/smartfillservice.h"  // 智能填充服务

class QComboBox;
class QDoubleSpinBox;
class QDateEdit;
class QLineEdit;
class QLabel;

/**
 * @brief 添加账单对话框类
 *
 * 设计目的：为用户提供添加新账单的图形界面，支持从下拉框选择或动态添加
 * 对方、科目、类别等数据，并利用 SmartFillService 提供智能填充提示。
 * 用户填写完成后，通过 getBill() 获取完整的 Bill 对象。
 *
 * @note 金额使用 QDoubleSpinBox（double），但业务层应转换为 int（分）存储
 * @note ID 使用 quint16/int 原始类型，后续应改为强类型（如 AccountID）
 */
class AddBillDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param fillService 智能填充服务指针，用于获取下拉框数据和智能提示
     * @param parent 父窗口指针
     * @note fillService 不能为 nullptr，调用方需确保传入有效指针
     */
    explicit AddBillDialog(SmartFillService* fillService, QWidget *parent = nullptr);

    /**
     * @brief 获取用户填写的账单数据
     * @return Bill 对象，包含用户在当前对话框中选择/输入的所有数据
     * @note 调用此函数前应确保对话框已通过 accept() 确认
     * @warning 返回的 Bill 对象未经过 Validator 校验，调用方需自行校验
     */
    Bill getBill() const;

private slots:
    /**
     * @brief 对方下拉框选项变化时的处理槽函数
     * @param index 当前选中的索引
     * @note 当用户切换对方时，可能需要更新科目和类别的可选范围
     */
    void onCounterpartChanged(int index);

    /**
     * @brief 确认按钮点击处理槽函数
     * @note 执行数据校验，校验通过后调用 accept() 关闭对话框
     * @warning 应在此函数内调用 Validator 进行参数校验
     */
    void onAcceptClicked();

    /**
     * @brief 对方下拉框激活时的处理槽函数（含动态添加选项）
     * @param index 当前激活的索引
     * @note 当用户点击下拉框时触发，可用于动态添加"新增对方"选项
     */
    void onCounterpartActivated(int index);

    /**
     * @brief 科目下拉框激活时的处理槽函数（含动态添加选项）
     * @param index 当前激活的索引
     * @note 当用户点击下拉框时触发，可用于动态添加"新增科目"选项
     */
    void onSubjectActivated(int index);

    /**
     * @brief 类别下拉框激活时的处理槽函数（含动态添加选项）
     * @param index 当前激活的索引
     * @note 当用户点击下拉框时触发，可用于动态添加"新增类别"选项
     */
    void onCategoryActivated(int index);

private:
    /**
     * @brief 初始化用户界面
     * @note 创建并布局所有 UI 控件，设置初始状态
     */
    void setupUI();

    /**
     * @brief 加载初始数据到下拉框
     * @note 从 SmartFillService 获取初始数据，填充到各个下拉框中
     */
    void loadInitialData();

    /** @brief 智能填充服务指针，用于获取下拉框数据和智能提示 */
    SmartFillService* m_fillService;

    /** @brief 账单类型下拉框（收入/支出） */
    QComboBox *typeComboBox;
    /** @brief 对方下拉框 */
    QComboBox *counterpartComboBox;
    /** @brief 科目下拉框 */
    QComboBox *subjectComboBox;
    /** @brief 类别下拉框 */
    QComboBox *categoryComboBox;
    /** @brief 金额输入控件（注意：使用 double，业务层需转换为 int 分） */
    QDoubleSpinBox *amountSpinBox;
    /** @brief 日期选择控件 */
    QDateEdit *dateEdit;
    /** @brief 备注输入控件 */
    QLineEdit *remarksLineEdit;
    /** @brief 智能填充提示标签 */
    QLabel *smartFillHintLabel;

    /**
     * @brief 用于生成动态对方 ID 的自增计数器
     * @note 当用户动态添加对方时，使用此计数器生成临时 ID
     * @warning 应使用强类型 ID（如 CounterpartID），当前为临时方案
     */
    quint16 m_nextCounterpartId;

    /**
     * @brief 用于生成动态科目 ID 的自增计数器
     * @note 当用户动态添加科目时，使用此计数器生成临时 ID
     * @warning 应使用强类型 ID（如 SubjectID），当前为临时方案
     */
    quint16 m_nextSubjectId;

    /**
     * @brief 用于生成动态支出类别 ID 的自增计数器
     * @note 当用户动态添加支出类别时，使用此计数器生成临时 ID
     * @warning 应使用强类型 ID（如 CategoryID），当前为临时方案
     */
    int m_nextExpenseCategoryPrimary;

    /**
     * @brief 用于生成动态收入类别 ID 的自增计数器
     * @note 当用户动态添加收入类别时，使用此计数器生成临时 ID
     * @warning 应使用强类型 ID（如 CategoryID），当前为临时方案
     */
    int m_nextIncomeCategoryPrimary;
};

#endif // ADDBILLDIALOG_H