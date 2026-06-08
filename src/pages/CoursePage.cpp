#include "CoursePage.h"
#include "../core/DatabaseManager.h"
#include <QLineEdit>
#include <QStringList>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>
#include <QTabWidget>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDate>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QAbstractItemView>
#include <QDialog>
#include <QFormLayout>
#include <QGridLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QSpinBox>
#include <QDateEdit>
#include <QApplication>

CoursePage::CoursePage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    setupConnection();
    refreshScheduleTable();
    refreshTaskTable();
    updateSummary();
}

void CoursePage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 25, 30, 25);
    mainLayout->setSpacing(14);

    titleLabel = new QLabel("课程表与DDL管理", this);
    QFont titleFont;
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    summaryLabel = new QLabel(this);

    addclassButton = new QPushButton("添加课程", this);
    addtodoButton = new QPushButton("添加DDL", this);
    deleteButton = new QPushButton("删除选中项", this);
    finishButton = new QPushButton("完成/取消完成", this);
    exportButton = new QPushButton("导出CSV", this);
    todayButton = new QPushButton("今日课表", this);
    nextWeekButton = new QPushButton("下周课表", this);
    scheduleDateEdit = new QDateEdit(this);
    scheduleDateEdit->setCalendarPopup(true);
    scheduleDateEdit->setDate(QDate::currentDate());

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(addclassButton);
    buttonLayout->addWidget(addtodoButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addWidget(finishButton);
    buttonLayout->addWidget(exportButton);
    buttonLayout->addWidget(new QLabel("查询日期：", this));
    buttonLayout->addWidget(scheduleDateEdit);
    buttonLayout->addWidget(todayButton);
    buttonLayout->addWidget(nextWeekButton);
    buttonLayout->addStretch();

    tabWidget = new QTabWidget(this);

    scheduleTable = new QTableWidget(this);
    scheduleTable->setColumnCount(7);
    scheduleTable->setRowCount(13);
    scheduleTable->setHorizontalHeaderLabels({"周一", "周二", "周三", "周四", "周五", "周六", "周日"});
    scheduleTable->setVerticalHeaderLabels({"08:00-08:45", "08:50-09:35", "09:50-10:35", "10:40-11:25", "11:30-12:15", "14:05-14:50","14:55-15:40","15:45-16:30","16:40-17:25","17:30-18:15","18:30-19:15","19:20-20:05","20:10-20:55"});
    scheduleTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    scheduleTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    scheduleTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    scheduleTable->setSelectionBehavior(QAbstractItemView::SelectItems);

    taskTable = new QTableWidget(this);
    taskTable->setColumnCount(6);
    taskTable->setHorizontalHeaderLabels({"课程", "任务", "截止时间", "优先级", "状态", "备注"});
    taskTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    taskTable->horizontalHeader()->setStretchLastSection(true);
    taskTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    taskTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    tabWidget->addTab(scheduleTable, "每周课程表");
    tabWidget->addTab(taskTable, "DDL任务");

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(summaryLabel);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(tabWidget);
}

void CoursePage::setupConnection()
{
    connect(addclassButton, &QPushButton::clicked,
            this, &CoursePage::addCourse);
    
    connect(addtodoButton, &QPushButton::clicked,
            this, &CoursePage::addTask);
    
    connect(deleteButton, &QPushButton::clicked,
            this, &CoursePage::deleteSelectedItem);
    
    connect(finishButton, &QPushButton::clicked,
            this, &CoursePage::toggleTaskStatus);
    
    connect(exportButton, &QPushButton::clicked,
            this, &CoursePage::exportToCSV);

    connect(scheduleDateEdit, &QDateEdit::dateChanged,
            this, [this](const QDate &date) {
                refreshScheduleTable(date);
            });

    connect(todayButton, &QPushButton::clicked,
            this, [this]() {
                scheduleDateEdit->setDate(QDate::currentDate());
                refreshScheduleTable(QDate::currentDate());
            });

    connect(nextWeekButton, &QPushButton::clicked,
            this, [this]() {
                QDate nextWeek = QDate::currentDate().addDays(7);
                scheduleDateEdit->setDate(nextWeek);
                refreshScheduleTable(nextWeek);
            });
}

