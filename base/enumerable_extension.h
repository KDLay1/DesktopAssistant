/**
 * @file enumerable_extension.h
 * @brief 基础工具层 - 集合扩展函数
 *
 * 本文件提供类似 C# LINQ 的集合操作扩展函数，包括：
 * - 升序检查（isAscending）
 * - 重复元素查找（findDuplicate）
 *
 * 依赖：标准库算法、Qt 容器（QList, QMap）
 * 分层：Base 层，无业务依赖
 *
 * @author tys
 * @date 2026-06-08
 */

#ifndef BASE_ENUMERABLE_EXTENSION_H
#define BASE_ENUMERABLE_EXTENSION_H

#include <algorithm>   // std::is_sorted, std::empty
#include <iterator>    // std::begin, std::end
#include <utility>     // std::declval
#include <QList>       // 返回类型容器
#include <QMap>        // 分组存储容器

namespace EnumerableExtension {

    /**
     * @brief 判断集合是否按指定键升序排列
     *
     * 使用 std::is_sorted 检查集合元素是否按 keySelector 提取的键严格升序。
     * 空集合视为已排序。
     *
     * @tparam Container 容器类型，需支持 std::begin/end 和 value_type
     * @tparam KeySelector 可调用对象类型，接受元素返回可比较键
     * @param sequence 待检查的集合
     * @param keySelector 键提取函数，例如 [](const Item& i) { return i.id; }
     * @return true 集合为空或按键升序排列
     * @return false 存在相邻元素键值不满足升序
     *
     * @note 使用严格弱序比较 (a < b)，不处理相等情况
     * @note 时间复杂度 O(n)，空间复杂度 O(1)
     */
    template <typename Container, typename KeySelector>
    inline bool isAscending(const Container& sequence, KeySelector keySelector) {
        // 空集合视为已排序
        if (std::empty(sequence)) return true;

        // std::is_sorted 默认检查元素是否符合严格弱序 (a < b)
        return std::is_sorted(std::begin(sequence), std::end(sequence),
                              [&keySelector](const auto& a, const auto& b) {
                                  return keySelector(a) < keySelector(b);
                              });
    }

    /**
     * @brief 查找集合中第一个出现重复键的元素分组
     *
     * 按 keySelector 提取的键对集合元素进行分组，返回第一个包含多个元素的分组。
     * 分组顺序保持键在集合中首次出现的顺序（保序特性）。
     *
     * @tparam Container 容器类型，需支持范围 for 循环和 value_type
     * @tparam KeySelector 可调用对象类型，接受元素返回可比较键
     * @param sequence 待检查的集合
     * @param keySelector 键提取函数，例如 [](const Item& i) { return i.categoryId; }
     * @return QList<ValueType> 第一个重复分组的所有元素，无重复时返回空列表
     *
     * @note 使用 QMap 实现分组，键需支持 operator<（QMap 要求）
     * @note 时间复杂度 O(n log n)，空间复杂度 O(n)
     * @warning 返回类型为 auto 推导，实际为 QList<Container::value_type>
     */
    template <typename Container, typename KeySelector>
    inline auto findDuplicate(const Container& sequence, KeySelector keySelector) {
        // 自动推导容器内的元素类型和 Selector 返回的 Key 类型
        using ValueType = typename Container::value_type;
        using KeyType = decltype(keySelector(std::declval<ValueType>()));

        QList<KeyType> order; // 记录键首次出现的顺序，以对齐 C# LINQ GroupBy 的保序特性
        QMap<KeyType, QList<ValueType>> groups; // 键到元素列表的映射

        // 遍历集合，按键分组
        for (const auto& item : sequence) {
            KeyType key = keySelector(item);
            if (!groups.contains(key)) {
                order.append(key); // 首次出现的键记录顺序
            }
            groups[key].append(item); // 将元素加入对应分组
        }

        // 按首次出现顺序检查，返回第一个重复分组
        for (const auto& key : std::as_const(order)) {
            if (groups[key].size() > 1) {
                return groups[key]; // 返回第一个重复分组的所有元素
            }
        }

        // 如果没有重复项，返回空列表
        return QList<ValueType>();
    }

} // namespace EnumerableExtension

#endif // BASE_ENUMERABLE_EXTENSION_H