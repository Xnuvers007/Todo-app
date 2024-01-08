#include <QApplication>
#include <QMainWindow>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QCheckBox>
#include <QLabel>
#include <QTime>
#include <QTimer>
#include <QMessageBox>
#include <QComboBox>
#include <QUndoStack>
#include <QUndoView>
#include <QGroupBox>
#include <QSplashScreen>

class TaskItem : public QListWidgetItem {
public:
    TaskItem(const QString &text, const QString &priority = "Low")
        : QListWidgetItem(text), taskPriority(priority), completed(false) {}

    QString taskPriority;
    bool completed;
};

void filterTasks(QListWidget* listWidget, const QString& keyword) {
    for (int i = 0; i < listWidget->count(); ++i) {
        QListWidgetItem* item = listWidget->item(i);
        if (item->text().contains(keyword, Qt::CaseInsensitive)) {
            item->setHidden(false);
        } else {
            item->setHidden(true);
        }
    }
}

void loadTasksFromFile(QListWidget* listWidget) {
    QFile file("tasks.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString task = in.readLine();
            if (!task.isEmpty()) {
                listWidget->addItem(new TaskItem(task));
            }
        }
        file.close();
    }
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Menampilkan splash screen
    QSplashScreen splash(QPixmap("./loading.gif"));
    splash.show();

    QMainWindow mainWindow;
    QWidget *centralWidget = new QWidget(&mainWindow);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    QLabel *timeLabel = new QLabel(centralWidget);
    timeLabel->setAlignment(Qt::AlignHCenter);
    layout->addWidget(timeLabel);

    QListWidget *todoList = new QListWidget(centralWidget);
    layout->addWidget(todoList);

    QLineEdit *taskInput = new QLineEdit(centralWidget);
    taskInput->setPlaceholderText("Enter task...");
    layout->addWidget(taskInput);

    QPushButton *addButton = new QPushButton("Add Task", centralWidget);
    layout->addWidget(addButton);

    QPushButton *removeButton = new QPushButton("Remove Selected Task", centralWidget);
    layout->addWidget(removeButton);

    QCheckBox *themeToggle = new QCheckBox("Dark Theme", centralWidget);
    layout->addWidget(themeToggle);

    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);

    QPalette lightPalette = app.palette();

    QObject::connect(themeToggle, &QCheckBox::toggled, [&](bool checked) {
        if (checked) {
            app.setPalette(darkPalette);
        } else {
            app.setPalette(lightPalette);
        }
    });

    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, [&]() {
        QTime currentTime = QTime::currentTime();
        QString timeText = currentTime.toString("hh:mm:ss");
        timeLabel->setText(timeText);
    });
    timer.start(1000);

    loadTasksFromFile(todoList);

    QUndoStack undoStack;
    
    QObject::connect(addButton, &QPushButton::clicked, [&]() {
        QString task = taskInput->text().trimmed();
        // Validasi input sebelum menggunakan data
        if (task.isEmpty()) {
            QMessageBox::critical(&mainWindow, "Error", "Task cannot be empty!");
            return;
        }
        if (!task.isEmpty()) {
            QUndoCommand *addCommand = new QUndoCommand("Add Task");
            TaskItem* newTask = new TaskItem(task);
            todoList->addItem(newTask);
            taskInput->clear();
            addCommand->setText(task);
            undoStack.push(addCommand);

            QFile file("tasks.txt");
            if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
                QTextStream out(&file);
                out << task << endl;
                file.close(); // Close the file after writing
            } else {
                QMessageBox::critical(&mainWindow, "Error", "Failed to save the task!");
                delete newTask; // Clean up the allocated memory
            }

            QMessageBox::information(&mainWindow, "Task Added", "Task has been added successfully.");
            if (!file.isOpen()) {
            delete newTask;
        }
        }
    });

    /*QObject::connect(addButton, &QPushButton::clicked, [&]() {
        QString task = taskInput->text().trimmed();
        if (!task.isEmpty()) {
            QUndoCommand *addCommand = new QUndoCommand("Add Task");
            todoList->addItem(new TaskItem(task));
            taskInput->clear();
            addCommand->setText(task);
            undoStack.push(addCommand);

            QFile file("tasks.txt");
            if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
                QTextStream out(&file);
                out << task << endl;
            }

            QMessageBox::information(&mainWindow, "Task Added", "Task has been added successfully.");
        }
    });
    */

    QLabel *priorityLabel = new QLabel("Task Priority", centralWidget);
    QComboBox *priorityComboBox = new QComboBox(centralWidget);
    priorityComboBox->addItem("Low");
    priorityComboBox->addItem("Medium");
    priorityComboBox->addItem("High");
    layout->addWidget(priorityLabel);
    layout->addWidget(priorityComboBox);

    QLineEdit *searchInput = new QLineEdit(centralWidget);
    searchInput->setPlaceholderText("Search tasks");
    layout->addWidget(searchInput);

    QObject::connect(searchInput, &QLineEdit::textChanged, [&]() {
        QString keyword = searchInput->text();
        filterTasks(todoList, keyword);
    });

    QObject::connect(priorityComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int index) {
        QList<QListWidgetItem*> selectedItems = todoList->selectedItems();
        for (QListWidgetItem* item : selectedItems) {
            switch (index) {
                case 0:
                    item->setForeground(QBrush(Qt::green));
                    break;
                case 1:
                    item->setForeground(QBrush(Qt::yellow));
                    break;
                case 2:
                    item->setForeground(QBrush(Qt::red));
                    break;
                default:
                    item->setForeground(QBrush(Qt::black));
            }
        }
    });

    /*QObject::connect(removeButton, &QPushButton::clicked, [&]() {
        QList<QListWidgetItem*> selectedItems = todoList->selectedItems();

        QFile file("tasks.txt");
        if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
            QTextStream in(&file);
            QString fileContents = in.readAll();
            QStringList lines = fileContents.split("\n");

            file.resize(0);

            QStringList updatedLines;

            for (QListWidgetItem* item : selectedItems) {
                QString task = item->text();
                lines.removeAll(task);
                delete item;
            }

            QTextStream out(&file);
            for (const QString& line : lines) {
                if (!line.isEmpty()) {
                    updatedLines << line;
                }
            }
            out << updatedLines.join("\n");
        }
    });
    */
    QObject::connect(removeButton, &QPushButton::clicked, [&]() {
        QList<QListWidgetItem*> selectedItems = todoList->selectedItems();
        
        if (selectedItems.isEmpty()) {
        QMessageBox::critical(&mainWindow, "Error", "Remove selected task: Nothing selected!");
        return;
    }

        QFile file("tasks.txt");
        if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
            QTextStream in(&file);
            QString fileContents = in.readAll();
            QStringList lines = fileContents.split("\n");

            file.resize(0);

            QStringList updatedLines;

            for (QListWidgetItem* item : selectedItems) {
                QString task = item->text();
                lines.removeAll(task);
                delete item;
            }

            QTextStream out(&file);
            for (const QString& line : lines) {
                if (!line.isEmpty()) {
                    updatedLines << line;
                }
            }
            out << updatedLines.join("\n");
            file.close(); // Close the file after updating
        }
    });

    QUndoView *undoView = new QUndoView(&undoStack);
    undoView->setWindowTitle("Undo/Redo History");
    undoView->setParent(centralWidget);
    layout->addWidget(undoView);

    QLabel *taskCountLabel = new QLabel("Task Count:", centralWidget);
    layout->addWidget(taskCountLabel);

    QLabel *totalTasksLabel = new QLabel("Total Tasks: 0", centralWidget);
    layout->addWidget(totalTasksLabel);

    QObject::connect(&undoStack, &QUndoStack::indexChanged, [&]() {
        totalTasksLabel->setText("Total Tasks: " + QString::number(todoList->count()));
        int completedTasks = 0;
        for (int i = 0; i < todoList->count(); ++i) {
            TaskItem* taskItem = dynamic_cast<TaskItem*>(todoList->item(i));
            if (taskItem && taskItem->completed) {
                ++completedTasks;
            }
        }
    });

    mainWindow.setCentralWidget(centralWidget);
    mainWindow.setWindowTitle("To-Do List Manager by Xnuvers007");
    
    // Menampilkan splash screen selama 5 detik
    QTimer::singleShot(5000, &splash, &QSplashScreen::close);
    
    // Menampilkan jendela utama setelah splash screen ditutup
    QTimer::singleShot(5000, &mainWindow, &QMainWindow::show);

    QMessageBox::information(&mainWindow, "Welcome", "To-Do List Manager is ready to use!");

    return app.exec();
}
