/**
 * @file date_extension.h
 * @brief 日期扩展工具函数集合（Base 层）
 *
 * 本文件提供日期相关的静态工具函数，主要用于按周/月/年进行统计分组。
 * 所有函数均为内联实现，无运行时依赖。
 *
 * @author tys
 * @date 2026-06-08
 *
 * @note 本模块属于 Base 层，可被 Models/Data/Services/Pages 层使用
 * @note 日期参数建议先通过 Validator 校验有效性
 */

#ifndef BASE_DATE_EXTENSION_H
#define BASE_DATE_EXTENSION_H

#include <QDate>

namespace DateExtension {

    /**
     * @brief 获取日期的绝对周数（常用于按周统计账单时的分组 ID）
     *
     * 计算指定日期在给定起始星期设定下的绝对周数。
     * 以公元元年1月1日（C# DateOnly 纪元）为基准，按7天为一周期进行分组。
     *
     * @param date 待计算的日期（建议先通过 Validator 校验有效性）
     * @param firstDayOfWeek 每周起始日（默认周日），取值范围 Qt::Monday ~ Qt::Sunday
     * @return qint64 绝对周数（从1开始计数）
     *
     * @note 本实现与 ISO 8601 标准不同，起始周定义取决于 firstDayOfWeek 参数
     * @note 当 date 为无效日期时，返回值无意义
     */
    inline qint64 getWeekNumber(const QDate& date, Qt::DayOfWeek firstDayOfWeek = Qt::Sunday) {
        // 计算与公元元年1月1日（C# DateOnly 纪元）的相差天数
        qint64 dayNumber = date.toJulianDay() - QDate(1, 1, 1).toJulianDay();
        
        // 将 Qt::DayOfWeek 枚举映射为整数偏移量（周日=0，周一=1，...）
        int mappedFirstDay = (firstDayOfWeek == Qt::Sunday) ? 0 : static_cast<int>(firstDayOfWeek);
        
        // 计算绝对周数：偏移后按7天分组，+1 使周数从1开始
        return (dayNumber - mappedFirstDay + 1) / 7 + 1;
    }

    /**
     * @brief 获取所属周的第一天
     *
     * 根据指定的每周起始日，计算给定日期所在周的第一天日期。
     * 使用取模运算处理跨周边界的情况，兼容任意起始星期设定。
     *
     * @param date 目标日期（建议先通过 Validator 校验有效性）
     * @param firstDayOfWeek 每周起始日（默认周日），取值范围 Qt::Monday ~ Qt::Sunday
     * @return QDate 所在周的第一天日期
     *
     * @note 当 date 为无效日期时，返回值可能为无效 QDate
     */
    inline QDate getFirstDayOfTheSameWeek(const QDate& date, Qt::DayOfWeek firstDayOfWeek = Qt::Sunday) {
        // 巧妙利用取模运算计算日期偏移量，兼容任何起始星期的设定
        int difference = (7 + (date.dayOfWeek() - static_cast<int>(firstDayOfWeek))) % 7;
        return date.addDays(-difference);
    }

    /**
     * @brief 获取所属月的第一天
     *
     * 直接构造指定日期所在月份的第一天（即当月1日）。
     *
     * @param date 目标日期（建议先通过 Validator 校验有效性）
     * @return QDate 所在月的第一天日期
     *
     * @note 当 date 为无效日期时，返回值可能为无效 QDate
     */
    inline QDate getFirstDayOfTheSameMonth(const QDate& date) {
        return QDate(date.year(), date.month(), 1);
    }

    /**
     * @brief 获取所属年的第一天
     *
     * 直接构造指定日期所在年份的第一天（即1月1日）。
     *
     * @param date 目标日期（建议先通过 Validator 校验有效性）
     * @return QDate 所在年的第一天日期
     *
     * @note 当 date 为无效日期时，返回值可能为无效 QDate
     */
    inline QDate getFirstDayOfTheSameYear(const QDate& date) {
        return QDate(date.year(), 1, 1);
    }

} // namespace DateExtension

#endif // BASE_DATE_EXTENSION_H