void CoursePage::addCourse()
{
    QDialog dialog(this);
    dialog.setWindowTitle("添加课程");
    dialog.resize(850, 420);

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);

    QLineEdit *courseNameEdit = new QLineEdit(&dialog);
    courseNameEdit->setPlaceholderText("请输入课程名称，例如：高等数学");
    courseNameEdit->setStyleSheet(
        "QLineEdit {"
        "background:white;"
        "color:black;"
        "padding:4px;"
        "border:1px solid gray;"
        "}"
    );

    QLineEdit *teacherEdit = new QLineEdit(&dialog);
    teacherEdit->setPlaceholderText("请输入任课老师，例如：张平文");
    teacherEdit->setStyleSheet(
        "QLineEdit {"
        "background:white;"
        "color:black;"
        "padding:4px;"
        "border:1px solid gray;"
        "}"
    );

    QSpinBox *countSpinBox = new QSpinBox(&dialog);
    countSpinBox->setRange(1, 10);
    countSpinBox->setValue(2);
    countSpinBox->setStyleSheet(
        "QSpinBox {"
        "background:white;"
        "color:black;"
        "padding:2px;"
        "}"
    );

    QDateEdit *startDateEdit = new QDateEdit(&dialog);
    startDateEdit->setCalendarPopup(true);
    startDateEdit->setDate(QDate::currentDate());

    QDateEdit *endDateEdit = new QDateEdit(&dialog);
    endDateEdit->setCalendarPopup(true);
    endDateEdit->setDate(QDate::currentDate().addMonths(4));

    QFormLayout *topLayout = new QFormLayout;
    QLabel *courseLabel = new QLabel("课程名称：", &dialog);
    QLabel *teacherLabel = new QLabel("任课老师：", &dialog);
    QLabel *countLabel = new QLabel("一周上课次数：", &dialog);
    QLabel *startDateLabel = new QLabel("开始日期：", &dialog);
    QLabel *endDateLabel = new QLabel("结束日期：", &dialog);
    courseLabel->setStyleSheet("color:white;");
    teacherLabel->setStyleSheet("color:white;");
    countLabel->setStyleSheet("color:white;");
    startDateLabel->setStyleSheet("color:white;");
    endDateLabel->setStyleSheet("color:white;");

    topLayout->addRow(courseLabel, courseNameEdit);
    topLayout->addRow(teacherLabel, teacherEdit);
    topLayout->addRow(countLabel, countSpinBox);
    topLayout->addRow(startDateLabel, startDateEdit);
    topLayout->addRow(endDateLabel, endDateEdit);
    mainLayout->addLayout(topLayout);

    QGridLayout *timeLayout = new QGridLayout;
    timeLayout->setHorizontalSpacing(12);
    timeLayout->setVerticalSpacing(10);

    timeLayout->setColumnMinimumWidth(0, 70);   // 次数
    timeLayout->setColumnMinimumWidth(1, 100);  // 星期
    timeLayout->setColumnMinimumWidth(2, 120);  // 开始时间
    timeLayout->setColumnMinimumWidth(3, 120);  // 结束时间
    timeLayout->setColumnMinimumWidth(4, 160);  // 地点

    QStringList headers = {"次数", "星期", "开始时间", "结束时间", "上课地点"};
    for (int i = 0; i < headers.size(); i++) {
        QLabel *label = new QLabel(headers[i], &dialog);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("color:white; font-weight:bold;");
        timeLayout->addWidget(label, 0, i);
    }

    QVector<QComboBox*> weekdayBoxes;
    QVector<QLineEdit*> startTimeEdits;
    QVector<QLineEdit*> endTimeEdits;
    QVector<QLineEdit*> locationEdits;

    auto rebuildRows = [&]() {
        while (timeLayout->count() > 5) {
            QLayoutItem *item = timeLayout->takeAt(5);
            if (item->widget()) delete item->widget();
            delete item;
        }

        weekdayBoxes.clear();
        startTimeEdits.clear();
        endTimeEdits.clear();
        locationEdits.clear();

        int count = countSpinBox->value();
        for (int i = 0; i < count; i++) {
            QLabel *indexLabel =new QLabel(QString("第%1次").arg(i + 1), &dialog);
            indexLabel->setStyleSheet(
                "color:white;font-weight:bold;"
            );
            indexLabel->setStyleSheet(
                "color:white;"
                "font-size:14px;"
                "font-weight:bold;"
            );

            QComboBox *weekdayBox = new QComboBox(&dialog);
            weekdayBox->addItems({"周一","周二","周三","周四","周五","周六","周日"});
            weekdayBox->setMinimumWidth(90);
            weekdayBox->setFixedHeight(32);
            weekdayBox->setStyleSheet(
                "QComboBox {"
                "background:white;"
                "color:black;"
                "padding:2px;"
                "}"
            );

            QLineEdit *startEdit = new QLineEdit(&dialog);
            startEdit->setPlaceholderText("08:00");
            startEdit->setStyleSheet(
                "QLineEdit {"
                "background:white;"
                "color:black;"
                "padding:4px;"
                "}"
            );

            QLineEdit *endEdit = new QLineEdit(&dialog);
            endEdit->setPlaceholderText("09:35");
            endEdit->setStyleSheet(
                "QLineEdit {"
                "background:white;"
                "color:black;"
                "padding:4px;"
                "}"
            );

            QLineEdit *locationEdit = new QLineEdit(&dialog);
            locationEdit->setPlaceholderText("教室");
            locationEdit->setStyleSheet(
                "QLineEdit {"
                "background:white;"
                "color:black;"
                "padding:4px;"
                "}"
            );
            timeLayout->addWidget(indexLabel, i + 1, 0);
            timeLayout->addWidget(weekdayBox, i + 1, 1);
            timeLayout->addWidget(startEdit, i + 1, 2);
            timeLayout->addWidget(endEdit, i + 1, 3);
            timeLayout->addWidget(locationEdit, i + 1, 4);

            weekdayBoxes.push_back(weekdayBox);
            startTimeEdits.push_back(startEdit);
            endTimeEdits.push_back(endEdit);
            locationEdits.push_back(locationEdit);
        }
    };

    rebuildRows();
    connect(countSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            &dialog, [&](int){ rebuildRows(); });

    mainLayout->addLayout(timeLayout);

    QDialogButtonBox *buttonBox =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    mainLayout->addWidget(buttonBox);
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) return;

    QString courseName = courseNameEdit->text().trimmed();
    QString teacher = teacherEdit->text().trimmed();
    if (courseName.isEmpty()) {
        QMessageBox::warning(this, "错误", "课程名称不能为空。");
        return;
    }
    QStringList newIds;
    for (int i = 0; i < weekdayBoxes.size(); i++) {
        QString weekday = weekdayBoxes[i]->currentText();
        QString startTime = startTimeEdits[i]->text().trimmed();
        QString endTime = endTimeEdits[i]->text().trimmed();
        QString location = locationEdits[i]->text().trimmed();

        if (startTime.isEmpty() || endTime.isEmpty()) {
            QMessageBox::warning(this, "错误", "开始时间和结束时间不能为空。");
            return;
        }

        int weekdayNumber =
            QStringList({"周一","周二","周三","周四","周五","周六","周日"})
                .indexOf(weekday) + 1;

        QSqlQuery query(DatabaseManager::instance().database());
        query.prepare(R"(
            INSERT INTO courses(
                course_name,
                weekday,
                start_time,
                end_time,
                location,
                teacher,
                start_date,
                end_date,
                created_at,
                updated_at
            ) VALUES(
                :course_name,
                :weekday,
                :start_time,
                :end_time,
                :location,
                :teacher,
                :start_date,
                :end_date,
                :created_at,
                :updated_at
            )
        )");

        QString now = QDateTime::currentDateTime().toString(Qt::ISODate);
        query.bindValue(":course_name", courseName);
        query.bindValue(":weekday", weekdayNumber);
        query.bindValue(":start_time", startTime);
        query.bindValue(":end_time", endTime);
        query.bindValue(":location", location);
        query.bindValue(":teacher", teacher);
        query.bindValue(":start_date", startDateEdit->date().toString("yyyy-MM-dd"));
        query.bindValue(":end_date", endDateEdit->date().toString("yyyy-MM-dd"));
        query.bindValue(":created_at", now);
        query.bindValue(":updated_at", now);

        if (!query.exec()) {
            QMessageBox::warning(this, "错误", "添加课程失败：" + query.lastError().text());
            return;
        }
        newIds << QString::number(query.lastInsertId().toInt());
    }
    refreshScheduleTable();
    updateSummary();
}


