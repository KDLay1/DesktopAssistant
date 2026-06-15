/**
 * @file CategoryID.h
 * @brief 分类ID模型 - 强类型ID，采用16位编码表示记账分类
 *
 * 所属分层: Models（领域模型层）
 * 依赖关系: 仅依赖 QtCore，无项目内依赖
 * 设计目的: 提供不可变的分类标识符，支持一级/二级分类、收支方向、仅流水标记等语义
 *
 * 编码结构（16位）:
 *   [inOutFlag:4][primary:4][flowOnlyFlag:4][secondary:4]
 *   - inOutFlag: 收支方向标记 (0x0=支出, 0xF=收入)
 *   - primary: 一级分类编号 (0-15)
 *   - flowOnlyFlag: 仅流水标记 (0x0=非仅流水, 0xF=仅流水)
 *   - secondary: 二级分类编号 (0-15)
 *
 * @author tys
 * @date 2026-06-08
 */
#ifndef MODELS_CATEGORYID_H
#define MODELS_CATEGORYID_H

#include <QtGlobal>

/**
 * @brief 分类ID类 - 强类型标识符，封装16位编码的分类ID
 *
 * 设计目的:
 * - 提供类型安全的分类ID，避免原始整数误用
 * - 支持一级/二级分类的层次结构
 * - 支持收支方向（收入/支出）和仅流水标记
 * - 提供丰富的静态格式检查和实例状态判定方法
 *
 * 使用约束:
 * - 一级分类编号范围: 0-15 (PrimaryCategoryCapacity=16)
 * - 二级分类编号范围: 0-15 (SecondaryCategoryCapacity=15)
 * - 收支方向: 0x0=支出, 0xF=收入
 * - 仅流水标记: 0x0=非仅流水, 0xF=仅流水
 *
 * @note 该类为不可变对象，所有成员变量均为私有且无setter
 */
class CategoryID {
public:
    /** @brief 一级分类容量上限（0-15共16个） */
    static constexpr int PrimaryCategoryCapacity = 0x10;
    /** @brief 二级分类容量上限（0-14共15个） */
    static constexpr int SecondaryCategoryCapacity = 0xF;

    /**
     * @brief 从原始字段值构造分类ID（一级或二级）
     * @param inOutFlag 收支方向标记（0x0=支出, 0xF=收入）
     * @param primaryNumber 一级分类编号（0-15）
     * @param flowOnlyFlag 仅流水标记（0x0=非仅流水, 0xF=仅流水）
     * @param secondaryNumber 二级分类编号（0-15，一级分类时传0）
     * @note 不进行参数范围校验，调用方需确保参数合法
     */
    CategoryID(int inOutFlag, int primaryNumber, int flowOnlyFlag, int secondaryNumber)
        : m_inOutFlag(inOutFlag), m_primary(primaryNumber), 
          m_flowOnlyFlag(flowOnlyFlag), m_secondary(secondaryNumber) {}

    /**
     * @brief 从布尔语义参数构造分类ID（一级或二级）
     * @param isInflow true=收入, false=支出
     * @param primaryNumber 一级分类编号（0-15）
     * @param isFlowOnly true=仅流水, false=非仅流水
     * @param secondaryNumber 二级分类编号（0-15，一级分类时传0）
     * @note 将布尔值转换为对应的16进制标记后委托给4int构造函数
     */
    CategoryID(bool isInflow, int primaryNumber, bool isFlowOnly, int secondaryNumber)
        : CategoryID(isInflow ? 0xF : 0x0, primaryNumber, isFlowOnly ? 0xF : 0x0, secondaryNumber) {}

    /**
     * @brief 从原始字段值构造一级分类ID
     * @param inOutFlag 收支方向标记（0x0=支出, 0xF=收入）
     * @param primaryNumber 一级分类编号（0-15）
     * @note 二级编号和仅流水标记自动置0
     */
    CategoryID(int inOutFlag, int primaryNumber)
        : CategoryID(inOutFlag, primaryNumber, 0, 0) {}

