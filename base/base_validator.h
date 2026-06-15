/**
 * @file base_validator.h
 * @brief 基础验证器模块，提供统一的验证结果类型和泛型验证器模板
 *
 * 所属分层：Base（基础层）
 * 功能说明：
 *   - ValidationResult：封装验证操作的成功/失败状态及错误信息
 *   - Validator<T>：泛型验证器，支持链式添加规则并按顺序执行验证
 *   - Validator<void>：针对无参数场景的模板特化版本
 *
 * 依赖关系：仅依赖 Qt 核心库（QString, QList, functional）
 * 被依赖关系：被 Models、Data、Services、Pages 等上层模块使用
 *
 * @author tys
 * @date 2026-06-08
 */

#ifndef BASE_VALIDATOR_H
#define BASE_VALIDATOR_H

#include <QString>
#include <QList>
#include <functional>

/**
 * @brief 验证结果类，封装验证操作的结果状态和错误信息
 *
 * 设计目的：提供统一的验证结果返回类型，避免使用 bool 或原始字符串。
 * 使用方式：通过静态工厂方法 success() 和 failure() 创建实例。
 */
class ValidationResult {
public:
    /**
     * @brief 判断验证是否通过
     * @return true 表示验证通过，false 表示验证失败
     */
    bool isValid() const { return m_isValid; }

    /**
     * @brief 获取验证失败时的错误信息
     * @return 错误描述字符串（验证通过时返回空字符串）
     */
    QString errorMessage() const { return m_errorMessage; }

    /**
     * @brief 创建验证成功的实例
     * @return 验证通过的结果对象（m_isValid=true，错误信息为空）
     */
    static ValidationResult success() {
        return ValidationResult(true, QString());
    }

    /**
     * @brief 创建验证失败的实例
     * @param errorMessage 错误描述信息
     * @return 验证失败的结果对象（m_isValid=false，携带指定错误信息）
     */
    static ValidationResult failure(const QString& errorMessage) {
        return ValidationResult(false, errorMessage);
    }

private:
    /**
     * @brief 私有构造函数，强制通过静态工厂方法创建实例
     * @param isValid 验证是否通过
     * @param errorMessage 错误描述信息
     */
    ValidationResult(bool isValid, const QString& errorMessage)
        : m_isValid(isValid), m_errorMessage(errorMessage) {}

    bool m_isValid;         /**< 验证是否通过 */
    QString m_errorMessage; /**< 验证失败时的错误信息 */
};

/**
 * @brief 泛型验证器模板类，用于对指定类型 T 的实例进行规则校验
 *
 * 设计目的：提供可复用的验证框架，支持链式添加验证规则，
 *          并按顺序执行短路求值验证（遇到第一个失败规则立即返回）。
 *
 * @tparam T 待验证对象的类型
 */
template <typename T = void>
class Validator {
private:
    /**
     * @brief 验证规则结构体，包含验证条件和对应的错误信息
     */
    struct ValidationRule {
        std::function<bool(const T&)> condition; /**< 验证条件函数，返回 true 表示通过 */
        QString errorMessage;                    /**< 条件不满足时的错误信息 */
    };

    QList<ValidationRule> m_rules; /**< 存储所有验证规则的列表 */

public:
    /**
     * @brief 添加一条验证规则
     * @param condition 验证条件函数，接收 const T& 参数，返回 bool
     * @param errorMessage 条件不满足时的错误描述
     * @return Validator<T>& 返回自身引用，支持链式调用
     *
     * 使用示例：
     * @code
     * Validator<int> v;
     * v.addRule([](int x) { return x > 0; }, "必须为正数")
     *  .addRule([](int x) { return x < 100; }, "必须小于100");
     * @endcode
     */
    Validator<T>& addRule(std::function<bool(const T&)> condition, const QString& errorMessage) {
        m_rules.append({condition, errorMessage}); // 将规则追加到列表末尾
        return *this; // 返回自身实现链式调用
    }

    /**
     * @brief 对指定实例执行验证
     * @param instance 待验证的实例对象
     * @return ValidationResult 验证结果
     *
     * 验证逻辑：按规则添加顺序依次执行，遇到第一个失败规则立即返回失败结果；
     *           所有规则通过则返回成功结果。
     */
    ValidationResult validate(const T& instance) const {
        for (const auto& rule : m_rules) {
            if (!rule.condition(instance)) { // 条件不满足
                return ValidationResult::failure(rule.errorMessage); // 立即返回失败
            }
        }
        return ValidationResult::success(); // 所有规则通过
    }
};

/**
 * @brief Validator<void> 模板特化，针对无参数场景的验证器
 *
 * 设计目的：用于验证全局状态或无需输入参数的场景（如系统配置检查），
 *          保持与泛型版本一致的接口风格。
 */
template <>
class Validator<void> {
private:
    /**
     * @brief 验证规则结构体（特化版本，condition 无参数）
     */
    struct ValidationRule {
        std::function<bool()> condition; /**< 验证条件函数，无参数，返回 bool */
        QString errorMessage;            /**< 条件不满足时的错误信息 */
    };

    QList<ValidationRule> m_rules; /**< 存储所有验证规则的列表 */

public:
    /**
     * @brief 添加一条验证规则（无参数版本）
     * @param condition 验证条件函数，无参数，返回 bool
     * @param errorMessage 条件不满足时的错误描述
     * @return Validator<void>& 返回自身引用，支持链式调用
     */
    Validator<void>& addRule(std::function<bool()> condition, const QString& errorMessage) {
        m_rules.append({condition, errorMessage}); // 将规则追加到列表末尾
        return *this; // 返回自身实现链式调用
    }

    /**
     * @brief 执行无参数验证
     * @return ValidationResult 验证结果
     *
     * 验证逻辑：按规则添加顺序依次执行，遇到第一个失败规则立即返回失败结果；
     *           所有规则通过则返回成功结果。
     */
    ValidationResult validate() const {
        for (const auto& rule : m_rules) {
            if (!rule.condition()) { // 条件不满足
                return ValidationResult::failure(rule.errorMessage); // 立即返回失败
            }
        }
        return ValidationResult::success(); // 所有规则通过
    }
};

#endif // BASE_VALIDATOR_H