void CoursePage::addTask()
{
    bool ok = false;
    qApp->setStyleSheet(
        qApp->styleSheet() +
        "QInputDialog QLineEdit { color: black; background-color: white; }"
    );
    QString courseName = QInputDialog::getText(this, "课程", "所属课程：", QLineEdit::Normal, "", &ok);
    if (!ok || courseName.trimmed().isEmpty()) return;

    if (!ok || courseName.trimmed().isEmpty()) return;

    QString taskTitle = QInputDialog::getText(this, "任务", "任务名称：", QLineEdit::Normal, "", &ok);
    if (!ok || taskTitle.trimmed().isEmpty()) return;

    QString deadline = QInputDialog::getText(this, "截止时间", "截止时间，例如 2026-06-01 23:59：", QLineEdit::Normal, QDate::currentDate().toString("yyyy-MM-dd") + " 23:59", &ok);
    if (!ok || deadline.trimmed().isEmpty()) return;

    QString priority = QInputDialog::getItem(this, "优先级", "优先级：", {"高", "中", "低"}, 1, false, &ok);
    if (!ok) return;

    QString description = QInputDialog::getText(this, "备注", "备注/说明：", QLineEdit::Normal, "", &ok);
    if (!ok) return;

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(R"(
        INSERT INTO course_tasks(course_name, task_title, description, deadline, priority, status, created_at, updated_at)
        VALUES(:course_name, :task_title, :description, :deadline, :priority, :status, :created_at, :updated_at)
    )");

    QString now = QDateTime::currentDateTime().toString(Qt::ISODate);
    query.bindValue(":course_name", courseName.trimmed());
    query.bindValue(":task_title", taskTitle.trimmed());
    query.bindValue(":description", description.trimmed());
    query.bindValue(":deadline", deadline.trimmed());
    query.bindValue(":priority", priorityValue(priority));
    query.bindValue(":status", "未完成");
    query.bindValue(":created_at", now);
    query.bindValue(":updated_at", now);

    if (!query.exec()) {
        QMessageBox::warning(this, "错误", "添加DDL失败：" + query.lastError().text());
        return;
    }

    refreshTaskTable();
    updateSummary();
}