    /**
     * @brief 从布尔语义参数构造一级分类ID
     * @param isInflow true=收入, false=支出
     * @param primaryNumber 一级分类编号（0-15）
     * @note 二级编号和仅流水标记自动置0
     */
    CategoryID(bool isInflow, int primaryNumber)
        : CategoryID(isInflow ? 0xF : 0x0, primaryNumber, 0, 0) {}

    /**
     * @brief 从16位编码解析构造分类ID
     * @param id 16位编码，格式: [inOutFlag:4][primary:4][flowOnlyFlag:4][secondary:4]
     * @note 通过位运算提取各字段后委托给4int构造函数
     */
    CategoryID(quint16 id)
        : CategoryID((id >> 12) & 0xF, (id >> 8) & 0xF, (id >> 4) & 0xF, id & 0xF) {}

    // ==================== Getter 方法 ====================

    /** @brief 获取收支方向标记（0x0=支出, 0xF=收入） */
    int inOutFlag() const { return m_inOutFlag; }
    /** @brief 获取一级分类编号（0-15） */
    int primary() const { return m_primary; }
    /** @brief 获取仅流水标记（0x0=非仅流水, 0xF=仅流水） */
    int flowOnlyFlag() const { return m_flowOnlyFlag; }
    /** @brief 获取二级分类编号（0-15，一级分类时为0） */
    int secondary() const { return m_secondary; }

    // ==================== 动态计算属性 ====================

    /**
     * @brief 获取完整的16位编码
     * @return 组合后的16位编码
     * @note 编码格式: [inOutFlag:4][primary:4][flowOnlyFlag:4][secondary:4]
     */
    quint16 id() const { 
        return static_cast<quint16>((m_inOutFlag << 12) | (m_primary << 8) | (m_flowOnlyFlag << 4) | m_secondary); 
    }

    /**
     * @brief 获取一级分类ID（高8位）
     * @return 高8位编码，低8位置0
     * @note 用于获取所属的一级分类标识
     */
    quint16 primaryId() const { return static_cast<quint16>(id() & 0xFF00); }

    // ==================== 静态格式验证方法 ====================

    /**
     * @brief 检查16位编码的格式是否合法
     * @param id 待检查的16位编码
     * @return true=格式合法, false=格式非法
     * @note 仅检查收支方向和仅流水标记的合法性，不检查编号范围
     */
    static bool isValidFormat(quint16 id) {
        int inoutFlag = (id >> 12) & 0xF;
        int flowOnlyFlag = (id >> 4) & 0xF;
        return (inoutFlag == 0x0 || inoutFlag == 0xF) && (flowOnlyFlag == 0x0 || flowOnlyFlag == 0xF);
    }

    /** @brief 从16位编码中提取一级分类ID（高8位） */
    static quint16 getPrimaryId(quint16 id) { return static_cast<quint16>(id & 0xFF00); }

    /** @brief 检查16位编码是否表示一级分类（低8位为0） */
    static bool isPrimaryFormat(quint16 id) { return (id & 0xFF) == 0x00; }
    /** @brief 检查16位编码是否表示二级分类（低8位非0） */
    static bool isSecondaryFormat(quint16 id) { return (id & 0xFF) != 0x00; }
    /** @brief 检查16位编码是否表示收入（收支标记为0xF） */
    static bool isInflowFormat(quint16 id) { return ((id >> 12) & 0xF) == 0xF; }
    /** @brief 检查16位编码是否表示支出（收支标记为0x0） */
    static bool isOutflowFormat(quint16 id) { return ((id >> 12) & 0xF) == 0x0; }
    /** @brief 检查16位编码是否表示仅流水（仅流水标记为0xF） */
    static bool isFlowOnlyFormat(quint16 id) { return ((id >> 4) & 0xF) == 0xF; }
    /** @brief 检查16位编码是否表示非仅流水（仅流水标记为0x0） */
    static bool isNotFlowOnlyFormat(quint16 id) { return ((id >> 4) & 0xF) == 0x0; }
    /** @brief 检查16位编码是否表示扩展分类（收支=0且仅流水=0） */
    static bool isExtendFormat(quint16 id) { return ((id >> 12) & 0xF) == 0x0 && ((id >> 4) & 0xF) == 0x0; }
    /** @brief 检查16位编码是否表示收入非仅流水（收支=0xF且仅流水=0） */
    static bool isIncomeFormat(quint16 id) { return ((id >> 12) & 0xF) == 0xF && ((id >> 4) & 0xF) == 0x0; }
    /** @brief 检查16位编码是否表示支出仅流水（收支=0且仅流水=0xF） */
    static bool isOutflowOnlyFormat(quint16 id) { return ((id >> 12) & 0xF) == 0x0 && ((id >> 4) & 0xF) == 0xF; }
    /** @brief 检查16位编码是否表示收入仅流水（收支=0xF且仅流水=0xF） */
    static bool isInflowOnlyFormat(quint16 id) { return ((id >> 12) & 0xF) == 0xF && ((id >> 4) & 0xF) == 0xF; }

