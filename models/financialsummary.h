/**
 * @file financialsummary.h
 * @brief 财务汇总模型
 * 
 * 所属分层：Models（模型层）
 * 功能：表示某个时间段内的财务统计摘要，包含出账、入账、退款、收入等汇总数据，
 *       并提供动态计算属性（如净流出、净支出、净现金流、净收入）。
 * 
 * @note 金额已使用 int（分）存储，符合项目规范。
 * 
 * @author tys
 * @date 2026-06-08
 */

#ifndef MODELS_FINANCIALSUMMARY_H
#define MODELS_FINANCIALSUMMARY_H

#include <QDate>
#include "models/MoneyRecord.h"
#include "models/eventsummary.h"

/**
 * @brief 财务汇总类
 * 
 * 设计目的：用于汇总和计算指定时间段内的财务统计数据。
 * 职责：存储时间段内的各项财务汇总数据，并提供动态计算属性。
 * 
 * @note 所有金额均以 int（分）为单位存储和计算。
 */
class FinancialSummary {
public:
    // ==========================================
    // 构造函数
    // ==========================================
    
    /**
     * @brief 全参/默认构造函数
     * 
     * @param start 统计开始日期（默认空日期）
     * @param end 统计结束日期（默认空日期）
     * @param outflowWithReversal 包含撤销的出账总额（分）
     * @param outflowReversal 撤销总额（分）
     * @param inflow 入账总额（分）
     * @param refund 退款总额（分）
     * @param extendWithRefund 包含退款的支出总额（分）
     * @param income 收入总额（分）
     */
    FinancialSummary(QDate start = QDate(), 
                     QDate end = QDate(),
                     int outflowWithReversal = 0,
                     int outflowReversal = 0,
                     int inflow = 0,
                     int refund = 0,
                     int extendWithRefund = 0,
                     int income = 0)
        : m_start(start),
          m_end(end),
          m_outflowWithReversal(outflowWithReversal),
          m_outflowReversal(outflowReversal),
          m_inflow(inflow),
          m_refund(refund),
          m_extendWithRefund(extendWithRefund),
          m_income(income) {}

    /**
     * @brief 从 EventSummary 转换构造
     * 
     * 将 EventSummary 中的 MoneyRecord 金额转换为 int（分）存储。
     * 
     * @param start 统计开始日期
     * @param end 统计结束日期
     * @param fm 事件汇总对象
     */
    FinancialSummary(const QDate& start, const QDate& end, const EventSummary& fm)
        : m_start(start),
          m_end(end),
          m_outflowWithReversal(static_cast<int>(fm.outflowWithReversal())), // ✅ 直接取分
          m_outflowReversal(static_cast<int>(fm.outflowReversal())),
          m_inflow(static_cast<int>(fm.inflow())),
          m_refund(static_cast<int>(fm.extendRefund())),
          m_extendWithRefund(static_cast<int>(fm.extendWithRefund())),
          m_income(static_cast<int>(fm.income())) {}

    // ==========================================
    // 基础字段 Getters
    // ==========================================
    
    QDate start() const { return m_start; }
    QDate end() const { return m_end; }
    int outflowWithReversal() const { return m_outflowWithReversal; }
    int outflowReversal() const { return m_outflowReversal; }
    int inflow() const { return m_inflow; }
    int refund() const { return m_refund; }
    int extendWithRefund() const { return m_extendWithRefund; }
    int income() const { return m_income; }

    // ==========================================
    // 动态计算属性（单位：分）
    // ==========================================
    
    int outflow() const { return m_outflowWithReversal - m_outflowReversal; }
    int extend() const { return m_extendWithRefund - m_refund; }
    int netFlow() const { return m_inflow - m_outflowWithReversal + m_outflowReversal; }
    int netIncome() const { return m_income - m_extendWithRefund + m_refund; }

    // ==========================================
    // 操作符重载
    // ==========================================
    
    bool operator==(const FinancialSummary& other) const {
        return m_start == other.m_start &&
               m_end == other.m_end &&
               m_outflowWithReversal == other.m_outflowWithReversal &&
               m_outflowReversal == other.m_outflowReversal &&
               m_inflow == other.m_inflow &&
               m_refund == other.m_refund &&
               m_extendWithRefund == other.m_extendWithRefund &&
               m_income == other.m_income;
    }

    bool operator!=(const FinancialSummary& other) const {
        return !(*this == other);
    }

private:
    QDate m_start;
    QDate m_end;
    int m_outflowWithReversal;  // 单位：分
    int m_outflowReversal;      // 单位：分
    int m_inflow;               // 单位：分
    int m_refund;               // 单位：分
    int m_extendWithRefund;     // 单位：分
    int m_income;               // 单位：分
};

#endif // MODELS_FINANCIALSUMMARY_H