void CoursePage::deleteSelectedItem()
{
    if (tabWidget->currentWidget() == taskTable) {
        int row = taskTable->currentRow();
        if (row < 0) {
            QMessageBox::information(this, "提示", "请先在DDL表中选择一行。");
            return;
        }

        int id = taskTable->item(row, 0)->data(Qt::UserRole).toInt();

        if (QMessageBox::question(this, "确认删除", "确定删除这条DDL吗？")
            != QMessageBox::Yes) {
            return;
        }

        QSqlQuery query(DatabaseManager::instance().database());
        query.prepare("DELETE FROM course_tasks WHERE id = :id");
        query.bindValue(":id", id);

        if (!query.exec()) {
            QMessageBox::warning(this, "错误", "删除DDL失败：" + query.lastError().text());
            return;
        }

        refreshTaskTable();
        updateSummary();
        return;
    }

    int row = scheduleTable->currentRow();
    int col = scheduleTable->currentColumn();

    if (row < 0 || col < 0) {
        QMessageBox::information(this, "提示", "请先在课程表中选择一个课程格子。");
        return;
    }

    QTableWidgetItem *cellItem = scheduleTable->item(row, col);

    if (!cellItem) {
        QMessageBox::information(this, "提示", "这个格子没有可删除的课程。");
        return;
    }

    QString idsText = cellItem->data(Qt::UserRole).toString();

    if (idsText.isEmpty()) {
        QMessageBox::information(this, "提示", "这个格子没有可删除的课程。");
        return;
    }

    QStringList ids = idsText.split(",", Qt::SkipEmptyParts);

    if (ids.isEmpty()) {
        QMessageBox::information(this, "提示", "这个格子没有可删除的课程。");
        return;
    }

    if (QMessageBox::question(
            this,
            "确认删除",
            QString("确定删除这个格子内的 %1 条课程记录吗？").arg(ids.size()))
        != QMessageBox::Yes) {
        return;
    }

    QSqlQuery query(DatabaseManager::instance().database());

    query.prepare(QString("DELETE FROM courses WHERE id IN (%1)").arg(ids.join(",")));

    if (!query.exec()) {
        QMessageBox::warning(this, "错误", "删除课程失败：" + query.lastError().text());
        return;
    }

    refreshScheduleTable();
    refreshTaskTable();
    updateSummary();
}

