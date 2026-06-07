/**
 * @file MoneyRecord.h
 * @brief 金额数据模型（模型层）
 *
 * 本文件定义了 MoneyRecord 类，用于封装金额数据。
 * 金额以“分”为单位存储（int），避免浮点精度丢失。
 * 提供与 double 的互转、算术运算、比较、字符串解析等功能。
 *
 * @author tys
 * @date 2026-06-08
 *
 * @note 所属分层：Models（模型层）
 * @note 依赖关系：仅依赖 Qt 基础库（QString）
 * @note 被依赖关系：可被 Data、Services、Pages 层使用
 */

#ifndef MODELS_MONEYRECORD_H
#define MODELS_MONEYRECORD_H

#include <cmath>    // 用于 std::round 四舍五入
#include <QString>  // 用于字符串操作

/**
 * @brief 金额记录类
 *
 * 封装金额数据，以“分”为单位存储（int），
 * 提供与 double 的互转、算术运算、比较、字符串解析等功能。
 * 设计目的是避免浮点精度丢失，确保金额计算的准确性。
 *
 * @note 金额存储单位：分（1元 = 100分）
 * @note 禁止直接使用 double 存储金额
 */
class MoneyRecord {
public:
    /**
     * @brief 默认构造函数（以分为单位）
     * @param record 金额值（单位：分），默认值为 0
     *
     * 直接存储以分为单位的整数值。
     * 适用于从数据库或业务逻辑中直接获取的整型金额。
     */
    MoneyRecord(int record = 0) : m_record(record) {}

    /**
     * @brief 带参构造函数（以元为单位）
     * @param real 金额值（单位：元），如 12.34
     *
     * 将 double 类型的金额四舍五入转换为分存储。
     * 使用 std::round 确保精度，避免浮点误差累积。
     *
     * @warning 未校验负数或精度超过2位小数的情况
     */
    MoneyRecord(double real)
        : m_record(static_cast<int>(std::round(real * 100)))
    {}

    /**
     * @brief 获取金额（以元为单位）
     * @return double 金额值（单位：元）
     *
     * 将内部存储的分转换为元返回。
     * 注意：浮点除法可能产生微小精度误差。
     */
    double view() const { return m_record / 100.0; }

    /**
     * @brief 隐式转换为 int（以分为单位）
     * @return int 金额值（单位：分）
     *
     * 允许 MoneyRecord 隐式转换为 int。
     * @warning 隐式转换可能导致意外类型转换，建议谨慎使用
     */
    operator int() const { return m_record; }

    // 运算符重载

    /**
     * @brief 加法运算符
     * @param right 右侧加数
     * @return MoneyRecord 两个金额之和（单位：分）
     *
     * 直接对分进行加法运算，确保精度。
     */
    MoneyRecord operator+(const MoneyRecord& right) const { return MoneyRecord(m_record + right.m_record); }

    /**
     * @brief 减法运算符
     * @param right 右侧减数
     * @return MoneyRecord 两个金额之差（单位：分）
     *
     * 直接对分进行减法运算，确保精度。
     */
    MoneyRecord operator-(const MoneyRecord& right) const { return MoneyRecord(m_record - right.m_record); }

    /**
     * @brief 除法运算符（除以浮点数）
     * @param d 除数（double 类型）
     * @return double 除法结果（单位：元）
     *
     * 将分除以浮点数，返回 double 类型。
     * @warning 返回 double 可能丢失精度，建议返回 MoneyRecord
     */
    double operator/(double d) const { return m_record / d; }

    /**
     * @brief 大于比较运算符（与 int 比较）
     * @param i 比较值（单位：分）
     * @return bool 如果当前金额大于 i 则返回 true
     *
     * 直接对分进行比较。
     * @note 不支持与 MoneyRecord 直接比较
     */
    bool operator>(int i) const { return m_record > i; }

    /**
     * @brief 小于比较运算符（与 int 比较）
     * @param i 比较值（单位：分）
     * @return bool 如果当前金额小于 i 则返回 true
     *
     * 直接对分进行比较。
     * @note 不支持与 MoneyRecord 直接比较
     */
    bool operator<(int i) const { return m_record < i; }

    /**
     * @brief 相等比较运算符
     * @param other 另一个 MoneyRecord 对象
     * @return bool 如果两个金额相等则返回 true
     *
     * 直接对分进行比较，确保精度。
     */
    bool operator==(const MoneyRecord& other) const { return m_record == other.m_record; }

    /**
     * @brief 不等比较运算符
     * @param other 另一个 MoneyRecord 对象
     * @return bool 如果两个金额不相等则返回 true
     *
     * 直接对分进行比较，确保精度。
     */
    bool operator!=(const MoneyRecord& other) const { return m_record != other.m_record; }

    /**
     * @brief 转换为字符串（以分为单位）
     * @return QString 金额的字符串表示（单位：分）
     *
     * 返回以分为单位的整数字符串。
     * @warning 应返回“元”格式（如 "12.34"），当前返回分格式
     */
    QString toString() const { return QString::number(m_record); }

    /**
     * @brief 尝试从字符串解析金额（以分为单位）
     * @param str 待解析的字符串
     * @param res 输出参数，解析成功后的 MoneyRecord 对象
     * @return bool 解析成功返回 true，失败返回 false
     *
     * 尝试将字符串解析为整数分。
     * 解析失败时，res 被设置为 0。
     *
     * @note 只支持整数分字符串，不支持小数格式（如 "12.34"）
     * @warning 应使用 OperationResult 作为返回值
     */
    static bool tryParse(const QString& str, MoneyRecord& res) {
        bool ok;
        int i = str.toInt(&ok);  // 尝试将字符串转换为整数
        if (ok) {
            res = MoneyRecord(i);  // 解析成功，设置结果
            return true;
        }
        res = MoneyRecord(0);  // 解析失败，设置为 0
        return false;
    }

private:
    int m_record; /**< 内部以分为单位存储的金额值，防止浮点精度丢失 */
};

#endif // MODELS_MONEYRECORD_H