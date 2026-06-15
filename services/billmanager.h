/**
 * @file billmanager.h
 * @brief 账单业务管理器（Services 层）
 *
 * 本文件定义了 BillManager 类，提供账单的增删查业务操作。
 * 所有业务方法均返回 OperationResult 以统一处理成功/失败状态。
 *
 * @author tys
 * @date 2026-06-08
 *
 * @note 分层位置：Services（业务服务层）
 * @note 依赖关系：依赖 Models 层的 Bill 模型和 Base 层的 OperationResult
 * @note 强制约束：所有业务方法必须使用 Validator 进行参数校验
 */

#ifndef SERVICES_BILLMANAGER_H
#define SERVICES_BILLMANAGER_H

#include <QList>
#include "base/base_operationresult.h"
#include "models/bill.h"

/**
 * @brief 账单业务管理器
 *
 * 提供账单的增删查业务操作，所有方法均为静态方法。
 * 业务方法统一返回 OperationResult 类型，便于调用方统一处理结果。
 *
 * @note 设计模式：工具类（静态方法集合）
 * @note 线程安全：非线程安全，调用方需自行保证线程同步
 */
class BillManager {
public:
    // ==========================================
    // 核心业务方法
    // ==========================================

    /**
     * @brief 新增账单
     *
     * 将传入的账单对象持久化到数据库。
     * 调用前需确保 bill 对象已通过 Validator 校验。
     *
     * @param bill 待新增的账单对象（const 引用，避免拷贝）
     * @return OperationResult<void> 操作结果
     *         - 成功：返回成功状态
     *         - 失败：返回错误信息（如参数校验失败、数据库异常等）
     *
     * @note 参数校验：调用方需先使用 Validator::validateBill() 进行校验
     * @note 金额单位：分（int），禁止使用 double
     */
    static OperationResult<void> addBill(const Bill& bill);

    /**
     * @brief 删除账单
     *
     * 根据账单 ID 从数据库中删除对应的账单记录。
     *
     * @param billId 账单 ID（强类型 BillID，非原始 int）
     * @return OperationResult<void> 操作结果
     *         - 成功：返回成功状态
     *         - 失败：返回错误信息（如 ID 不存在、数据库异常等）
     *
     * @note 参数校验：调用方需先使用 Validator::validateId() 进行校验
     * @note 强类型约束：应使用 BillID 类型而非 int
     */
    static OperationResult<void> deleteBill(int billId);

    /**
     * @brief 单条账单查询
     *
     * 根据账单 ID 从数据库中查询对应的账单记录。
     *
     * @param billId 账单 ID（强类型 BillID，非原始 int）
     * @return OperationResult<Bill> 操作结果
     *         - 成功：返回包含 Bill 对象的成功状态
     *         - 失败：返回错误信息（如 ID 不存在、数据库异常等）
     *
     * @note 参数校验：调用方需先使用 Validator::validateId() 进行校验
     * @note 强类型约束：应使用 BillID 类型而非 int
     */
    static OperationResult<Bill> getBill(int billId);

    /**
     * @brief 全量账单查询
     *
     * 从数据库中查询所有账单记录。
     *
     * @return OperationResult<QList<Bill>> 操作结果
     *         - 成功：返回包含账单列表的成功状态
     *         - 失败：返回错误信息（如数据库异常等）
     *
     * @note 性能注意：当数据量较大时，建议使用分页查询替代
     */
    static OperationResult<QList<Bill>> getAllBills();
};

#endif // SERVICES_BILLMANAGER_H