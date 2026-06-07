/**
 * @file accountservice.cpp
 * @brief 账户业务服务实现文件
 * 
 * 所属分层：Services（业务服务层）
 * 依赖关系：Data 层（AccountBalance, Bill）、Base 层（Validator, OperationResult）
 * 
 * @author tys
 * @date 2026-06-08
 */

#include "services/accountservice.h"

/**
 * @brief 应用账单事件更新账户余额
 * 
 * 核心业务方法：根据账单的收支类型（支出/收入/退款）更新账户余额。
 * 使用 Validator 进行参数校验，确保业务数据合法性。
 * 采用不可变值对象模式，通过构造新 AccountBalance 实现更新。
 * 
 * @param acc [in/out] 待更新的账户余额对象（通过引用修改）
 * @param evt [in] 触发余额变更的账单事件
 * @return OperationResult<void> 业务操作结果
 *         - 成功：返回 OperationResult<void>::success()
 *         - 失败：返回 OperationResult<void>::failure() 并携带错误信息
 * 
 * @note 金额单位统一为分（int），禁止使用 double
 * @note 账单金额必须为非负数（允许为 0，但建议业务上避免 0 金额账单）
 */
OperationResult<void> AccountService::apply(AccountBalance& acc, const Bill& evt) {
    // 1. 使用基础工具层的 Validator 进行入参校验
    Validator<Bill> validator;
    validator.addRule([](const Bill& b) {
        // 假设业务规则：账单金额不能为负数 (MoneyRecord 重载了 operator>)
        return b.amount() > -1; 
    }, "账单更新失败：账单金额不能为负数");

    ValidationResult valResult = validator.validate(evt);
    if (!valResult.isValid()) {
        // 校验失败，中断操作并返回错误信息
        return OperationResult<void>::failure(valResult.errorMessage());
    }

    // 2. 核心业务逻辑：根据账单的收支类型更新余额
    // 由于 AccountBalance 是不可变的值模型，我们通过构造新对象并赋值，等效于 C# 的 record with 语法
    if (evt.isOutflow()) {
        // 支出：余额减少
        acc = AccountBalance(acc.balance() - evt.amount());
    } else if (evt.isInflow() || evt.isRefund()) {
        // 收入或退款：余额增加
        acc = AccountBalance(acc.balance() + evt.amount());
    }

    // 3. 返回成功状态
    return OperationResult<void>::success();
}