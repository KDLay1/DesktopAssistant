/**
 * @file eventsummary.h
 * @brief 事件财务汇总模型
 *
 * 所属分层：Models（数据模型层）
 * 依赖关系：依赖 models/MoneyRecord.h（同层模型）
 * 功能描述：表示一个事件（如交易、活动）的财务汇总，包含出账、入账、退款、收入等分类汇总。
 *           所有金额以分为单位存储，使用 MoneyRecord 类型确保精度。
 *
 * @author tys
 * @date 2026-06-08
 */

#ifndef MODELS_EVENTSUMMARY_H
#define MODELS_EVENTSUMMARY_H

#include "models/MoneyRecord.h"

/**
 * @class EventSummary
 * @brief 事件财务汇总类
 *
 * 设计目的：聚合一个事件中所有财务相关的记录，提供分类汇总和动态计算属性。
 * 职责：
 *   - 存储出账、入账、退款、收入等分类汇总
 *   - 提供净出账（outflow）和净支出（extend）的动态计算
 *   - 支持相等性比较
 *
 * @note 所有金额以分为单位，使用 MoneyRecord 类型存储，禁止直接使用 double。
 *       业务层应使用 Validator 校验参数合理性，本类仅作为数据容器。
 */
class EventSummary {
public:
    /**
     * @brief 构造函数，提供默认值
     *
     * 初始化所有财务汇总字段，默认值为零金额的 MoneyRecord。
     *
     * @param outflowWithReversal 包含撤销的出账总额（默认：0）
     * @param outflowReversal     出账撤销总额（默认：0）
     * @param inflow              入账总额（默认：0）
     * @param extendRefund        支出退款总额（默认：0）
     * @param extendWithRefund    包含退款的支出总额（默认：0）
     * @param income              纯收入总额（默认：0）
     *
     * @note 构造函数不进行参数校验，业务层应确保传入合理的值。
     *       例如：outflowWithReversal 应 >= outflowReversal。
     */
    EventSummary(MoneyRecord outflowWithReversal = MoneyRecord(0),
                 MoneyRecord outflowReversal = MoneyRecord(0),
                 MoneyRecord inflow = MoneyRecord(0),
                 MoneyRecord extendRefund = MoneyRecord(0),
                 MoneyRecord extendWithRefund = MoneyRecord(0),
                 MoneyRecord income = MoneyRecord(0))
        : m_outflowWithReversal(outflowWithReversal),
          m_outflowReversal(outflowReversal),
          m_inflow(inflow),
          m_extendRefund(extendRefund),
          m_extendWithRefund(extendWithRefund),
          m_income(income) {}

    // ==========================================
    // 基础字段 Getters
    // ==========================================

    /**
     * @brief 获取包含撤销的出账总额
     * @return MoneyRecord 包含撤销的出账总额（以分为单位）
     */
    MoneyRecord outflowWithReversal() const { return m_outflowWithReversal; }

    /**
     * @brief 获取出账撤销总额
     * @return MoneyRecord 出账撤销总额（以分为单位）
     */
    MoneyRecord outflowReversal() const { return m_outflowReversal; }

    /**
     * @brief 获取入账总额
     * @return MoneyRecord 入账总额（以分为单位）
     */
    MoneyRecord inflow() const { return m_inflow; }

    /**
     * @brief 获取支出退款总额
     * @return MoneyRecord 支出退款总额（以分为单位）
     */
    MoneyRecord extendRefund() const { return m_extendRefund; }

    /**
     * @brief 获取包含退款的支出总额
     * @return MoneyRecord 包含退款的支出总额（以分为单位）
     */
    MoneyRecord extendWithRefund() const { return m_extendWithRefund; }

    /**
     * @brief 获取纯收入总额
     * @return MoneyRecord 纯收入总额（以分为单位）
     */
    MoneyRecord income() const { return m_income; }

    // ==========================================
    // 动态计算属性
    // ==========================================

    /**
     * @brief 计算净出账金额
     *
     * 净出账 = 包含撤销的出账总额 - 出账撤销总额
     * 结果可能为负值（当撤销大于出账时），业务层应自行处理。
     *
     * @return MoneyRecord 净出账金额（以分为单位）
     */
    MoneyRecord outflow() const { 
        return m_outflowWithReversal - m_outflowReversal; 
    }
    
    /**
     * @brief 计算净支出金额
     *
     * 净支出 = 包含退款的支出总额 - 支出退款总额
     * 结果可能为负值（当退款大于支出时），业务层应自行处理。
     *
     * @return MoneyRecord 净支出金额（以分为单位）
     */
    MoneyRecord extend() const { 
        return m_extendWithRefund - m_extendRefund; 
    }

    // ==========================================
    // 操作符重载
    // ==========================================

    /**
     * @brief 相等比较运算符
     *
     * 比较两个 EventSummary 对象的所有字段是否完全相等。
     *
     * @param other 另一个 EventSummary 对象
     * @return true 如果所有字段都相等，否则返回 false
     */
    bool operator==(const EventSummary& other) const {
        return m_outflowWithReversal == other.m_outflowWithReversal &&
               m_outflowReversal == other.m_outflowReversal &&
               m_inflow == other.m_inflow &&
               m_extendRefund == other.m_extendRefund &&
               m_extendWithRefund == other.m_extendWithRefund &&
               m_income == other.m_income;
    }

    /**
     * @brief 不等比较运算符
     *
     * 基于 operator== 的结果取反。
     *
     * @param other 另一个 EventSummary 对象
     * @return true 如果任意字段不相等，否则返回 false
     */
    bool operator!=(const EventSummary& other) const {
        return !(*this == other);
    }

private:
    MoneyRecord m_outflowWithReversal; /**< 包含撤销的出账总额（以分为单位） */
    MoneyRecord m_outflowReversal;     /**< 出账撤销总额（以分为单位） */
    MoneyRecord m_inflow;              /**< 入账总额（以分为单位） */
    MoneyRecord m_extendRefund;        /**< 支出退款总额（以分为单位） */
    MoneyRecord m_extendWithRefund;    /**< 包含退款的支出总额（以分为单位） */
    MoneyRecord m_income;              /**< 纯收入总额（以分为单位） */
};

#endif // MODELS_EVENTSUMMARY_H