/**
 * @file bill.cpp
 * @brief 账单实体类实现文件
 *
 * 所属分层：Models 层
 * 功能描述：实现 Bill 账单实体的构造函数、事件类型判断方法及运算符重载
 * 依赖关系：依赖 CategoryID（分类ID工具类）、MoneyRecord（金额记录类型）
 * 被依赖：被 Data 层（数据访问）、Services 层（业务逻辑）、Pages 层（界面）使用
 *
 * @author tys
 * @date 2026-06-08
 */

#include "models/bill.h"

/**
 * @brief 构造函数：初始化账单所有成员变量
 *
 * @param dayNumber 日期编号（格式：YYYYMMDD）
 * @param isRefund 是否退款标志
 * @param categoryId 分类ID（quint16，通过 CategoryID 工具类验证）
 * @param subjectId 科目ID（quint16）
 * @param counterpartId 对方ID（quint16）
 * @param amount 金额记录（MoneyRecord 类型，注意项目要求使用 int 分存储）
 * @param remarks 备注文本（QString）
 * @param id 账单唯一标识（int，应改为强类型 BillID）
 *
 * @note 当前未使用 Validator 进行参数校验，违反项目强制约束
 * @note 金额类型应为 int（分），当前使用 MoneyRecord 不符合规范
 * @note ID 类型应为强类型（如 BillID），当前使用 int 不符合规范
 */
Bill::Bill(int dayNumber, bool isRefund, quint16 categoryId, 
           quint16 subjectId, quint16 counterpartId, 
           MoneyRecord amount, const QString& remarks,int id)
    : m_id(id),                    // 账单ID（应改为强类型）
      m_dayNumber(dayNumber),      // 日期编号
      m_isRefund(isRefund),        // 退款标志
      m_categoryId(categoryId),    // 分类ID
      m_subjectId(subjectId),      // 科目ID
      m_counterpartId(counterpartId), // 对方ID
      m_amount(amount),            // 金额（应改为 int 分）
      m_remarks(remarks)           // 备注
{
}

// ==========================================
// 静态判断方法实现
// ==========================================

/**
 * @brief 静态判断是否为支出事件
 *
 * 根据分类ID格式和退款标志判断是否为支出事件
 * 支出事件条件：分类ID为支出格式 且 非退款
 *
 * @param isRefund 是否退款标志
 * @param categoryId 分类ID
 * @return true 是支出事件，false 不是支出事件
 *
 * @note 依赖 CategoryID::isOutflowFormat() 静态方法
 */
bool Bill::isOutflowEvent(bool isRefund, quint16 categoryId) {
    return CategoryID::isOutflowFormat(categoryId) && !isRefund;
}

/**
 * @brief 静态判断是否为收入事件
 *
 * 根据分类ID格式和退款标志判断是否为收入事件
 * 收入事件条件：分类ID为收入格式 或 是退款
 *
 * @param isRefund 是否退款标志
 * @param categoryId 分类ID
 * @return true 是收入事件，false 不是收入事件
 *
 * @note 依赖 CategoryID::isInflowFormat() 静态方法
 */
bool Bill::isInflowEvent(bool isRefund, quint16 categoryId) {
    return CategoryID::isInflowFormat(categoryId) || isRefund;
}

/**
 * @brief 静态判断是否为扩展事件
 *
 * 根据分类ID格式和退款标志判断是否为扩展事件
 * 扩展事件条件：分类ID为扩展格式 且 非退款
 *
 * @param isRefund 是否退款标志
 * @param categoryId 分类ID
 * @return true 是扩展事件，false 不是扩展事件
 *
 * @note 依赖 CategoryID::isExtendFormat() 静态方法
 */
bool Bill::isExtendEvent(bool isRefund, quint16 categoryId) {
    return CategoryID::isExtendFormat(categoryId) && !isRefund;
}

/**
 * @brief 静态判断是否为收入事件（纯收入）
 *
 * 根据分类ID格式和退款标志判断是否为纯收入事件
 * 纯收入事件条件：分类ID为收入格式 且 非退款
 *
 * @param isRefund 是否退款标志
 * @param categoryId 分类ID
 * @return true 是收入事件，false 不是收入事件
 *
 * @note 依赖 CategoryID::isIncomeFormat() 静态方法
 */
bool Bill::isIncomeEvent(bool isRefund, quint16 categoryId) {
    return CategoryID::isIncomeFormat(categoryId) && !isRefund;
}

// ==========================================
// 实例判断方法实现
// ==========================================

