/**
 * @file billmanager.cpp
 * @brief 账单业务管理器实现文件
 * 
 * 所属分层：Services（业务服务层）
 * 功能描述：提供账单的增删查业务逻辑，包括参数校验、业务规则验证和数据持久化协调
 * 
 * @author tys
 * @date 2026-06-08
 */

#include "services/billmanager.h"
#include "base/base_validator.h"
#include "data/databasemanager.h" // 接入你现有的持久化层
#include "models/CategoryID.h"

// ==========================================
// 新增账单逻辑
// ==========================================
/**
 * @brief 新增账单业务方法
 * 
 * 执行完整的业务规则校验后，将合法账单持久化到数据库。
 * 校验规则包括：金额非负、分类格式合法、主体账户有效。
 * 
 * @param bill 待新增的账单对象（const引用，避免拷贝）
 * @return OperationResult<void> 业务操作结果
 *         - 成功：无数据返回
 *         - 失败：包含错误描述信息
 * 
 * @note 金额以分为单位存储，禁止使用double
 * @note 主体账户ID必须为有效值（>0），0代表未分配或非法
 */
OperationResult<void> BillManager::addBill(const Bill& bill) {
    // 1. 业务规则校验：组装 Validator 责任链
    Validator<Bill> validator;
    validator.addRule([](const Bill& b) {
        return b.amount() > -1; // 金额不能为负数（0表示免费交易）
    }, "账单录入失败：交易金额不能为负数")
    .addRule([](const Bill& b) {
        return CategoryID::isValidFormat(b.categoryId()); // 分类ID必须符合预定义格式
    }, "账单录入失败：交易分类格式非法")
    .addRule([](const Bill& b) {
        // 主体账户不能为0（0通常代表未分配或非法）
        return b.subjectId() > 0; // TODO: 应使用强类型 AccountID::isValid()
    }, "账单录入失败：必须指定有效的交易主体账户");

    // 执行校验
    ValidationResult valRes = validator.validate(bill);
    if (!valRes.isValid()) {
        return OperationResult<void>::failure(valRes.errorMessage());
    }

    // 2. 调用数据层进行持久化 (需根据你的 DatabaseManager 实际接口调整)
    // 这里假设 DatabaseManager 提供静态方法 addBill，返回 bool 代表是否写入成功
    bool dbResult = DatabaseManager::addBill(bill);
    
    if (dbResult) {
        return OperationResult<void>::success();
    } else {
        return OperationResult<void>::failure("系统异常：账单数据写入数据库失败");
    }
}

// ==========================================
// 删除账单逻辑
// ==========================================
/**
 * @brief 删除指定账单业务方法
 * 
 * 执行物理或逻辑删除操作，先校验账单ID有效性，再调用数据层执行删除。
 * 
 * @param billId 待删除账单的流水号（必须为正整数）
 * @return OperationResult<void> 业务操作结果
 *         - 成功：无数据返回
 *         - 失败：包含错误描述信息（无效ID、记录不存在或数据库锁定）
 * 
 * @note billId 应为强类型 BillID，当前使用 int 需后续重构
 */
OperationResult<void> BillManager::deleteBill(int billId) {
    // 1. 入参基础校验
    if (billId <= 0) {
        return OperationResult<void>::failure("删除失败：无效的账单流水号");
    }

    // 2. 传递给数据层执行物理或逻辑删除
    bool dbResult = DatabaseManager::deleteBill(billId);
    
    if (dbResult) {
        return OperationResult<void>::success();
    } else {
        return OperationResult<void>::failure("操作失败：未找到对应账单或数据库锁定");
    }
}

// ==========================================
// 查询单条账单逻辑
// ==========================================
/**
 * @brief 查询单条账单业务方法
 * 
 * 根据账单流水号查询单条账单记录，先校验ID有效性，再调用数据层查询。
 * 
 * @param billId 待查询账单的流水号（必须为正整数）
 * @return OperationResult<Bill> 业务操作结果
 *         - 成功：携带查询到的 Bill 对象
 *         - 失败：包含错误描述信息（无效ID或记录不存在）
 * 
 * @note billId 应为强类型 BillID，当前使用 int 需后续重构
 * @note 数据层通过引用传出 Bill 对象，避免值拷贝
 */
OperationResult<Bill> BillManager::getBill(int billId) {
    if (billId <= 0) {
        return OperationResult<Bill>::failure("查询失败：无效的账单流水号");
    }

    Bill resultBill;
    // 假设 data 层的 getBill 支持通过引用/指针传出数据，并返回 bool 代表命中状态
    bool exists = DatabaseManager::getBill(billId, resultBill); 
    
    if (exists) {
        return OperationResult<Bill>::success(resultBill);
    } else {
        return OperationResult<Bill>::failure("查询失败：所请求的账单记录不存在");
    }
}

// ==========================================
// 查询全量账单逻辑
// ==========================================
/**
 * @brief 查询全量账单业务方法
 * 
 * 直接透传数据层的全量查询结果，不进行额外业务校验。
 * 全量查询通常由 UI 层发起，用于列表展示或数据导出。
 * 
 * @return OperationResult<QList<Bill>> 业务操作结果
 *         - 成功：携带所有账单记录的 QList 集合
 *         - 失败：理论上不会失败，但保留扩展性
 * 
 * @note 大数据量时建议增加分页或过滤参数，避免性能问题
 * @note 返回空列表表示数据库中没有账单记录，非错误状态
 */
OperationResult<QList<Bill>> BillManager::getAllBills() {
    // 全量查询通常由 UI 层发起，直接透传给数据层读取
    QList<Bill> bills = DatabaseManager::getAllBills();
    
    // 返回成功结果携带数据集
    return OperationResult<QList<Bill>>::success(bills);
}