    // ==================== 实例状态判定方法 ====================

    /** @brief 判断当前分类是否为一级分类（仅流水和二级均为0） */
    bool isPrimary() const { return m_flowOnlyFlag == 0x0 && m_secondary == 0x0; }
    /** @brief 判断当前分类是否为二级分类（非一级分类） */
    bool isSecondary() const { return !isPrimary(); }
    /** @brief 判断当前分类是否为收入（收支标记为0xF） */
    bool isInflow() const { return m_inOutFlag == 0xF; }
    /** @brief 判断当前分类是否为支出（收支标记为0x0） */
    bool isOutflow() const { return m_inOutFlag == 0x0; }
    /** @brief 判断当前分类是否为仅流水（仅流水标记为0xF） */
    bool isFlowOnly() const { return m_flowOnlyFlag == 0xF; }
    /** @brief 判断当前分类是否为非仅流水（仅流水标记为0x0） */
    bool isNotFlowOnly() const { return m_flowOnlyFlag == 0x0; }
    /** @brief 判断当前分类是否为扩展分类（收支=0且仅流水=0） */
    bool isExtend() const { return m_inOutFlag == 0x0 && m_flowOnlyFlag == 0x0; }
    /** @brief 判断当前分类是否为收入非仅流水（收支=0xF且仅流水=0） */
    bool isIncome() const { return m_inOutFlag == 0xF && m_flowOnlyFlag == 0x0; }
    /** @brief 判断当前分类是否为支出仅流水（收支=0且仅流水=0xF） */
    bool isOutflowOnly() const { return m_inOutFlag == 0x0 && m_flowOnlyFlag == 0xF; }
    /** @brief 判断当前分类是否为收入仅流水（收支=0xF且仅流水=0xF） */
    bool isInflowOnly() const { return m_inOutFlag == 0xF && m_flowOnlyFlag == 0xF; }

    /**
     * @brief 综合检查当前分类ID是否合法
     * @return true=合法, false=非法
     * @note 检查条件：
     *       1. 收支方向合法（收入或支出）
     *       2. 仅流水标记合法（仅流水或非仅流水）
     *       3. 一级编号不超过容量上限
     *       4. 二级编号不超过容量上限
     */
    bool isValid() const {
        return (isInflow() || isOutflow()) && (isFlowOnly() || isOutflowOnly()) &&
               m_primary <= PrimaryCategoryCapacity && m_secondary <= SecondaryCategoryCapacity;
    }

    // ==================== 隐式转换与比较操作符 ====================

    /**
     * @brief 隐式转换为16位编码
     * @return 当前分类ID的16位编码
     * @note 允许CategoryID直接用于需要quint16的上下文
     */
    operator quint16() const { return id(); }

    /** @brief 相等比较（基于16位编码） */
    bool operator==(const CategoryID& other) const { return id() == other.id(); }
    /** @brief 不等比较（基于16位编码） */
    bool operator!=(const CategoryID& other) const { return id() != other.id(); }

private:
    int m_inOutFlag;      /**< 收支方向标记（0x0=支出, 0xF=收入） */
    int m_primary;        /**< 一级分类编号（0-15） */
    int m_flowOnlyFlag;   /**< 仅流水标记（0x0=非仅流水, 0xF=仅流水） */
    int m_secondary;      /**< 二级分类编号（0-15，一级分类时为0） */
};

#endif // MODELS_CATEGORYID_H