/**
 * @brief 判断当前账单是否为扩展退款
 *
 * 基于当前实例的退款标志和分类ID判断是否为扩展退款
 * 扩展退款条件：是退款 且 分类ID为扩展格式
 *
 * @return true 是扩展退款，false 不是扩展退款
 *
 * @note 依赖 CategoryID::isExtendFormat() 静态方法
 */
bool Bill::isExtendRefund() const {
    return m_isRefund && CategoryID::isExtendFormat(m_categoryId);
}

/**
 * @brief 判断当前账单是否为支出冲正
 *
 * 基于当前实例的退款标志和分类ID判断是否为支出冲正
 * 支出冲正条件：是退款 且 分类ID为纯流水格式 且 分类ID为支出格式
 *
 * @return true 是支出冲正，false 不是支出冲正
 *
 * @note 依赖 CategoryID::isFlowOnlyFormat() 和 isOutflowFormat() 静态方法
 */
bool Bill::isOutflowReversal() const {
    return m_isRefund && CategoryID::isFlowOnlyFormat(m_categoryId) && CategoryID::isOutflowFormat(m_categoryId);
}

/**
 * @brief 判断当前账单是否为支出
 *
 * 基于当前实例的退款标志和分类ID判断是否为支出
 * 支出条件：分类ID为支出格式 且 非退款
 *
 * @return true 是支出，false 不是支出
 *
 * @note 依赖 CategoryID::isOutflowFormat() 静态方法
 */
bool Bill::isOutflow() const {
    return CategoryID::isOutflowFormat(m_categoryId) && !m_isRefund;
}

/**
 * @brief 判断当前账单是否为收入
 *
 * 基于当前实例的退款标志和分类ID判断是否为收入
 * 收入条件：分类ID为收入格式 或 是扩展退款
 *
 * @return true 是收入，false 不是收入
 *
 * @note 依赖 CategoryID::isInflowFormat() 静态方法和 isExtendRefund() 实例方法
 */
bool Bill::isInflow() const {
    return CategoryID::isInflowFormat(m_categoryId) || isExtendRefund();
}

/**
 * @brief 判断当前账单是否为扩展
 *
 * 基于当前实例的退款标志和分类ID判断是否为扩展
 * 扩展条件：分类ID为扩展格式 且 非退款
 *
 * @return true 是扩展，false 不是扩展
 *
 * @note 依赖 CategoryID::isExtendFormat() 静态方法
 */
bool Bill::isExtend() const {
    return CategoryID::isExtendFormat(m_categoryId) && !m_isRefund;
}

/**
 * @brief 判断当前账单是否为纯收入
 *
 * 基于当前实例的退款标志和分类ID判断是否为纯收入
 * 纯收入条件：分类ID为收入格式 且 非退款
 *
 * @return true 是纯收入，false 不是纯收入
 *
 * @note 依赖 CategoryID::isIncomeFormat() 静态方法
 */
bool Bill::isIncome() const {
    return CategoryID::isIncomeFormat(m_categoryId) && !m_isRefund;
}

// ==========================================
// 操作符重载实现
// ==========================================

/**
 * @brief 相等运算符重载
 *
 * 比较两个 Bill 对象的所有成员变量是否完全相等
 * 用于判断两个账单记录是否相同
 *
 * @param other 要比较的另一个 Bill 对象
 * @return true 两个对象完全相等，false 存在差异
 *
 * @note 比较所有成员变量，包括 ID、日期、退款标志、分类ID、科目ID、对方ID、金额、备注
 */
bool Bill::operator==(const Bill& other) const {
    return m_id == other.m_id &&           // 比较账单ID
           m_dayNumber == other.m_dayNumber && // 比较日期编号
           m_isRefund == other.m_isRefund &&   // 比较退款标志
           m_categoryId == other.m_categoryId && // 比较分类ID
           m_subjectId == other.m_subjectId &&   // 比较科目ID
           m_counterpartId == other.m_counterpartId && // 比较对方ID
           m_amount == other.m_amount &&       // 比较金额
           m_remarks == other.m_remarks;       // 比较备注
}

/**
 * @brief 不等运算符重载
 *
 * 基于相等运算符的结果取反
 * 用于判断两个账单记录是否不同
 *
 * @param other 要比较的另一个 Bill 对象
 * @return true 两个对象不相等，false 完全相等
 *
 * @note 直接调用 operator== 并取反，避免重复实现比较逻辑
 */
bool Bill::operator!=(const Bill& other) const {
    return !(*this == other);
}