void CoursePage::toggleTaskStatus()
{
    int row = taskTable->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "提示", "请先在DDL任务表中选择一行。");
        return;
    }

    int id = taskTable->item(row, 0)->data(Qt::UserRole).toInt();
    QString oldStatus = taskTable->item(row, 4)->text();
    QString newStatus = (oldStatus == "已完成") ? "未完成" : "已完成";

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("UPDATE course_tasks SET status = :status, updated_at = :updated_at WHERE id = :id");
    query.bindValue(":status", newStatus);
    query.bindValue(":updated_at", QDateTime::currentDateTime().toString(Qt::ISODate));
    query.bindValue(":id", id);

    if (!query.exec()) {
        QMessageBox::warning(this, "错误", "更新任务状态失败：" + query.lastError().text());
        return;
    }

    refreshTaskTable();
    updateSummary();
}

void CoursePage::exportToCSV()
{
    QString fileName = QFileDialog::getSaveFileName(this, "导出CSV", "course_ddl.csv", "CSV Files (*.csv)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法创建文件。请检查路径权限。");
        return;
    }

    QTextStream out(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    out.setCodec("UTF-8");
#endif

    out << "类型,课程,任务/时间,截止/地点,优先级/教师,状态/星期,日期范围/备注\n";

    QSqlQuery courseQuery = DatabaseManager::instance().query(
        "SELECT course_name, weekday, start_time, end_time, location, teacher FROM courses ORDER BY weekday, start_time"
    );
    while (courseQuery.next()) {
        out << "课程," << courseQuery.value(0).toString() << ","
            << courseQuery.value(2).toString() << "-" << courseQuery.value(3).toString() << ","
            << courseQuery.value(4).toString() << ","
            << courseQuery.value(5).toString() << ","
            << weekdayText(courseQuery.value(1).toInt()) << ",\n";
    }

    QSqlQuery taskQuery = DatabaseManager::instance().query(
        "SELECT course_name, task_title, deadline, priority, status, description FROM course_tasks ORDER BY status, deadline, priority"
    );
    while (taskQuery.next()) {
        out << "DDL," << taskQuery.value(0).toString() << ","
            << taskQuery.value(1).toString() << ","
            << taskQuery.value(2).toString() << ","
            << priorityText(taskQuery.value(3).toInt()) << ","
            << taskQuery.value(4).toString() << ","
            << taskQuery.value(5).toString() << "\n";
    }

    file.close();
    QMessageBox::information(this, "成功", "课程表和DDL已导出。\n" + fileName);
}

void CoursePage::refreshScheduleTable(const QDate &date)
{
    scheduleTable->clearContents();

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(
        "SELECT id, course_name, weekday, start_time, end_time, location, teacher "
        "FROM courses "
        "WHERE (start_date IS NULL OR start_date = '' OR start_date <= :date) "
        "AND (end_date IS NULL OR end_date = '' OR end_date >= :date) "
        "ORDER BY weekday, start_time"
    );
    query.bindValue(":date", date.toString("yyyy-MM-dd"));
    if (!query.exec()) {
        QMessageBox::warning(this, "错误", "查询课程失败：" + query.lastError().text());
        return;
    }

    while (query.next()) {
        int id = query.value(0).toInt();
        QString courseName = query.value(1).toString();
        int weekday = query.value(2).toInt();
        QString startTime = query.value(3).toString();
        QString endTime = query.value(4).toString();
        QString location = query.value(5).toString();
        QString teacher = query.value(6).toString();

        int col = weekday - 1;
        if (col < 0 || col > 6) continue;

        int startRow = timeToRow(startTime);
        int endRow = endTimeToRow(endTime);

        if (startRow < 0 || endRow < 0 || endRow < startRow) {
            continue;
        }

        QString text = courseName + "\n" + startTime + "-" + endTime;
        if (!location.isEmpty()) text += "\n@" + location;
        if (!teacher.isEmpty()) text += "\n" + teacher;

        for (int row = startRow; row <= endRow; row++)
        {
            QTableWidgetItem *oldItem = scheduleTable->item(row, col);

            if (oldItem) {
                oldItem->setText(oldItem->text() + "\n---\n" + text);

                QString ids = oldItem->data(Qt::UserRole).toString();

                if (!ids.isEmpty()) {
                    ids += ",";
                }

                ids += QString::number(id);

                oldItem->setData(Qt::UserRole, ids);
                oldItem->setBackground(QColor(220, 220, 220));
            } else {
                QTableWidgetItem *item = new QTableWidgetItem(text);
                item->setTextAlignment(Qt::AlignCenter);
                item->setData(Qt::UserRole, QString::number(id));
                item->setBackground(courseColor(courseName));
                scheduleTable->setItem(row, col, item);
            }
        }
    }
}

