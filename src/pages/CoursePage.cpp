#include "CoursePage.h"
#include "../app/Theme.h"
#include "../core/CourseRepository.h"
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
#include <QTime>
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
    summaryLabel->setStyleSheet(Theme::subtleTextStyle());

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

    addclassButton->setProperty("buttonRole", "peach");
    addtodoButton->setProperty("buttonRole", "rose");
    deleteButton->setProperty("buttonRole", "neutral");
    finishButton->setProperty("buttonRole", "mint");
    exportButton->setProperty("buttonRole", "sunny");
    todayButton->setProperty("buttonRole", "lavender");
    nextWeekButton->setProperty("buttonRole", "lavender");

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
    dialog.setStyleSheet(Theme::dialogStyle());

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
    courseLabel->setStyleSheet("color:#5f5654;");
    teacherLabel->setStyleSheet("color:#5f5654;");
    countLabel->setStyleSheet("color:#5f5654;");
    startDateLabel->setStyleSheet("color:#5f5654;");
    endDateLabel->setStyleSheet("color:#5f5654;");

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
        label->setStyleSheet("color:#5f5654; font-weight:bold;");
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
            QLabel *indexLabel = new QLabel(QString("第%1次").arg(i + 1), &dialog);
            indexLabel->setStyleSheet(
                "color:#5f5654;"
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

    if (startDateEdit->date() > endDateEdit->date()) {
        QMessageBox::warning(this, "错误", "结课日期不能早于开课日期。");
        return;
    }

    for (int i = 0; i < startTimeEdits.size(); i++) {
        QString startTime = startTimeEdits[i]->text().trimmed();
        QString endTime = endTimeEdits[i]->text().trimmed();
        QTime start = QTime::fromString(startTime, "HH:mm");
        QTime end = QTime::fromString(endTime, "HH:mm");
        QStringList validStarts = {
            "08:00", "08:50", "09:50", "10:40", "11:30",
            "14:05", "14:55", "15:45", "16:40", "17:30",
            "18:30", "19:20", "20:10"
        };
        QStringList validEnds = {
            "08:45", "09:35", "10:35", "11:25", "12:15",
            "14:50", "15:40", "16:30", "17:25", "18:15",
            "19:15", "20:05", "20:55"
        };

        if (!start.isValid() || !end.isValid()) {
            QMessageBox::warning(this, "错误", "时间请按 HH:mm 格式填写，例如 08:00。");
            return;
        }
        if (start >= end) {
            QMessageBox::warning(this, "错误", "结束时间必须晚于开始时间。");
            return;
        }
        if (!validStarts.contains(startTime) || !validEnds.contains(endTime)) {
            QMessageBox::warning(this, "错误", "课程时间需要和课表中的节次时间一致。");
            return;
        }
    }

    QList<CourseRecord> courses;
    for (int i = 0; i < weekdayBoxes.size(); i++) {
        QString weekday = weekdayBoxes[i]->currentText();
        CourseRecord course;
        course.name = courseName;
        course.teacher = teacher;
        course.weekday = QStringList({"周一","周二","周三","周四","周五","周六","周日"})
                             .indexOf(weekday) + 1;
        course.startTime = startTimeEdits[i]->text().trimmed();
        course.endTime = endTimeEdits[i]->text().trimmed();
        course.location = locationEdits[i]->text().trimmed();
        course.startDate = startDateEdit->date().toString("yyyy-MM-dd");
        course.endDate = endDateEdit->date().toString("yyyy-MM-dd");
        courses.append(course);
    }

    // 多条上课时间由仓库放在同一个事务中保存
    QString error;
    if (!CourseRepository::addCourses(courses, &error)) {
        QMessageBox::warning(this, "错误", "添加课程失败：" + error);
        return;
    }

    refreshScheduleTable();
    updateSummary();
}

