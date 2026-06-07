/**
 * @file AccountStatusOptions.h
 * @brief 账户状态枚举定义（Models 层）
 *
 * 本文件定义了账户可能的状态，用于标识账户的当前可用性。
 * 属于 Models 层，不依赖任何其他模块。
 *
 * @author tys
 * @date 2026-06-08
 */

#ifndef MODELS_ACCOUNTSTATUSOPTIONS_H
#define MODELS_ACCOUNTSTATUSOPTIONS_H

/**
 * @enum AccountStatusOptions
 * @brief 账户状态枚举
 *
 * 使用 enum class 保证强类型作用域，避免命名污染。
 * 该枚举用于标识账户的当前状态，影响账户的可用操作。
 *
 * @note 状态变更需通过 Service 层处理，并记录操作日志。
 * @note 逻辑删除（Deleted）的账户不可恢复，需管理员审批。
 */
enum class AccountStatusOptions {
    Active,   ///< 正常状态，账户可正常使用
    Inactive, ///< 未激活状态，需用户激活后才能使用
    Locked,   ///< 锁定状态，因安全原因（如密码错误次数过多）暂时禁用
    Frozen,   ///< 冻结状态，因法律或管理原因暂停所有操作
    Deleted   ///< 逻辑删除状态，账户不可见且不可恢复
};

#endif // MODELS_ACCOUNTSTATUSOPTIONS_H