void CoursePage::refreshTaskTable()
{
    taskTable->setRowCount(0);

    QSqlQuery query = DatabaseManager::instance().query(
        "SELECT id, course_name, task_title, deadline, priority, status, description "
        "FROM course_tasks ORDER BY status DESC, deadline ASC, priority ASC"
    );

    int row = 0;
    while (query.next()) {
        taskTable->insertRow(row);

        QString courseName = query.value(1).toString();
        QColor color = courseColor(courseName);

        QTableWidgetItem *courseItem = new QTableWidgetItem(courseName);
        courseItem->setData(Qt::UserRole, query.value(0).toInt());

        taskTable->setItem(row, 0, courseItem);
        taskTable->setItem(row, 1, new QTableWidgetItem(query.value(2).toString()));
        taskTable->setItem(row, 2, new QTableWidgetItem(query.value(3).toString()));
        taskTable->setItem(row, 3, new QTableWidgetItem(priorityText(query.value(4).toInt())));
        taskTable->setItem(row, 4, new QTableWidgetItem(query.value(5).toString()));
        taskTable->setItem(row, 5, new QTableWidgetItem(query.value(6).toString()));

        for (int col = 0; col < 6; col++) {
            if (taskTable->item(row, col)) {
                taskTable->item(row, col)->setBackground(color);
            }
        }

        row++;
    }
}

void CoursePage::updateSummary()
{
    QSqlQuery unfinished = DatabaseManager::instance().query(
        "SELECT COUNT(*) FROM course_tasks WHERE status != '已完成'"
    );
    int unfinishedCount = unfinished.next() ? unfinished.value(0).toInt() : 0;

    int todayWeekday = QDate::currentDate().dayOfWeek();
    QSqlQuery todayCourse(DatabaseManager::instance().database());
    todayCourse.prepare("SELECT COUNT(*) FROM courses WHERE weekday = :weekday");
    todayCourse.bindValue(":weekday", todayWeekday);
    int todayCourseCount = 0;
    if (todayCourse.exec() && todayCourse.next()) {
        todayCourseCount = todayCourse.value(0).toInt();
    }

    summaryLabel->setText(QString("剩余DDL：%1件    今日课程：%2节")
                          .arg(unfinishedCount)
                          .arg(todayCourseCount));
}

QString CoursePage::priorityText(int priority) const
{
    if (priority == 1) return "高";
    if (priority == 2) return "中";
    return "低";
}

int CoursePage::priorityValue(const QString &priority) const
{
    if (priority == "高") return 1;
    if (priority == "中") return 2;
    return 3;
}

QString CoursePage::weekdayText(int weekday) const
{
    QStringList weekdays = {"", "周一", "周二", "周三", "周四", "周五", "周六", "周日"};
    if (weekday >= 1 && weekday <= 7) return weekdays[weekday];
    return "未知";
}

QColor CoursePage::courseColor(const QString &courseName) const
{
    static QVector<QColor> colors =
    {
        QColor(52,152,219),   // 蓝
        QColor(231,76,60),    // 红
        QColor(46,204,113),   // 绿
        QColor(155,89,182),   // 紫
        QColor(241,196,15),   // 黄
        QColor(230,126,34),   // 橙
        QColor(26,188,156),   // 青
        QColor(127,140,141)   // 灰
    };

    uint hash = qHash(courseName);

    return colors[hash % colors.size()];
}

int CoursePage::timeToRow(const QString &time) const
{
    if (time >= "08:00" && time <= "08:45") return 0;
    if (time >= "08:50" && time <= "09:35") return 1;
    if (time >= "09:50" && time <= "10:35") return 2;
    if (time >= "10:40" && time <= "11:25") return 3;
    if (time >= "11:30" && time <= "12:15") return 4;
    if (time >= "14:05" && time <= "14:50") return 5;
    if (time >= "14:55" && time <= "15:40") return 6;
    if (time >= "15:45" && time <= "16:30") return 7;
    if (time >= "16:40" && time <= "17:25") return 8;
    if (time >= "17:30" && time <= "18:15") return 9;
    if (time >= "18:30" && time <= "19:15") return 10;
    if (time >= "19:20" && time <= "20:05") return 11;

    return -1;
}

int CoursePage::endTimeToRow(const QString &time) const
{
    return timeToRow(time);
}

