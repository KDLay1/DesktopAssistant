/**
 * @file services/accountservice.h
 * @brief 账户业务服务层头文件
 *
 * 本文件定义了 AccountService 类，提供账户余额相关的业务操作。
 * 属于 Services 层，依赖 Models 层和 Base 层。
 *
 * @author tys
 * @date 2026-06-08
 */

#ifndef SERVICES_ACCOUNTSERVICE_H
#define SERVICES_ACCOUNTSERVICE_H

#include "base/base_operationresult.h"  // 基础操作结果类型
#include "base/base_validator.h"        // 参数校验器
#include "models/AccountBalance.h"      // 账户余额模型
#include "models/AccountItem.h"         // 账户条目模型（依规范引入）
#include "models/bill.h"                // 账单事件模型

/**
 * @class AccountService
 * @brief 账户业务服务类
 *
 * 提供账户余额相关的业务操作，包括将账单事件应用到账户余额上。
 * 所有方法均为静态，无需实例化即可调用。
 *
 * @note 金额以 int（分）为单位存储，禁止使用 double。
 * @note 所有业务操作均通过 OperationResult 返回结果，错误时返回 failure()。
 */
class AccountService {
public:
    /**
     * @brief 将账单事件应用到账户余额上
     *
     * 根据账单事件的类型（收入/支出）和金额，更新账户余额对象。
     * 内部会使用 Validator 校验账单事件的合法性。
     *
     * @param acc 引用传递的账户余额对象，计算后原地更新余额
     * @param evt 触发更新的账单事件，包含金额、类型等信息
     * @return OperationResult<void> 成功返回 success()，校验失败返回 failure()
     *
     * @note 调用前需确保 evt 已通过 Validator 校验
     * @note 金额操作基于 int（分）单位，避免浮点精度问题
     */
    static OperationResult<void> apply(AccountBalance& acc, const Bill& evt);
};

#endif // SERVICES_ACCOUNTSERVICE_H