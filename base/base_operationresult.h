/**
 * @file base_operationresult.h
 * @brief 基础层 - 操作结果封装类
 *
 * 本文件定义了 OperationResult 模板类及其 void 特化版本。
 * 该类用于统一封装业务操作的执行结果，包含成功/失败状态、错误信息及可选返回值。
 * 它是整个项目业务逻辑层（Services）的标准返回值类型，强制调用方处理操作结果，
 * 避免了异常或错误码的混乱使用。
 *
 * @author tys
 * @date 2023-10-27
 */

#ifndef BASE_OPERATIONRESULT_H
#define BASE_OPERATIONRESULT_H

#include <QString>

/**
 * @brief 泛型操作结果类
 *
 * 用于封装带有返回数据的操作结果。例如，查询操作成功后，可以返回查询到的数据对象。
 * 使用静态工厂方法 success() 和 failure() 创建实例，语义更清晰。
 *
 * @tparam T 操作成功时返回的数据类型
 */
template <typename T = void>
class OperationResult {
public:
    /**
     * @brief 构造函数：创建一个成功的结果
     * @param data 操作成功时返回的数据
     * @note 此构造函数未标记为 explicit，允许从 T 隐式转换。建议优先使用静态工厂方法 success()。
     */
    OperationResult(const T& data)
        : m_isSuccess(true), m_errorMessage(QString()), m_data(data) {}

    /**
     * @brief 构造函数：创建一个失败的结果
     * @param errorMessage 描述失败原因的字符串
     * @note 标记为 explicit，防止 QString 隐式转换为 OperationResult。
     */
    explicit OperationResult(const QString& errorMessage)
        : m_isSuccess(false), m_errorMessage(errorMessage), m_data() {}

    /**
     * @brief 判断操作是否成功
     * @return true 表示操作成功，false 表示操作失败
     */
    bool isSuccess() const { return m_isSuccess; }

    /**
     * @brief 获取操作失败时的错误信息
     * @return 错误描述字符串。如果操作成功，返回空字符串。
     */
    QString errorMessage() const { return m_errorMessage; }

    /**
     * @brief 获取操作成功时返回的数据
     * @return 返回存储的数据对象
     * @warning 调用此函数前，必须通过 isSuccess() 确认操作成功。
     *          如果操作失败，此函数将返回一个默认构造的 T 对象，其内容无效。
     */
    T data() const { return m_data; }

    /**
     * @brief 静态工厂方法：创建一个成功的结果
     * @param data 操作成功时返回的数据
     * @return 包含成功状态和数据的 OperationResult 对象
     */
    static OperationResult<T> success(const T& data) {
        return OperationResult<T>(data);
    }

    /**
     * @brief 静态工厂方法：创建一个失败的结果
     * @param errorMessage 描述失败原因的字符串
     * @return 包含失败状态和错误信息的 OperationResult 对象
     */
    static OperationResult<T> failure(const QString& errorMessage) {
        return OperationResult<T>(errorMessage);
    }

private:
    bool m_isSuccess;          /**< 操作是否成功 */
    QString m_errorMessage;    /**< 错误信息 */
    T m_data;                  /**< 成功时返回的数据 */
};

/**
 * @brief 特化版本：无返回数据的操作结果类
 *
 * 用于封装不需要返回数据的操作结果，例如保存、删除、更新等操作。
 * 移除了 data() 方法和 m_data 成员，使接口更简洁。
 */
template <>
class OperationResult<void> {
public:
    /**
     * @brief 默认构造函数：创建一个成功的结果
     */
    OperationResult()
        : m_isSuccess(true), m_errorMessage(QString()) {}

    /**
     * @brief 构造函数：创建一个失败的结果
     * @param errorMessage 描述失败原因的字符串
     * @note 标记为 explicit，防止 QString 隐式转换为 OperationResult。
     */
    explicit OperationResult(const QString& errorMessage)
        : m_isSuccess(false), m_errorMessage(errorMessage) {}

    /**
     * @brief 判断操作是否成功
     * @return true 表示操作成功，false 表示操作失败
     */
    bool isSuccess() const { return m_isSuccess; }

    /**
     * @brief 获取操作失败时的错误信息
     * @return 错误描述字符串。如果操作成功，返回空字符串。
     */
    QString errorMessage() const { return m_errorMessage; }

    /**
     * @brief 静态工厂方法：创建一个成功的结果
     * @return 包含成功状态的 OperationResult 对象
     */
    static OperationResult<void> success() {
        return OperationResult<void>();
    }

    /**
     * @brief 静态工厂方法：创建一个失败的结果
     * @param errorMessage 描述失败原因的字符串
     * @return 包含失败状态和错误信息的 OperationResult 对象
     */
    static OperationResult<void> failure(const QString& errorMessage) {
        return OperationResult<void>(errorMessage);
    }

private:
    bool m_isSuccess;          /**< 操作是否成功 */
    QString m_errorMessage;    /**< 错误信息 */
};

#endif // BASE_OPERATIONRESULT_H