void CoursePage::addTask()
{
    bool ok = false;
    QString courseName = QInputDialog::getText(this, "课程", "所属课程：", QLineEdit::Normal, "", &ok);
    if (!ok || courseName.trimmed().isEmpty()) return;

    QString taskTitle = QInputDialog::getText(this, "任务", "任务名称：", QLineEdit::Normal, "", &ok);
    if (!ok || taskTitle.trimmed().isEmpty()) return;

    QString deadline = QInputDialog::getText(this, "截止时间", "截止时间，例如 2026-06-01 23:59：", QLineEdit::Normal, QDate::currentDate().toString("yyyy-MM-dd") + " 23:59", &ok);
    if (!ok || deadline.trimmed().isEmpty()) return;
    if (!QDateTime::fromString(deadline.trimmed(), "yyyy-MM-dd HH:mm").isValid()) {
        QMessageBox::warning(this, "错误", "截止时间格式不正确，请按 yyyy-MM-dd HH:mm 填写。");
        return;
    }

    QString priority = QInputDialog::getItem(this, "优先级", "优先级：", {"高", "中", "低"}, 1, false, &ok);
    if (!ok) return;

    QString description = QInputDialog::getText(this, "备注", "备注/说明：", QLineEdit::Normal, "", &ok);
    if (!ok) return;

    CourseTaskRecord task;
    task.courseName = courseName.trimmed();
    task.title = taskTitle.trimmed();
    task.description = description.trimmed();
    task.deadline = deadline.trimmed();
    task.priority = priorityValue(priority);
    task.status = "未完成";

    QString error;
    if (!CourseRepository::addTask(task, &error)) {
        QMessageBox::warning(this, "错误", "添加DDL失败：" + error);
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

        QString error;
        if (!CourseRepository::deleteTask(id, &error)) {
            QMessageBox::warning(this, "错误", "删除DDL失败：" + error);
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

    QList<int> courseIds;
    for (const QString &id : ids) {
        courseIds.append(id.toInt());
    }

    QString error;
    if (!CourseRepository::deleteCourses(courseIds, &error)) {
        QMessageBox::warning(this, "错误", "删除课程失败：" + error);
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

    QString error;
    if (!CourseRepository::updateTaskStatus(id, newStatus, &error)) {
        QMessageBox::warning(this, "错误", "更新任务状态失败：" + error);
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

    const QList<CourseRecord> courses = CourseRepository::allCourses();
    for (const CourseRecord &course : courses) {
        out << "课程," << course.name << ","
            << course.startTime << "-" << course.endTime << ","
            << course.location << ","
            << course.teacher << ","
            << weekdayText(course.weekday) << ",\n";
    }

    const QList<CourseTaskRecord> tasks = CourseRepository::tasksForExport();
    for (const CourseTaskRecord &task : tasks) {
        out << "DDL," << task.courseName << ","
            << task.title << ","
            << task.deadline << ","
            << priorityText(task.priority) << ","
            << task.status << ","
            << task.description << "\n";
    }

    file.close();
    QMessageBox::information(this, "成功", "课程表和DDL已导出。\n" + fileName);
}

void CoursePage::refreshScheduleTable(const QDate &date)
{
    scheduleTable->clearContents();

    QString error;
    const QList<CourseRecord> courses = CourseRepository::coursesForDate(date, &error);
    if (!error.isEmpty()) {
        QMessageBox::warning(this, "错误", "查询课程失败：" + error);
        return;
    }

    for (const CourseRecord &course : courses) {
        int col = course.weekday - 1;
        if (col < 0 || col > 6) continue;

        int startRow = timeToRow(course.startTime);
        int endRow = endTimeToRow(course.endTime);

        if (startRow < 0 || endRow < 0 || endRow < startRow) {
            continue;
        }

        QString text = course.name + "\n" + course.startTime + "-" + course.endTime;
        if (!course.location.isEmpty()) text += "\n@" + course.location;
        if (!course.teacher.isEmpty()) text += "\n" + course.teacher;

        for (int row = startRow; row <= endRow; row++)
        {
            QTableWidgetItem *oldItem = scheduleTable->item(row, col);

            if (oldItem) {
                oldItem->setText(oldItem->text() + "\n---\n" + text);

                QString ids = oldItem->data(Qt::UserRole).toString();

                if (!ids.isEmpty()) {
                    ids += ",";
                }

                ids += QString::number(course.id);

                oldItem->setData(Qt::UserRole, ids);
                oldItem->setBackground(Theme::conflictCellColor());
            } else {
                QTableWidgetItem *item = new QTableWidgetItem(text);
                item->setTextAlignment(Qt::AlignCenter);
                item->setData(Qt::UserRole, QString::number(course.id));
                item->setBackground(courseColor(course.name));
                scheduleTable->setItem(row, col, item);
            }
        }
    }
}

void CoursePage::refreshTaskTable()
{
    taskTable->setRowCount(0);

    const QList<CourseTaskRecord> tasks = CourseRepository::allTasks();
    int row = 0;
    for (const CourseTaskRecord &task : tasks) {
        taskTable->insertRow(row);

        QColor color = courseColor(task.courseName);

        QTableWidgetItem *courseItem = new QTableWidgetItem(task.courseName);
        courseItem->setData(Qt::UserRole, task.id);

        taskTable->setItem(row, 0, courseItem);
        taskTable->setItem(row, 1, new QTableWidgetItem(task.title));
        taskTable->setItem(row, 2, new QTableWidgetItem(task.deadline));
        taskTable->setItem(row, 3, new QTableWidgetItem(priorityText(task.priority)));
        taskTable->setItem(row, 4, new QTableWidgetItem(task.status));
        taskTable->setItem(row, 5, new QTableWidgetItem(task.description));

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
    int unfinishedCount = CourseRepository::unfinishedTaskCount();
    int todayCourseCount = CourseRepository::courseCount(QDate::currentDate());

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
    const auto &colors = Theme::courseColors();
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
    if (time >= "20:10" && time <= "20:55") return 12;

    return -1;
}

int CoursePage::endTimeToRow(const QString &time) const
{
    return timeToRow(time);
}

