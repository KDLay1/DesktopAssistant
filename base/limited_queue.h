/**
 * @file limited_queue.h
 * @brief 限制最大容量的队列模板类
 *
 * 所属分层：Base（基础工具层）
 * 功能：提供固定容量上限的队列，当队列满时自动丢弃最旧元素（FIFO 淘汰策略）
 * 依赖：Qt QQueue（QtCore 模块）
 * 被依赖：Models、Data、Services、Pages 层均可使用
 *
 * @author tys
 * @date 2026-06-08
 */

#ifndef BASE_LIMITED_QUEUE_H
#define BASE_LIMITED_QUEUE_H

#include <QQueue>

namespace Base {

    /**
     * @brief 限制最大容量的队列
     *
     * 继承自 QQueue<T>，重写 enqueue 方法实现自动淘汰逻辑。
     * 当队列达到设定的最大容量时，新元素的入队会自动将队首（最老）的元素挤出。
     * 容量为 0 时，拒绝任何元素入队（队列始终为空）。
     *
     * @tparam T 队列元素类型
     */
    template <typename T>
    class LimitedQueue : public QQueue<T> {
    public:
        /**
         * @brief 构造函数，初始化容量上限
         * @param limit 容量上限（允许为 0，表示禁止入队）
         */
        explicit LimitedQueue(int limit) : m_limit(limit) {}

        /**
         * @brief 入队操作（重写基类方法）
         *
         * 当队列大小达到或超过容量上限时，自动移除队首元素以腾出空间。
         * 若容量为 0，则直接丢弃新元素（不执行任何操作）。
         *
         * @param item 待入队的元素
         * @note 调用方无法感知元素是否被丢弃（无返回值），如需反馈请使用 OperationResult
         */
        void enqueue(const T& item) {
            // 当达到或超过限制时，循环移除队首元素，直到腾出空间
            // 增加 isEmpty 校验以防止 limit 被设置为 <= 0 时导致的死循环或越界崩溃
            while (this->size() >= m_limit && !this->isEmpty()) {
                this->dequeue();  // 移除最旧元素
            }

            // 安全限制：如果 limit 为 0，则拒绝任何元素入队
            if (m_limit > 0) {
                QQueue<T>::enqueue(item);  // 调用基类实际入队
            }
        }

        /**
         * @brief 获取当前容量上限
         * @return 容量上限值（可能为 0）
         */
        int limit() const { return m_limit; }

        /**
         * @brief 动态调整容量上限
         *
         * 调整后，若当前元素数量超出新容量，则立即移除多余的最旧元素。
         *
         * @param limit 新的容量上限（允许为 0，表示清空队列并禁止后续入队）
         * @note 若传入负数，会导致 size() > m_limit 恒成立，从而清空队列（非预期行为）
         */
        void setLimit(int limit) {
            m_limit = limit;
            // 调整容量时，如果当前元素已经超出新容量，立即裁切
            while (this->size() > m_limit && !this->isEmpty()) {
                this->dequeue();  // 移除最旧元素直到符合新容量
            }
        }

    private:
        int m_limit;  ///< 容量上限（0 表示禁止入队）
    };

} // namespace Base

#endif // BASE_LIMITED_QUEUE_H